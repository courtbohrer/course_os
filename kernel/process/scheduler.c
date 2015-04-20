#include "../include/scheduler.h"
#include "../include/vm.h"
#include "../include/klibc.h"
#include "../include/process.h"

#define MAX_TASKS 100   // in the future, cap will be removed
#define MAX_ACTIVE_TASKS 4  // in the future, will dynamically change based on load
#define MAX_NICENESS -20
#define MIN_NICENESS 20
#define TASK_STATE_FINSIHED 3
#define TASK_STATE_INACTIVE  2
#define TASK_STATE_ACTIVE 1
#define TASK_STATE_NONE 0
#define MAX(a, b) 	((a > b) ? a : b)
#define MIN(a, b)   ((a < b) ? a : b)

#define OK      1
#define FAIL    0

static char * last_err;
static prq_handle * all_tasks;
static prq_handle * active_tasks;
static sched_task * active_task;
static hmap_handle * all_tasks_map;

// NOTE
// scheduler logic only. not tested

// FIXME
// - add comments
// - register interrupt
// - deregister interrupts instead of flag
// - optimize

// scheduler
// ---------
// round-robin timer-enabled scheduler that runs tasks in descending priority
// with the help of a heap-based priority queue

// supported syscalls
// ---------
// exec
// waitpid

uint32_t sched_init() {
    os_printf("Initializing scheduler\n");
    last_err = "No error";
    all_tasks = prq_create_fixed(MAX_TASKS);
    active_tasks = prq_create_fixed(MAX_ACTIVE_TASKS);
    active_task = 0;
    return 1;
}

static void __sched_register_timer_irq();
static void __sched_deregister_timer_irq();

// contract
// --------
// file_p must be created by the kernel and is located in the kernel_vas
sched_task* sched_create_task(uint32_t* file_p, int niceness) {
    if (prq_count(all_tasks) >= MAX_TASKS) {
        last_err = "Too many tasks";
        return 0;
    }

    __sched_deregister_timer_irq();

    // use the kernel memory
    vm_use_kernel_vas();

    pcb * pcb_pointer = process_create(file_p);

    if (!pcb_pointer) {
        last_err = "Process create error";
        ignore_interrupt = 0;
        return 0;
    }

    struct vas * vas_struct = vm_new_vas();
    sched_task * task = (sched_task*) kmalloc(sizeof(sched_task));

    // FIXME
    // Check this code @lkolbly
    // set up the vas
    // call vm_set_mapping(struct vas *vas, void *vptr, void *pptr, int permission);
    // not sure how to use it

    niceness = MIN(MAX_NICENESS, niceness);
    niceness = MAX(MIN_NICENESS, niceness);

    task->niceness = niceness;
    task->pcb = pcb_pointer;
    task->state = TASK_STATE_NONE;
    task->node = 0;
    task->vas_struct = vas_struct;

    if (active_task != 0) {
        vm_enable_vas(active_task->vas_struct);
    }

    __sched_register_timer_irq();

    return task;
}

// expecting access to kernel global level
void sched_waitpid(uint32_t pid){
    while(true) {
        sched_task * task = (sched_task*) hmap_get(&all_tasks_map, pid);
        if(task == 0 || task->state == TASK_STATE_FINSIHED) { break; }
        sleep(500);
    }
}

void sched_interrupt_handler() {
    if (ignore_interrupt) {
        return;
    }

    __sched_deregister_timer_irq(); // prevent interrupts while handling another interrupt

    // use the kernel memory
    vm_use_kernel_vas();

    if (prq_count(active_tasks) < MAX_ACTIVE_TASKS) {
        if (prq_count(all_tasks) > 0) {
            prq_node * node = prq_dequeue(all_tasks);
            prq_enqueue(active_tasks, node); // add to active_tasks if the task
            kfree(active_task->node);
        }
    }

    if (prq_count(active_tasks) == 0) {
        __sched_register_timer_irq();
        return;
    }

    sched_task * task;

    // check if there is active task
    if (active_task != 0) {
        task = active_task;
    } else {
        prq_node * node = prq_peek(active_tasks);
        task = (sched_task*) node->data;
    }

    switch (task->state) {
        case TASK_STATE_INACTIVE: 				      // start running task->pcb
            active_task = task;                   // set the active_task pointer
            active_task->state = TASK_STATE_ACTIVE; // mark task process as active
            vm_enable_vas(active_task->vas_struct);
            __sched_register_timer_irq();             // allow interrupts
            execute_process_no_vas(active_task->pcb); // start running the process function
            sched_remove_task(active_task);
            // NOTE next interrupt will get the start the process

            break;
            // switch tasks; active task will go to sleep and let another task proceed
        case TASK_STATE_ACTIVE:
            if (prq_count(active_tasks) > 1) { // check if there at least one another task in the queue
                prq_remove(active_tasks, active_task->node); // remove the running task from queue
                prq_enqueue(active_tasks, active_task->node); // add it again to see if its position in the queue
                sched_task * next = (sched_task*) prq_peek(active_tasks)->data;

                if (active_task == next) {
                    vm_enable_vas(active_task->vas_struct);
                    break;
                }

                active_task = next;
                save_process_state(task->pcb->PID); // save the current state of the process
                load_process_state(active_task->pcb->PID); // continue with the next process
                vm_enable_vas(active_task->vas_struct);
            }

            break;
    }

    __sched_register_timer_irq();
}

uint32_t sched_add_task(sched_task * task) {
    if (task) {
        if (task->state == TASK_STATE_NONE) {
            last_err = "Reusing task object not allowed";
            return FAIL;
        }

        // use the kernel memory
        vm_use_kernel_vas();

        prq_node * new_node = (prq_node*) kmalloc(sizeof(prq_node));
        new_node->data = task;
        new_node->priority = task->niceness;

        task->state = TASK_STATE_INACTIVE;
        task->node = new_node;
        prq_enqueue(all_tasks, new_node);

        if (active_task != 0) {
            vm_enable_vas(active_task->vas_struct);
        }

        return OK;
    }

    last_err = "Invalid sched_task pointer";
    return FAIL;
}

uint32_t sched_remove_task(sched_task * task) {
    if(task != 0) { 
        switch(task->state){
            case TASK_STATE_INACTIVE:
                __sched_deregister_timer_irq(); 
                prq_remove(&all_tasks, task->node);
                __sched_register_timer_irq(); 
                break;
            case TASK_STATE_ACTIVE:
                __sched_deregister_timer_irq(); 
                vm_use_kernel_vas();
                active_task->state = TASK_STATE_FINSIHED;       // process completed
                free_PCB(active_task->pcb);               // free process memory
                prq_remove(active_tasks, active_task->node); // remove it from the queue
                kfree(active_task->node);
                active_task = 0;
                __sched_register_timer_irq(); 
                break;
            case TASK_STATE_FINISHED:
                break;
        }

        return task->state;
    }

    return FAIL;
}

uint32_t sched_remove_task(uint32_t * pid){
    sched_task * task = (sched_task*) hmap_get(&all_tasks_map, pid);
    if(task) {
        return sched_remove_task(task);
    }

    return FAIL;
}

uint32_t sched_get_active_pid() {
    if (active_task) {
        return active_task->pcb->PID;
    }

    return 0;
}

const char * sched_last_err() {
    return last_err;
}