// SPDX-FileCopyrightText: CGL-KFS
// SPDX-License-Identifier: BSD-3-Clause

/* include/kernel/kvm.h
 *
 * 
 *
 * created: 2022/12/06 - lfalkau <lfalkau@student.42.fr>
 * updated: 2022/12/06 - lfalkau <lfalkau@student.42.fr>
 */

#ifndef KVM_H
#define KVM_H

typedef struct pagedir {
	// TODO
} pagedir_t;

/*
 * Creates and sets the kernel page directory,
 * with mapped regions
 */
void kvm_init();

/*
 * Creates a new page directory
 * Returns a pointer to the newly created pagedir struct
 */
pagedir_t *kvm_new_pagedirectory();

/*
 * Sets the CR3 register to set the given page directory as
 * the used one
 */
void kvm_set_pagedirectory(pagedir_t *pagedir);


/*
 * Maps @n pages from @phy to @virt virtual addresses.
 */
void kvm_map(void *phy, void *virt, uint32_t n);

/*
 * Unmaps @n pages from @virt address
 */
void kvm_unmap(void *virt, uint32_t n);

/*
 * Allocates @size bytes through kpm_alloc and
 * returns the virtual address of the newly allocated
 * area
 */
void *kvm_alloc(uint32_t size);

/*
 * Releases memory
 * Note that @addr must be page aligned, and should have
 * been obtained through a previous kvm_alloc call
 * @size determines the size of the memory area to release
 */
void kvm_free(void *addr, uint32_t size);

/*
 * Converts a virtual address to a physical one
 */
static void *get_physical(void *virt);



#endif
