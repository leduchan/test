/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and
 * GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RT_IMPLEM_IMPLEM_H__
#define __RT_IMPLEM_IMPLEM_H__

/// @cond IMPLEM

#include "rt/implem/utils.h"
#include "rt/implem/hyperram.h"
#include "rt/implem/cluster.h"
#include "rt/implem/udma.h"

#if PULP_CHIP == CHIP_VEGA
#include "rt/implem/vega.h"
#elif PULP_CHIP == CHIP_GAP9
#include "rt/implem/gap9.h"
#endif


void __rt_deinit();

extern void __pi_yield();

static inline void pi_task_timeout_set(pi_task_t *task, uint32_t timeout_us)
{

}

static inline int32_t pi_task_transfer_end_result_get(pi_task_t *task)
{
  return 0;
}

static inline struct pi_task *pi_task_block(struct pi_task *task)
{
  task->id = PI_TASK_NONE_ID;
  task->arg[0] = (uint32_t)0;
  task->implem.keep = 1;
  __rt_task_init(task);
  return task;
}

static inline void pi_task_destroy(pi_task_t *task)
{
}

static inline int pmsis_kickoff(void *arg)
{
  ((void (*)())arg)();
  return -1;
}

static inline void pmsis_exit(int err)
{
  exit(err);
}

static inline void pi_yield()
{
  rt_event_yield(NULL);
}

static inline uint32_t __native_core_id() {
  return rt_core_id();
}

static inline uint32_t __native_cluster_id() {
  return rt_cluster_id();
}

static inline uint32_t __native_is_fc() {
  return rt_is_fc();
}

static inline uint32_t __native_native_nb_cores() {
  return rt_nb_pe();
}


/// @endcond

#endif
