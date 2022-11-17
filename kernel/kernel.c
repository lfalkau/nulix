// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel.c
 *
 * Entrypoint of the KFS kernel
 *
 * created: 2022/10/11 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/11/04 - lfalkau <lfalkau@student.42.fr>
 */

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyboard.h>
#include <kernel/pic_8259.h>
#include <kernel/port.h>
#include <kernel/ps2.h>
#include <kernel/print.h>
#include <kernel/string.h>
#include <kernel/vga.h>

extern struct vga vga;

/* Initialize all descriptor tables (gdt, idt, ...)
 *
 */
static void init_descriptor_tables() {
	pic_8259_init(PIC1_OFFSET, PIC2_OFFSET);
	gdt_init();
	idt_init();
}

static void print_help() {
	char *shortcuts[] = {
		[1] = " Clear the screen",
		[2] = " Display help",
		[3] = " Set the foreground color to red",
		[4] = " Generates an interrupt",
		[5] = " Reboot",
		[6] = " Go to erase mode",
		[7] = " Go to normal mode",
		[8] = " Switch to qwerty",
		[9] = " Switch to azerty",
		[10] = "Print the stack",
		[11] = "Print the GDT",
		[12] = "Reset terminal",
	};

	for (int i = 1; i <= 12; i++) {
		VGA_setforegroundcolor(VGA_COLOR_GREEN);
		kprintf("F%d: ", i);
		VGA_setforegroundcolor(VGA_COLOR_LIGHT_GREY);
		kprintf("%s\n", shortcuts[i]);
	}
}

/* Reboot kernel through the PS/2 controller by sending
 * a reset signal to the CPU as soon as the ps/2 data
 * buffer is empty.
 */
static void reboot()
{
	while (port_read(PS2_STATUS_REGISTER_PORT) & PS2_INPUT_BUFFER_FULL);
	port_write(PS2_STATUS_COMMAND_PORT, CPU_RESET);
	asm volatile ("hlt");
}

void kernel_main(void) {
	char c = 0;
	struct kbd_event evt;
	init_descriptor_tables();

	VGA_initialize();
	KBD_initialize();

	print_help();
	while (1) {
		while (!KBD_poll());
		if (KBD_getevent(&evt) == 0 && evt.type == KEY_PRESSED) {
			if ((c = KBD_getchar(&evt))) {
				VGA_putchar(c);
			} else {
				switch (evt.key) {
					case KEY_F1:
						VGA_clear();
						break;
					case KEY_F2:
						print_help();
						break;
					case KEY_F3:
						VGA_setforegroundcolor(VGA_COLOR_RED);
						break;
					case KEY_F4:
						asm volatile ("int $0x0");
						break;
					case KEY_F5:
						reboot();
						break;
					case KEY_F6:
						VGA_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
						break;
					case KEY_F7:
						vga.color = VGA_DFL_COLOR;
						break;
					case KEY_F8:
						KBD_setkeymap(US104_getkeymapentry);
						break;
					case KEY_F9:
						KBD_setkeymap(FR_getkeymapentry);
						break;
					case KEY_F10:
						print_stack();
						break;
					case KEY_F11:
						print_gdt();
						break;
					case KEY_F12:
						vga.color = VGA_DFL_COLOR;
						VGA_clear();
						print_help();
						break;
					case KEY_CURSOR_LEFT:
						VGA_move_cursor_to(vga.x - 1, vga.y);
						break;
					case KEY_CURSOR_RIGHT:
						VGA_move_cursor_to(vga.x + 1, vga.y);
						break;
					case KEY_CURSOR_UP:
						VGA_move_cursor_to(vga.x, vga.y - 1);
						break;
					case KEY_CURSOR_DOWN:
						VGA_move_cursor_to(vga.x, vga.y + 1);
						break;
				}
			}
		}
		KBD_flush();
	}
}
