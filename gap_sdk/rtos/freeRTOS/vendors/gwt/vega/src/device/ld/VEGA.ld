/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

OUTPUT_ARCH(riscv)
ENTRY(reset_handler)

MEMORY
{
  /* L2_priv 0 : FC private data. */
  L2_priv0          : ORIGIN = 0x1C000004, LENGTH = 0x0FFFC
  L2_priv0_aliased  : ORIGIN = 0x00000004, LENGTH = 0x0FFFC

  /* Considering L2_PRIV0 and L2_PRIV1 as one unique memory. */
  /* L2_priv1 : FC code. */
  /* L2_priv1          : ORIGIN = 0x1c008000, LENGTH = 0x8000 */
  /* L2_priv1_aliased  : ORIGIN = 0x00008000, LENGTH = 0x8000 */

  /* L2 shared memory(FC+Cluster). */
  L2_shared         : ORIGIN = 0x1C010000, LENGTH = 0x180000

  /* L1 cluster memory. */
  L1                : ORIGIN = 0x10000000, LENGTH = 0x00020000
  L1_aliased        : ORIGIN = 0x00000000, LENGTH = 0x00020000
}

/* Here are defined stack sizes for FC and cluster. */
__STACK_FC_SIZE      = 0x1000;
__STACK_IRQ_SIZE     = 0x400;

SECTIONS
{
    /*
     * L2_PRIV0.
     * L2 PRIVATE BANK0.
     * FC Data.
     */
    .data_tiny_fc :
    {
        . = ALIGN(16);
        *(.data_tiny_fc)
        *(.data_tiny_fc.*)
        *(.fcTcdmTiny)
        *(.fcTcdmTiny.*)
        . = ALIGN(16);
    } > L2_priv0_aliased AT> L2_priv0

    .boot :
    {
        . = ALIGN(16);
        *(.boot)
        *(.boot.*)
    } > L2_priv0

    .rodata :
    {
        . = ALIGN(16);
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(16);
        *(.srodata)
        *(.srodata.*)
        *(.eh_frame*)
        *(.eh_frame.*)
    } > L2_priv0

    .shbss :
    {
        . = ALIGN(16);
        *(.shbss)
        *(.shbss.*)
    } > L2_priv0

    /*--------------------------------------------------------------------*/
    /*                GCC Special sections.                               */
    /*--------------------------------------------------------------------*/
    .ctors :
    {
        . = ALIGN(16);
        __CTOR_LIST__ = .;
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           from the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        __CTOR_END__ = .;
    } > L2_priv0

    .dtors :
    {
        . = ALIGN(16);
        __DTOR_LIST__ = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        __DTOR_END__ = .;
    }  > L2_priv0

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > L2_priv0

    .init_array :
    {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
    } > L2_priv0

    .fini_array :
    {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      PROVIDE_HIDDEN (__fini_array_end = .);
    } > L2_priv0

    .data_fc :
    {
        . = ALIGN(4);
        __printf_lock_ptr_l2 = .;
        . = __printf_lock_ptr_l2 + 0x4;
        . = ALIGN(16);
        sdata  =  .;
        _sdata  =  .;
        *(.sdata)
        *(.sdata.*)
        *(.data_fc)
        *(.data_fc.*)
        *(.fcTcdm)
        *(.fcTcdm.*)
        *(.fcTcdm_g)
        *(.fcTcdm_g.*)
        *(.heapfcram)
        *(.heapfcram.*)
        . = ALIGN(16);
        edata  =  .;
        _edata  =  .;
    } > L2_priv0

    .stack_irq :
    {
        . = ALIGN(16);
        __fc_bss_start__ = .;
        __stack_irq_end__ = .;
        . = . + __STACK_IRQ_SIZE;
        __stack_irq_start__ = .;
        __fc_bss_end__ = __stack_irq_start__;
        . = ALIGN(16);
    } > L2_priv0

    /* L2_priv0 heap. */
    __heapfcram_start = .;
    /* Stack used by main, will be included in l2_priv0 heap. */
    __stack_fc_end__ = .;
    . = . + __STACK_FC_SIZE;
    __stack_fc_start__ = .;

    __heapfcram_size = ORIGIN(L2_priv0) + LENGTH(L2_priv0) - __heapfcram_start;

    /* End of L2_PRIV0. */
    /* L2_PRIV1 is considered extension of L2_PRIV0. */
    /* End of L2_PRIV1. */

    /*
     * L2 SHARED BANKS
     *
     * Contains other data such as peripheral data and cluster code and data
     */
    .vectors_irq :
    {
        . = ALIGN(256);
        __irq_vector_base_m__ = .;
        KEEP(*(.vectors_irq))
    } > L2_shared

    .cluster_vectors_irq :
    {
        . = ALIGN(256);
        __irq_cluster_vector_base_m__ = .;
        KEEP(*(.cluster_vectors_irq))
    } > L2_shared

    .text :
    {
        . = ALIGN(16);
        _stext = .;
        *(.text.reset)
        *(.text)
        *(.text.*)
        . = ALIGN(16);
        __cluster_text_start = .;
        *(.text.cluster)
        *(.text.cluster.*)
        __cluster_text_end = .;
        . = ALIGN(16);
        _etext  =  .;
        *(.lit)
        *(.shdata)
        . = ALIGN(16);
        _endtext = .;
    } > L2_shared

    .data :
    {
        . = ALIGN(16);
        __data_start__ = .;
        *(.data)
        *(.data.*)
        *(.l2_data)
        *(.l2_data.*)
        *(.data_fc_shared)
        *(.data_fc_shared.*)
        *(.heapl2ram)
        *(.heapl2ram.*)
        __data_end__ = .;
    } > L2_shared

    .bss :
    {
        . = ALIGN(16);
        __bss_start__ = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end__ = .;
        __l1_preload_start_inL2 = .;
    } > L2_shared


    /*
     * Following sections are keeping the cluster data
     * in L2 until the cluster is powered up.
     */

    .data_tiny_l1 (ORIGIN(L1)) :
    {
        . = ALIGN(16);
        __l1_preload_start = .;
        *(.data_tiny_l1)
        *(.data_tiny_l1.*)
        *(.data_alias_l1)
        *(.data_alias_l1.*)
        *(.l1clusterTiny)
        *(.l1clusterTiny.*)
        . = ALIGN(16);
        __data_tiny_l1_end = .;
    } > L1 AT> L2_shared

    .l1cluster_g (__data_tiny_l1_end):
    {
        . = ALIGN(16);
        __printf_lock_ptr_l1 = .;
        . = __printf_lock_ptr_l1 + 0x4;
        *(.l1cluster_g)
        *(.l1cluster_g.*)
        *(.data_l1)
        *(.data_l1.*)
        *(.heapsram)
        *(.heapsram.*)
        . = ALIGN(16);
    } > L1 AT> L2_shared


    /* L1 heap. */
    __l1_heapsram_start = .;
    __l1_heapsram_size = ORIGIN(L1) + LENGTH(L1) - __l1_heapsram_start;

    __l1_preload_size = SIZEOF(.data_tiny_l1) + SIZEOF(.l1cluster_g);

    /* L2 heap. */
    __heapl2ram_start = LOADADDR(.l1cluster_g) + SIZEOF(.l1cluster_g);
    __heapl2ram_size = ORIGIN(L2_shared) + LENGTH(L2_shared) - __heapl2ram_start;
}
