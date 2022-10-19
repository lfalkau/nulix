/* kernel/isr.c
 *
 * Definition of ISR (Interrupt Service Routines) to handle exceptions
 * and interrupts
 *
 * created: 2022/10/18 - xlmod <glafond-@student.42.fr>
 * updated: 2022/10/18 - xlmod <glafond-@student.42.fr>
 */

#include <kernel/print.h>

#include "idt_internal.h"

/* Debug function to print all interrupt frame.
 *
 * @arg(int_frame): interrupt frame structure.
 */
static void print_int_frame(t_int_frame *int_frame, uint32_t *errcode) {
	kprintf("frame interupt: %p\n", int_frame);
	kprintf("     eip: %x\n", int_frame->eip);
	kprintf("      cs: %x\n", int_frame->cs);
	kprintf("     flg: %x\n", int_frame->flags);
	kprintf("      sp: %x\n", int_frame->sp);
	kprintf("      ss: %x\n", int_frame->ss);
	if (errcode)
	kprintf("     err: %x\n", *errcode);
}

/* DIVIDE ERROR (0)
 * Occure when division by 0 happen
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void divide_error_handler(t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;

	kprintf("DIVIDE ERROR\n");
	print_int_frame(int_frame, NULL);

	RESET_INTERRUPT_STACK;
}

/* OVERFLOW (4)
 *
 * @arg(int_frame): interrupt frame structure.
 */
__attribute__ ((interrupt)) void overflow_handler(t_int_frame *int_frame)
{
	LOAD_INTERRUPT_STACK;

	kprintf("OVERFLOW\n");
	print_int_frame(int_frame, NULL);

	RESET_INTERRUPT_STACK;
}


/* DOUBLE FAULT(0)
 * Occure when a fault happen in a interrupt/exception or when no handle functions
 * is in the idt.
 *
 * @arg(int_frame): interrupt frame structure.
 * @arg(error_code): code d'erreur.
 */
__attribute__ ((interrupt)) void double_fault_handler(t_int_frame *int_frame, uint32_t error_code)
{
	LOAD_INTERRUPT_STACK;

	kprintf("DOUBLE FAULT\n");
	print_int_frame(int_frame, &error_code);
	asm volatile ("hlt");

	RESET_INTERRUPT_STACK;
}