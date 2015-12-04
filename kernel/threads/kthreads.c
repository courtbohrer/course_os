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

uint32_t kthread_start(kthread_handle * kthread)
{
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
int kthread_create(uint32_t (*func)(), void *arg)
{
	// get into kernel VAS and save state
	vm_use_kernel_vas();
	struct vas *curr_vas = vm_get_current_vas();

	// create handle and pass function
	kthread_handle * kthread = kmalloc(sizeof(kthread_handle));
	 kthread->func = func;
	// kthread->R15 = (uint32_t)func;
	 kthread->arg = arg;

	// leave kernel vas
	vm_enable_vas(curr_vas);

	kthread_start(kthread);

	// ** fix this **
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
}

void kthread_load_state(kthread_handle * handle_pointer)
{
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
	//asm("MOV r11, %0"::"r"(11):);
	asm("MOV r12, %0"::"r"(handle_pointer->R12):);

	asm("MOV r13, %0"::"r"(handle_pointer->R13):);

	asm("MOV r14, %0"::"r"(handle_pointer->R14):);
//assert(1==11);

	asm("MOV r15, %0"::"r"(handle_pointer->R15):);

	// don't know why you would need this?
	//__builtin_unreachable();
}