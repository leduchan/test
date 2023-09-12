/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __POS_DATA_KERNEL_H__
#define __POS_DATA_KERNEL_H__

#ifndef LANGUAGE_ASSEMBLY

typedef struct pos_cbsys_s
{
    int (*callback)(void *);
    void *arg;
    struct pos_cbsys_s *next;
    uint8_t id;
} pos_cbsys_t;

#endif

#endif
