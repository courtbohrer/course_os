#include <stdio.h>
#include "../libc/arch/arm/syscall_arch.h"
#include <fs_syscalls.h>
#include <thread.h>

void test_thread_lib();
void *test_function_for_thread(void *args);

int main() {
	__syscall3(99, 0, 0, 0);

	printf("Hello world... from hello.c\n");

	printf("LET'S TEST %d\n", 10);

	int* mem = 0;
	mem = (int*) malloc(100);

	printf("malloc returned %x\n", mem);

	mem[0] = 1;
	mem[10] = 2;

    free(mem);

    test_thread_lib();

    printf("success\n");

    while(1);
}

//void infloop() { while (1); }

/**
 *
 * This function is to test the functionality of our thread library.
 * 
 * - CPH
 *
 **/
void test_thread_lib()
{
	printf("Now testing the thread library... :)\n");

	thread_t *thread;

	int ret_val = 0;

	ret_val = thread_create(thread, &test_function_for_thread, 10);

	thread_self();

	ret_val = thread_get_id(thread);

	ret_val = thread_join(thread, NULL);

	// thread_exit(NULL);

	// SHOULD NEVER BE REACHED?
	printf("...done testing the thread library.\n");
}

/**
 *
 * This function just exists for test thread(s) to bind to. It doesn't 
 *    matter what it does.
 * 
 * - CPH
 *
 **/
void *test_function_for_thread(void *args)
{
	int return_me = 1;
	for (int i = 1; i < (int)args; i++)
		return_me *= i;
	return (void *)return_me;
}
