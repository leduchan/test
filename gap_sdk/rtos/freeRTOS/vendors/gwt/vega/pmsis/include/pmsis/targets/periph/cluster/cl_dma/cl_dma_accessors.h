
/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT.
 */

/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __ARCHI_CL_DMA_ACCESSORS__
#define __ARCHI_CL_DMA_ACCESSORS__

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

#include <stdint.h>

#endif




//
// REGISTERS ACCESS FUNCTIONS
//

#if !defined(LANGUAGE_ASSEMBLY) && !defined(__ASSEMBLER__)

static inline uint32_t cl_dma_cmd_get(uint32_t base)
{
    return GAP_READ(base, CL_DMA_CMD_OFFSET);
}

static inline void cl_dma_cmd_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CL_DMA_CMD_OFFSET, value);
}


static inline uint32_t cl_dma_status_get(uint32_t base)
{
    return GAP_READ(base, CL_DMA_STATUS_OFFSET);
}

static inline void cl_dma_status_set(uint32_t base, uint32_t value)
{
    GAP_WRITE(base, CL_DMA_STATUS_OFFSET, value);
}


#endif

#endif
