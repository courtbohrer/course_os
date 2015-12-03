#include <process.h>
#include "scheduler.h"
/*
 * kthread.h
 *
 *  Created on: Apr 23, 2015
 *      Author: mwkurian
 */

#ifndef KERNEL_INCLUDE_KTHREAD_H_
#define KERNEL_INCLUDE_KTHREAD_H_

typedef enum THREAD_STATE
{
	THREAD_NEW, THREAD_READY, THREAD_RUNNING, THREAD_BLOCKED
} THREAD_STATE;

typedef struct kthread_handle {
    uint32_t parent_pid;
    int niceness;
    THREAD_STATE current_state;
    void (*func)(void *arg);
    void *arg;

	//unbanked register
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;

	//banked registers
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R12;
	uint32_t R13; //corresponds to the SP
	uint32_t R14;
	uint32_t R15; //corresponds to the PC

} kthread_handle;

int kthread_create(void (*func)(void *a), void *arg);

void kthread_save_state( kthread_handle *handle_pointer );

void kthread_load_state( kthread_handle *handle_pointer );

#endif /* KERNEL_INCLUDE_KTHREAD_H_ */
