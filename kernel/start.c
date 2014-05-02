/*
 *  A bit of background:
 *  - The ARM architecture has 7 modes of operation:
 *      + USR - user mode
 *      + FIQ - processing "fast" interrupts
 *      + IRQ - processing "normal" interrupts
 *      + SVC - proctected mode for OS
 *      + UND - processing an undefined instruction exception
 *      + SYS - also protecteed mode for OS --if anyone wants to clarify, feel free--
 *
 *  - These modes can be entered or exited by modifying the CPSR (status register), first 5 bits
 *	+ 0b10000 = user mode
 *	+ 0b10001 = FIQ (fast interrupt) mode
 *	+ 0b10010 = IRQ (normal interrupt) mode
 *	+ 0b10011 = SVC (supervisor, or, OS) mode
 *	(others...)
 */

#include <stdint.h>
#include "include/hw_handlers.h"
#include "include/global_defs.h"
#include "include/argparse.h"
#include "include/interrupt.h"
#include "include/mmap.h"
#include "include/process.h"
#include "include/memory.h"

#define UART0_IMSC (*((volatile uint32_t *)(UART0_ADDRESS + 0x038)))
void uart_handler(void *null) {
	print_uart0("uart0!\n");
}

void start() {
  print_uart0("\nCourseOS!\n");

  init_vector_table();

  //don't allow interrpts messing with memory
  disable_interrupts(); 
 
 
 //asm volatile("wfi");
 
  enable_interrupts();


  //Test: UART0 mapped to the correct virtual address   
  print_uart0("MMU enabled\n");
  init_kheap(31 * 0x100000);
  
  //initialize pcb table and PID
  init_all_processes(); 

  uint32_t* hello = pa2va(0x810000);  
  pcb* p = process_create(hello); 
  asm volatile("SWI 7");

  //execute_process(p);

  //main();
  asm volatile("wfi");

}
