/*
 * kthreads.c
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#include "kthread.h"
#include <global_defs.h>
#include <vm.h> // for getting current VAS
#include "klibc.h"

static uint32_t GLOBAL_TID;

int init_all_threads(){
	GLOBAL_TID = 0;
	return 0;
}

uint32_t kthread_start(kthread_handle * kthread)
{
	os_printf("entered kthread_start");
	sched_task * task = sched_create_task_from_kthread(kthread, 10);   // using default 10 niceness
	sched_add_task(task);
	return 0;
}

/**
 *
 * This will create and schedule a kernel thread.
 * The thread will be calling the function "func" with argument "arg".
 *
 * - CAB
 *
 **/
int kthread_create(kthread_handle *handle, uint32_t (*func)(), void *arg)
{
	// get into kernel VAS and save state
	os_printf("in kthread create");
	struct vas *curr_vas = vm_get_current_vas();

	vm_use_kernel_vas();
	
	// create handle and pass function
	kthread_handle * kthread = kmalloc(sizeof(kthread_handle));
	kthread->TID = ++GLOBAL_TID;

	kthread->func = func;
	kthread->arg = arg;

	// save PC and SP
	//kthread->R13 = STACK_TOP -(kthread->TID * BLOCK_SIZE) - 24 ;  //SP ///check this
	kthread->R15 = (uint32_t)func;	//PC


	// leave kernel vas
	vm_enable_vas(curr_vas);
	
	kthread_start(kthread);

	return 0;
}




//kthread_handle* kthread_create(kthread_callback_handler cb_handler)
//{
//	kthread_handle * kthread = kmalloc(sizeof(kthread_handle));
//	kthread->cb_handler = cb_handler;
//	kthread_start(kthread);
//	return kthread;
//}

void kthread_save_state( kthread_handle * handle_pointer )
{
	// CAB might need something similar?? 
	//assert(pcb_p && get_address_of_PCB(pcb_p->PID) > 0 && "Invalid PID in load_process_state");

	os_printf("in save state -- ");

	asm("MOV %0, r0":"=r"(handle_pointer->R0)::);
	asm("MOV %0, r1":"=r"(handle_pointer->R1)::);
	asm("MOV %0, r2":"=r"(handle_pointer->R2)::);
	asm("MOV %0, r3":"=r"(handle_pointer->R3)::);
	asm("MOV %0, r4":"=r"(handle_pointer->R4)::);
	asm("MOV %0, r5":"=r"(handle_pointer->R5)::);
	asm("MOV %0, r6":"=r"(handle_pointer->R6)::);
	asm("MOV %0, r7":"=r"(handle_pointer->R7)::);
	asm("MOV %0, r8":"=r"(handle_pointer->R8)::);
	asm("MOV %0, r9":"=r"(handle_pointer->R9)::);
	asm("MOV %0, r10":"=r"(handle_pointer->R10)::);
	asm("MOV %0, r11":"=r"(handle_pointer->R11)::);
	asm("MOV %0, r12":"=r"(handle_pointer->R12)::);
	asm("MOV %0, r13":"=r"(handle_pointer->R13)::);
	asm("MOV %0, r14":"=r"(handle_pointer->R14)::);
	asm("MOV %0, r15":"=r"(handle_pointer->R15)::);

	os_printf("leaving saved state");
}

void kthread_load_state(kthread_handle * handle_pointer)
{
	os_printf("in save state -- ");

	//vm_enable_vas(handle_pointer->stored_vas);

	asm("MOV r0, %0"::"r"(handle_pointer->R0):);
	asm("MOV r1, %0"::"r"(handle_pointer->R1):);
	asm("MOV r2, %0"::"r"(handle_pointer->R2):);
	asm("MOV r3, %0"::"r"(handle_pointer->R3):);
	asm("MOV r4, %0"::"r"(handle_pointer->R4):);
	asm("MOV r5, %0"::"r"(handle_pointer->R5):);
	asm("MOV r6, %0"::"r"(handle_pointer->R6):);
	asm("MOV r7, %0"::"r"(handle_pointer->R7):);
	asm("MOV r8, %0"::"r"(handle_pointer->R8):);
	asm("MOV r9, %0"::"r"(handle_pointer->R9):);
	asm("MOV r10, %0"::"r"(handle_pointer->R10):);
	asm("MOV r12, %0"::"r"(handle_pointer->R12):);
	asm("MOV r13, %0"::"r"(handle_pointer->R13):);
	asm("MOV r14, %0"::"r"(handle_pointer->R14):);
	asm("MOV r15, %0"::"r"(handle_pointer->R15):);

	os_printf("leaving loaded state");
}

void execute_kthread(kthread_handle *handle, pcb * pcb_p)
{
	os_printf("in kthread execute");
	asm("MOV %0, r15":"=r"(handle->R14)::);
	//hard coding for only one thread -- fix later if there is time
	vm_enable_vas(pcb_p->stored_vas + BLOCK_SIZE);
	handle->current_state = THREAD_RUNNING;
	kthread_load_state(handle);
}

// void init_kthread_stack(kthread_handle *handle)
// {
// 	int retval = 0;

// 	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
// 	{
// 		retval = vm_allocate_page(pcb_p->stored_vas,
// 				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
// 		if (retval)
// 		{
// 			os_printf("vm_allocate_page error code: %d\n", retval);
// 			break;
// 		}
// 		else
// 		{
// 			os_printf(
// 					"A page have been allocated for process stack at vptr: 0x%x\n",
// 					(STACK_BASE + (i * BLOCK_SIZE)));
// 		}

// 			vm_map_shared_memory(KERNEL_VAS,
// 				(void*) (STACK_BASE + (i * BLOCK_SIZE)), pcb_p->stored_vas,
// 				(void*) (STACK_BASE + (i * BLOCK_SIZE)), VM_PERM_USER_RW);
		
// 	}

// 	// Stick a NULL at STACK_TOP-sizeof(int*)
// 	uint32_t *stack_top = (uint32_t*) STACK_TOP;
// 	stack_top[-1] = 0;
// 	stack_top[-2] = 0;
// 	stack_top[-3] = 0;
// 	stack_top[-4] = 0;
// 	stack_top[-5] = STACK_BASE;
// 	stack_top[-6] = 1;

// 	os_strcpy((char*) STACK_BASE, pcb_p->name);

// 	// We need to set sp (r13) to stack_top - 12
// 	pcb_p->R13 = STACK_TOP - 4 * 6;
// 	print_process_state(pcb_p->PID);

// 	for (int i = 0; i < (STACK_SIZE / BLOCK_SIZE); i++)
// 	{
// 		vm_free_mapping(KERNEL_VAS, (void*) (STACK_BASE + (i * BLOCK_SIZE)));

// 	}
// }
