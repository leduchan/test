/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__
#define __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__

#include <stdint.h>
#include "pmsis/targets/target.h"


/**
 * \brief Clock-gated load.
 *
 * On architectures that support it, this executes the special p.elw instruction
 * which is used to put the core in idle mode until it receives an event.
 * On other architectures, it executes a classic read which has the same effect
 * but does not handle correctly interrupts while the core is sleeping.
 *
 * \param base           Base address of the event unit.
 * \param offset         Offset in the event unit where to load from.
 *                       Depending on this offset, this will trigger different
 *                       behaviors (barrier, wait, wait&clear, etc).
 *
 * \return               The loaded value, after the core has been waken-up.
 *                       This value depends on the feature which is accessed.
 */
//static inline uint32_t hal_eu_read32(uint32_t base, uint32_t offset)
#define hal_eu_read32(base, offset)                                     \
    ({                                                                  \
        uint32_t value = 0;                                             \
        value = __builtin_pulp_event_unit_read_fenced((void *) base, offset); \
        value;                                                          \
    })


/**
 * \brief EU_Core.
 *
 * Function to acces cluster event unit.
 *
 * Set event mask .
 * Clear event mask.
 * Set IRQ mask.
 * Clear IRQ mask.
 */
static inline void hal_cl_eu_event_mask_set(uint32_t mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    GAP_WRITE(base, CL_DEMUX_EU_CORE_EVENT_MASK_OR, mask);
}

static inline void hal_cl_eu_event_mask_clear(uint32_t mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    GAP_WRITE(base, CL_DEMUX_EU_CORE_EVENT_MASK_AND, mask);
}

static inline void hal_cl_eu_irq_mask_set(uint32_t mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    GAP_WRITE(base, CL_DEMUX_EU_CORE_IRQ_MASK_OR, mask);
}

static inline void hal_cl_eu_irq_mask_clear(uint32_t mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    GAP_WRITE(base, CL_DEMUX_EU_CORE_IRQ_MASK_AND, mask);
}

/**
 * \brief Events and IRQs status.
 *
 * This register contains one bit per event, 1 means the event is set.
 * This register is a buffer, which means if an event is received while its bit
 * is already at 1, the event is somehow lost as the bit stays at 1.
 * Thus events should be used as Linux signals.
 *
 * \return               Events and IRQs received.
 *
 * \note This register is actually used as a status register for both events and
 *       interrupts, so clearing one will clear the other.
 */
static inline uint32_t hal_cl_eu_event_irq_status(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER);
}

/**
 * \brief Event status.
 *
 * Same as hal_cl_eu_evt_irq_status(), with the content ANDed with current event
 * mask, so that only relevant events (that can wake-up the core) are shown.
 * It can for example be used to know which events must be handled after a core
 * wake-up as the core has been waken-up due to one of them.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_event_status(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER_MASKED);
}

/**
 * \brief IRQs status.
 *
 * Same as hal_cl_eu_evt_irq_status(), with the content ANDed with current event
 * IRQ mask, so that only relevant events (that can wake-up the core) are shown.
 * It can for example be used to know which events must be handled after a core
 * wake-up as the core has been waken-up due to one of them.
 *
 * \return               IRQs received.
 */
static inline uint32_t hal_cl_eu_irq_status(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER_IRQ_MASKED);
}

/**
 * \brief Clear event status.
 *
 * Clears the corresponding buffered bit in eu_core_buffer to 0.
 * If the corresponding event line is active in the same cycle where the clear
 * operation gets effective, the bit still does get cleared to 0.
 */
static inline void hal_cl_eu_event_clear(uint32_t mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    GAP_WRITE(base, CL_DEMUX_EU_CORE_BUFFER_CLEAR, mask);
}

/**
 * \brief Wait and sleep.
 *
 * Reading this register will stop the clock of the core until at least one
 * event with the corresponding mask bit set to 1 occurs. The read content of
 * this register is identical to that of eu_core_buffer_masked.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_event_wait(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    return hal_eu_read32(base, CL_DEMUX_EU_CORE_EVENT_WAIT);
}

/**
 * \brief Wait and sleep, and clear event.
 *
 * Reading from this register has the same effect as reading from eu_core_event_wait.
 * In addition, the bits of eu_core_buffer that are set to 1 in eu_core_mask
 * will be cleared to 0 after the read.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_event_wait_and_clear(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_core(0);
    return hal_eu_read32(base, CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR);
}

/**
 * \brief Wait and sleep.
 *
 * Modify the event mask, put the core to sleep mode until it receives an event,
 * clears the active events and restore the mask.
 * This is similar to hal_cl_eu_evt_wait() but the events whose bit is set to 1
 * in the given mask, are activated in the event mask before going to sleep, and
 * are cleared when the core is waken-up.
 *
 * \param event_mask     Event mask used for activating and deactivating events.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_event_mask_wait(uint32_t event_mask)
{
    hal_cl_eu_event_mask_set(event_mask);
    uint32_t result = hal_cl_eu_event_wait();
    hal_cl_eu_event_mask_clear(event_mask);
    return result;
}

/**
 * \brief Wait and sleep, and clear event.
 *
 * Modify the event mask, put the core to sleep mode until it receives an event,
 * clears the active events and restore the mask.
 * This is similar to hal_cl_eu_evt_wait_and_clear() but the events whose bit is set to 1
 * in the given mask, are activated in the event mask before going to sleep, and
 * are cleared when the core is waken-up.
 *
 * \param event_mask     Event mask used for activating and deactivating events.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_event_mask_wait_and_clear(uint32_t event_mask)
{
    hal_cl_eu_event_mask_set(event_mask);
    uint32_t result = hal_cl_eu_event_wait_and_clear();
    hal_cl_eu_event_mask_clear(event_mask);
    return result;
}
/* Compat. */
#define hal_eu_evt_mask_wait_and_clr hal_cl_eu_event_mask_wait_and_clear


/**
 * \brief EU_SW_Trig.
 *
 * Functions to use to trigger a SW IRQ from FC to cluster.
 *
 * Trigger a SW IRQ to cores of a given cluster.
 */
static inline void hal_cl_eu_glob_sw_event_trigger(uint32_t cluster_id,
                                                   uint32_t sw_event,
                                                   uint32_t core_mask)
{
    uint32_t base = (uint32_t) cl_glob_eu_sw_evt(cluster_id);
    GAP_WRITE(base, CL_DEMUX_EU_SW_EVT_TRIGGER + (sw_event << 2), core_mask);
}
/* Compat. */
#define hal_eu_cluster_evt_trig_set(event_num, val) hal_cl_eu_glob_sw_event_trigger(0, event_num, val)

static inline void hal_cl_eu_sw_event_trigger(uint32_t sw_event, uint32_t core_mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_sw_evt(0);
    GAP_WRITE(base, CL_DEMUX_EU_SW_EVT_TRIGGER + (sw_event << 2), core_mask);
}


/**
 * \brief EU_Dispatch.
 *
 * Functions to dispatch messages/info to cores.
 * Used by cluster core master to send cluster task info to slaves.
 *
 * Push a message to slave cores.
 * Pop a message, by slave cores.
 * Configure cores that will receive messages.
 */
static inline void hal_cl_eu_dispatch_fifo_push(uint32_t message)
{
    uint32_t base = (uint32_t) cl_demux_eu_dispatch(0);
    GAP_WRITE(base, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS, message);
}

static inline uint32_t hal_cl_eu_dispatch_fifo_pop(void)
{
    uint32_t base = (uint32_t) cl_demux_eu_dispatch(0);
    return GAP_READ(base, CL_DEMUX_EU_DISPATCH_FIFO_ACCESS);
}

static inline void hal_cl_eu_dispatch_team_config(uint32_t team_mask)
{
    uint32_t base = (uint32_t) cl_demux_eu_dispatch(0);
    GAP_WRITE(base, CL_DEMUX_EU_DISPATCH_TEAM_CONFIG, team_mask);
}


/**
 * \brief EU_HW_Mutex.
 *
 * Functions to lock/unlock cluster HW mutex.
 *
 * Initialize mutex.
 * Lock mutex.
 * Unlock mutex.
 */
static inline void hal_cl_eu_mutex_init(uint32_t mutex_id)
{
    uint32_t base = (uint32_t) cl_demux_eu_mutex(mutex_id);
    hal_compiler_barrier();
    GAP_WRITE(base, CL_DEMUX_EU_HW_MUTEX_MUTEX, 0);
    hal_compiler_barrier();
}

static inline void hal_cl_eu_mutex_lock(uint32_t mutex_id)
{
    uint32_t base = (uint32_t) cl_demux_eu_mutex(mutex_id);
    hal_eu_read32(base, CL_DEMUX_EU_HW_MUTEX_MUTEX);
}

static inline void hal_cl_eu_mutex_unlock(uint32_t mutex_id)
{
    uint32_t base = (uint32_t) cl_demux_eu_mutex(mutex_id);
    hal_compiler_barrier();
    GAP_WRITE(base, CL_DEMUX_EU_HW_MUTEX_MUTEX, 0);
    hal_compiler_barrier();
}


/**
 * \brief EU_HW_Barrier.
 *
 * Functions to use to config/access cluster barrier.
 *
 * Setup a barrier for a mask of cores.
 * Get current status of barrier.
 * Trigger barrier with a mask.
 * Trigger barrier with core_id of core calling func.
 * Trigger barrier with core_id of core calling func and sleep.
 * Trigger barrier with core_id of core calling func and sleep and clear buffer.
 */
static inline uint32_t hal_cl_eu_barrier_addr_get(uint32_t barrier_id)
{
    return (uint32_t) cl_demux_eu_barrier(barrier_id);
}

static inline uint32_t hal_cl_eu_barrier_id_get(uint32_t bar_addr)
{
    return CL_DEMUX_EU_HW_BARRIER_ID_GET(bar_addr);
}

static inline void hal_cl_eu_barrier_setup(uint32_t bar_addr, uint32_t core_mask)
{
    GAP_WRITE(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK, core_mask);
    GAP_WRITE(bar_addr, CL_DEMUX_EU_HW_BARRIER_TARGET_MASK, core_mask);
}

static inline uint32_t hal_cl_eu_barrier_team_get(uint32_t bar_addr)
{
    uint32_t team_mask = GAP_READ(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK);
    return team_mask;
}

static inline uint32_t hal_cl_eu_barrier_status(uint32_t bar_addr)
{
    return hal_eu_read32(bar_addr, CL_DEMUX_EU_HW_BARRIER_STATUS);
}

static inline void hal_cl_eu_barrier_trigger(uint32_t bar_addr, uint32_t core_mask)
{
    GAP_WRITE(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER, core_mask);
}

static inline uint32_t hal_cl_eu_barrier_trigger_self(uint32_t bar_addr)
{
    return hal_eu_read32(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER_SELF);
}

static inline uint32_t hal_cl_eu_barrier_trigger_wait(uint32_t bar_addr)
{
    return hal_eu_read32(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT);
}

static inline uint32_t hal_cl_eu_barrier_trigger_wait_clear(uint32_t bar_addr)
{
    return hal_eu_read32(bar_addr, CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR);
}

#endif  /* __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__ */
