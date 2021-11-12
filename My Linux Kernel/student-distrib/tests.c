#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "file_system.h"
#include "file_system_driver.h"
#include "terminal.h"
#include "keyboard.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
	TEST_HEADER;

	int i;
	int result = PASS;

	//return result;

	for (i = 0; i < 32; i++)
	{
		if (idt[i].reserved3 != 1)
		{
			assertion_failure();
			result = FAIL;
		}
	}
	for (i = 32; i < 256; i++)
	{
		if (idt[i].reserved3 != 0)
		{
			assertion_failure();
			result = FAIL;
		}
	}

	for (i = 0; i < 10; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

int idt_divide_by_zero_test()
{
	int a, b;
	a = 1;
	b = 1;
	return a / (b - 1);
}

// /* Checkpoint 2 tests */
// void test_rtc()
// {
// 	int i, j;
// 	//init_rtc();
// 	clear();

// 	rtc_open(); //tests rtc_open, set to 2Hz
// 	printf(" open() working");
// 	printf("\n rtc_open (2Hz):");
// 	for (i = 0; i < 1; i++)
// 	{
// 		rtc_read();
// 	}

// 	for (j = 4; j <= 1024; j = j * 2)
// 	{
// 		printf("\n %dHz:  ", j);
// 		rtc_write(j); //test rtc_write at j Hz
// 		for (i = 0; i < (j / 2); i++)
// 		{

// 			rtc_read();
// 			printf(".");
// 			if (i == 69)
// 			{
// 				printf("\n");
// 			}
// 			if (!((i - 69) % 80) && ((i - 69) != 0))
// 			{
// 				printf("\n");
// 			}
// 		}
// 	}
// 	printf("\n read() works");
// 	printf("\n write() working for 2-1024Hz");
// 	rtc_close();
// 	printf("\n close() also working");
// }

void test_scroll()
{
	int i;
	printf("scroll \n");
	scroll();
	printf("and again \n");
	scroll();
	printf("and the rest");
	for (i = 0; i < 17; i++)
	{
		scroll();
	}
}

// void test_terminal_funcs()
// {
// 	int i;
// 	char bufff[78];
// 	printf("terimnal write: \n");
// 	for (i = 0; i < 78; i++)
// 	{
// 		bufff[i] = 'h';
// 	}
// 	bufff[77] = '\0';
// 	terminal_write(bufff, 1);

// 	printf("\n terimnal read+ write: \n");
// 	terminal_read(bufff);
// 	terminal_write(bufff, 1);
// 	printf("\n");
// 	update_cursor();
// }

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */

/* Test suite entry point */
void launch_tests()
{
	// TEST_OUTPUT("idt_test", idt_test());
	// printf("running divde test");
	// TEST_OUTPUT("divide_by_zero", idt_divide_by_zero_test());
	// launch your tests here
	// test_rtc();

	/* Checkpoint 3 tests */
	// test_scroll();
	// test_file_driver();
	// print_filenames();
	// test_file_read();
	// test_terminal_funcs();
}
