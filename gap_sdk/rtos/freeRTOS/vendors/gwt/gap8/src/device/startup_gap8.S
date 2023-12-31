/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

        .file "startup_gap8.S"

/*******************************************************************************
        This file contains the Bootcode and initializes the Interrupt Vector
        Table.
*******************************************************************************/

/*******************************************************************************
        EXTERNAL VARIABLES & FUNCTIONS
*******************************************************************************/
        .extern __irq_vector_base_m__
        .extern __data_start__
        .extern __bss_start__
        .extern __bss_end__
        .extern __stack_fc_start__
        .extern __stack_irq_start__
        .extern __stack_irq_end__

        .extern cl_cluster_exec_loop
        .extern system_init
        .extern main
        .extern exit
        .extern ill_ins_handler
        .extern ecall_handler
        .extern pendSV_handler
        .extern systick_handler
        .extern cluster_notify_fc_handler
        .extern fc_event_handler
        .extern dma_irq_handler
        .extern fc_hyper_handler
/******************************************************************************/


/*******************************************************************************
        MACRO DEFINITION
*******************************************************************************/

/******************************************************************************/
        .macro DECLARE Routine
        .global \Routine
        .func \Routine
        .type \Routine, %function
        \Routine:
        .endm
/******************************************************************************/


/*******************************************************************************
        BOOTCODE
*******************************************************************************/
        .section .text.reset, "ax", @progbits
        DECLARE reset_handler
        csrr a0, 0xf14 /* Cluster ID */
        andi a1, a0, 0x1f       /* Core ID */
        srli a0, a0, 5

#if defined(FEATURE_CLUSTER)
        li a2, 32 /* FC_Cluster ID */
        //bne a2, a0, cl_cluster_exec_loop
        .extern cluster_exec_loop
        bne a2, a0, cluster_exec_loop
#endif /* FEATURE_CLUSTER */

        li a0, 0x1800 /* Set MSTATUS : Machine Mode */
        csrw mstatus, a0

        la a0, __irq_vector_base_m__ /* Set MTVEC */
        csrw mtvec, a0

        la gp, __data_start__ /* Set global pointer(global var) */

        la sp, __stack_fc_start__ /* Set stack pointer in L1(FC) */

_fill_zero: /* Init BSS section */
        la a0, __bss_start__
        la a1, __bss_end__
.LC1_zero_bss:
        sw    x0, (a0)
        addi  a0, a0, 4
        bltu  a0, a1, .LC1_zero_bss

        la    a0, __stack_irq_end__
        la    a1, __stack_irq_start__
.LC2_zero_stack_irq:
        sw    x0, (a0)
        addi  a0, a0, 4
        bltu  a0, a1, .LC2_zero_stack_irq

        jal ra, system_init

_jump2main: /* Jump to main */
        jal ra, main
        /* Exit. */
        jal ra, exit
        .endfunc
/******************************************************************************/


/*******************************************************************************
        INTERRUPT VECTOR TABLE
        -----------------------------
         ADDR   |  Desc
        0x00      Interrupt 0
         -        Interrupt [1-30]
        0x7C      Interrupt 31
        0x80      Reset
        0x84      Illegal Instruction
        0x88      ECALL Instruction Executed
*******************************************************************************/
        .section .vectors_irq, "ax"
        .option norvc;

        j default_handler           /* IRQ=0 : undefined */
        j pendSV_handler            /* IRQ=1 : PendSV Handler. */
        j cluster_notify_fc_handler /* IRQ=2 : Cluster to FC notify Handler. */
        j default_handler           /* IRQ=3 : undefined */
        j default_handler           /* IRQ=4 : undefined */
        j default_handler           /* IRQ=5 : undefined */
        j default_handler           /* IRQ=6 : undefined */
        j default_handler           /* IRQ=7 : undefined */
        j default_handler           /* IRQ=8 : undefined */
        j dma_irq_handler           /* IRQ=9 : DMA IRQ Handler. */
        j systick_handler           /* IRQ=10 : Systick Handler. */
        j hi_res_timer_handler      /* IRQ=11 : hi res timer irq handler */
        j default_handler           /* IRQ=12 : undefined */
        j default_handler           /* IRQ=13 : undefined */
        j default_handler           /* IRQ=14 : undefined */
        j default_handler           /* IRQ=15 : undefined */
        j default_handler           /* IRQ=16 : undefined */
        j default_handler           /* IRQ=17 : undefined */
        j default_handler           /* IRQ=18 : undefined */
        j default_handler           /* IRQ=19 : undefined */
        j default_handler           /* IRQ=20 : undefined */
        j default_handler           /* IRQ=21 : undefined */
        j default_handler           /* IRQ=22 : undefined */
        j default_handler           /* IRQ=23 : undefined */
        j default_handler           /* IRQ=24 : undefined */
        j default_handler           /* IRQ=25 : undefined */
        j default_handler           /* IRQ=26 : undefined */
        j fc_event_handler          /* IRQ=27 : FC SoC event Handler. */
        j default_handler           /* IRQ=28 : undefined */
#if (CHIP_VERSION == 3)
        j fc_hyper_handler           /* IRQ=29 : undefined */
        j fc_hyper_handler           /* IRQ=30 : undefined */
#else
        j default_handler           /* IRQ=29 : undefined */
        j default_handler           /* IRQ=30 : undefined */
#endif
        j default_handler           /* IRQ=31 : undefined */

        j reset_handler             /* After IRQ=31, Reset Handler. */

        j ill_ins_handler           /* After Reset : Illegal Instruction Handler. */

        j ecall_handler             /* After Illegal : Ecall Handler. */


        .org 0x90
        .global __rt_debug_struct_ptr
        __rt_debug_struct_ptr:
        .word 0xdeadbeef
