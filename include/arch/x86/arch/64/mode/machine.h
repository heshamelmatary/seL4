/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __ARCH_MACHINE_64_H
#define __ARCH_MACHINE_64_H

#include <config.h>
#include <arch/model/statedata.h>
#include <arch/machine/cpu_registers.h>

#define wordRadix 6
#define wordBits (1 << wordRadix)

/* Address space control */
static inline cr3_t getCurrentCR3(void)
{
    return x64KSCurrentCR3;
}

static inline paddr_t getCurrentVSpaceRoot(void)
{
    return cr3_get_pml4_base_address(getCurrentCR3());
}

static inline void setCurrentCR3(cr3_t cr3, word_t preserve_translation)
{
    x64KSCurrentCR3 = cr3;
    if (config_set(CONFIG_SUPPORT_PCID)) {
        write_cr3(cr3.words[0]  | (preserve_translation ? BIT(63) : 0));
    } else {
        write_cr3(cr3_new(getCurrentVSpaceRoot(), 0).words[0]);
    }
}

static inline void setCurrentVSpaceRoot(paddr_t addr, word_t pcid)
{
    setCurrentCR3(cr3_new(addr, pcid), 1);
}

/* GDT installation */
void x64_install_gdt(gdt_idt_ptr_t* gdt_idt_ptr);

/* IDT installation */
void x64_install_idt(gdt_idt_ptr_t* gdt_idt_ptr);

/* LDT installation */
void x64_install_ldt(uint32_t ldt_sel);

/* TSS installation */
void x64_install_tss(uint32_t tss_sel);

void handle_fastsyscall(void);

void init_syscall_msrs(void);

/* Get current stack pointer */
static inline void* get_current_esp(void)
{
    word_t stack;
    void *result;
    asm volatile("movq %[stack_address], %[result]" : [result] "=r"(result) : [stack_address] "r"(&stack));
    return result;
}

typedef struct invpcid_desc {
    uint64_t    asid;
    uint64_t    addr;
} invpcid_desc_t;

#define INVPCID_TYPE_ADDR           0
#define INVPCID_TYPE_SINGLE         1
#define INVPCID_TYPE_ALL_GLOBAL     2   /* also invalidate global */
#define INVPCID_TYPE_ALL            3

static inline void invalidatePCID(word_t type, void *vaddr, asid_t asid)
{
    if (config_set(CONFIG_SUPPORT_INVPCID)) {
        invpcid_desc_t desc;
        desc.asid = asid & 0xfff;
        desc.addr = (uint64_t)vaddr;
        asm volatile ("invpcid %1, %0" :: "r"(type), "m"(desc));
    } else {
        switch(type) {
        case INVPCID_TYPE_ADDR:
            asm volatile("invlpg (%[vptr])" :: [vptr] "r"(vaddr));
            break;
        case INVPCID_TYPE_SINGLE:
        case INVPCID_TYPE_ALL:
            /* reload CR3 to perform a full flush */
            setCurrentCR3(getCurrentCR3(), 0);
            break;
        case INVPCID_TYPE_ALL_GLOBAL:
            {
                /* clear and reset the global bit to flush global mappings */
                unsigned long cr4 = read_cr4();
                write_cr4(cr4 & ~BIT(7));
                write_cr4(cr4);
            }
            break;
        }
    }
}

static inline void invalidateTranslationSingle(vptr_t vptr)
{
    /* As this may be used to invalidate global mappings by the kernel,
     * and as its only used in boot code, we can just invalidate
     * absolutely everything form the tlb */
    invalidatePCID(INVPCID_TYPE_ALL_GLOBAL, (void*)0, 0);
}

static inline void invalidateTranslationSingleASID(vptr_t vptr, asid_t asid)
{
    invalidatePCID(INVPCID_TYPE_ADDR, (void*)vptr, asid);
}

static inline void invalidateTranslationAll(void)
{
    invalidatePCID(INVPCID_TYPE_ALL_GLOBAL, (void*)0, 0);
}

static inline void invalidatePageStructureCacheASID(paddr_t root, asid_t asid)
{
    if (config_set(CONFIG_SUPPORT_INVPCID)) {
        /* store our previous cr3 */
        cr3_t cr3 = getCurrentCR3();
        /* load new vspace root, invalidating translation for it */
        setCurrentCR3(cr3_new(root, asid), 0);
        /* reload old cr3, preserving its translation */
        setCurrentCR3(cr3, 1);
    } else {
        /* just invalidate the page structure cache as per normal, by
         * doing a dummy invalidation of a tlb entry */
        asm volatile("invlpg (%[vptr])" :: [vptr] "r"(0));
    }
}

#ifdef CONFIG_FSGSBASE_INST

static inline void x86_write_fs_base(word_t base)
{
    asm volatile ("wrfsbase %0"::"r"(base):);
}

static inline void x86_write_gs_base(word_t base)
{
    asm volatile ("wrgsbase %0"::"r"(base):);
}

static inline word_t x86_read_fs_base(void)
{
    word_t base = 0;
    asm volatile ("rdfsbase %0":"=r"(base)::);
    return base;
}

static inline word_t x86_read_gs_base(void)
{
    word_t base = 0;
    asm volatile ("rdgsbase %0":"=r"(base)::);
    return base;
}

#endif
#endif
