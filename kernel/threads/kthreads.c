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
	os_printf("about to finish kthread_start\n");
	return -17;
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
	int retval = 0;

	os_printf("about to start kthread_create\n");

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
	kthread->R13 = STACK_TOP -(kthread->TID * BLOCK_SIZE) - 24 ;  //SP
	kthread->R15 = (uint32_t)func;	//PC


	// leave kernel vas
	vm_enable_vas(curr_vas);

	os_printf("about to call kthread_start\n");

	retval = kthread_start(kthread);

	os_printf("done calling kthread_start ~ ", retval);
   		switch (retval) 
   		{
   		case -20: printf("retval=-20\n"); break; case -19: printf("retval=-19\n"); break;
   		case -18: printf("retval=-18\n"); break; case -17: printf("retval=-17\n"); break;
   		case -16: printf("retval=-16\n"); break; case -15: printf("retval=-15\n"); break;
   		case -14: printf("retval=-14\n"); break; case -13: printf("retval=-13\n"); break;
   		case -12: printf("retval=-12\n"); break; case -11: printf("retval=-11\n"); break;
   		case -10: printf("retval=-10\n"); break; case -9: printf("ret=-9\n");   break;
   		case -8: printf("retval=-8\n");   break; case -7: printf("ret=-7\n");   break;
   		case -6: printf("retval=-6\n");   break; case -5: printf("ret=-5\n");   break;
   		case -4: printf("retval=-4\n");   break; case -3: printf("ret=-3\n");   break;
   		case -2: printf("retval=-2\n");   break; 
   		case -1: printf("retval=-1\n");   break; case 0: printf("retval=0\n");     break;
   		case 20: printf("retval=20\n");   break; case 19: printf("retval=19\n");   break;
   		case 18: printf("retval=18\n");   break; case 17: printf("retval=17\n");   break;
   		case 16: printf("retval=16\n");   break; case 15: printf("retval=15\n");   break;
   		case 14: printf("retval=14\n");   break; case 13: printf("retval=13\n");   break;
   		case 12: printf("retval=12\n");   break; case 11: printf("retval=11\n");   break;
   		case 10: printf("retval=10\n");   break; case 9: printf("retval=9\n");     break;
   		case 8: printf("retval=8\n");     break; case 7: printf("retval=7\n");     break;
   		case 6: printf("retval=6\n");     break; case 5: printf("retval=5\n");     break;
   		case 4: printf("retval=4\n");     break; case 3: printf("retval=3\n");     break;
   		case 2: printf("retval=2\n");     break; case 1: printf("retval=1\n");     break;
   		}

   	handle = (kthread_handle *)retval;

	/**
	 *
	 * Note: this point of the function will not be reached.
	 *          Uncomment the line below to verify.
	 *
	 * - CPH
	 *
	 **/
	// os_printf("in about to return kthread_create\n");

	return retval;

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

void execute_kthread(kthread_handle *handle)
{
	os_printf("in kthread execute");
	asm("MOV %0, r15":"=r"(handle->R14)::);
	//vm_enable_vas(handle->stored_vas);
	handle->current_state = THREAD_RUNNING;
	kthread_load_state(handle);
}
