// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* kernel/boot_init.c
 *
 * Initialize temporary boot pages and load them in cr3
 *
 * created: 2022/12/12 - xlmod <glafond-@student.42.fr>
 * updated: 2023/01/17 - glafond- <glafond-@student.42.fr>
 */

#include <stdint.h>

#include <kernel/paging.h>
#include <kernel/kernel.h>
#include <kernel/string.h>
#include <kernel/memory.h>
#include <kernel/symbole.h>

/*
 * Create a page directory and a page table and map the first 4M at virtual
 * address 0x0 and 0xc0000000.
 */
void boot_init() {
	struct page_entry *page_directory = (struct page_entry *)(0x1000);
	struct page_entry *page_table1 = (struct page_entry *)(0x2000);
	struct page_entry *page_table2 = (struct page_entry *)(0x3000);

	(memset - KERNEL_VIRT_OFFSET)(page_directory, 0, PAGE_DIR_LENGTH * 4);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory + LAST_PAGE_ENTRY, page_directory, 1, 0);

	(memset - KERNEL_VIRT_OFFSET)(page_table1, 0, PAGE_TABLE_LENGTH * 4);
	for (uint32_t addr = PAGE_SIZE; addr < 4 * MB; addr += PAGE_SIZE)
		(page_init - KERNEL_VIRT_OFFSET)(page_table1 + (addr / PAGE_SIZE), (void *)addr, 1, 0);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory, page_table1, 1 ,0);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory + (KERNEL_VIRT_OFFSET >> 22), page_table1, 1, 0);

	(memset - KERNEL_VIRT_OFFSET)(page_table2, 0, PAGE_TABLE_LENGTH * 4);
	for (uint32_t addr = SYM_STACK, virt = SYM_VIRT_STACK; addr < SYM_STACK + (STACK_TOP - STACK_BOTTOM); addr += PAGE_SIZE, virt += PAGE_SIZE)
		(page_init - KERNEL_VIRT_OFFSET)(page_table2 + ((virt / PAGE_SIZE) & 0x3ff), (void *)addr, 1, 0);
	(page_init - KERNEL_VIRT_OFFSET)(page_directory + (LAST_PAGE_ENTRY - 1), page_table2, 1, 0);

	// Load the page directory to cr3 to tell the cpu to using this page directory
	// to resolve virtual address.
	__asm__ volatile ("movl %0, %%cr3" :: "r" ((uint32_t)page_directory));
}
