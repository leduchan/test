/*
 * Copyright (C) 2017 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include "pmsis.h"
#include "stdio.h"
#include <bsp/bsp.h>
#ifdef USE_HYPERRAM
#include <bsp/ram/hyperram.h>
#endif
#ifdef USE_SPIRAM
#include <bsp/ram/spiram.h>
#endif

#define BUFF_SIZE 2048

static char *buff[2];
static char *rcv_buff[2];
static uint32_t hyper_buff[2];
static int count = 0;
static struct pi_device hyper;
static struct pi_task fc_tasks[2];

static void end_of_rx(void *arg)
{
  int i = (int)arg;
  printf("End of RX for id %d\n", (int)arg);
  count++;
}

static void end_of_tx(void *arg)
{
  printf("End of TX for id %d\n", (int)arg);
  int i = (int)arg;
  pi_ram_read_async(&hyper, hyper_buff[i], rcv_buff[i], BUFF_SIZE, pi_task_callback(&fc_tasks[i], end_of_rx, (void *)i));
}

int test_entry()
{

#if defined(USE_HYPERRAM) || defined(USE_SPIRAM)

#ifdef USE_HYPERRAM
    printf("Entering main controller (mode: hyperram)\n");

  struct pi_hyperram_conf conf;
  pi_hyperram_conf_init(&conf);
#else
#if defined(CONFIG_APS25XXXN)
    printf("Entering main controller (mode: octospi ram aps25xxn)\n");

  struct pi_aps25xxxn_conf conf;
  pi_aps25xxxn_conf_init(&conf);
#else
    printf("Entering main controller (mode: quad spi ram)\n");

  struct pi_spiram_conf conf;
  pi_spiram_conf_init(&conf);
#endif
#endif

#else

  printf("Entering main controller (mode: default)\n");

  struct pi_default_ram_conf conf;
  pi_default_ram_conf_init(&conf);

#endif

  for (int i=0; i<2; i++)
  {
    buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (buff[i] == NULL)
      return -1;
    rcv_buff[i] = pmsis_l2_malloc(BUFF_SIZE);
    if (rcv_buff[i] == NULL)
      return -2;
  }

  pi_open_from_conf(&hyper, &conf);

  if (pi_ram_open(&hyper))
    return -3;

  if (pi_ram_alloc(&hyper, &hyper_buff[0], BUFF_SIZE))
    return -4;

  if (pi_ram_alloc(&hyper, &hyper_buff[1], BUFF_SIZE))
    return -5;

  for (int i=0; i<BUFF_SIZE; i++)
    {
      buff[0][i] = i & 0x7f;
      buff[1][i] = i | 0x80;
    }

#ifdef ASYNC
  pi_ram_write_async(&hyper, hyper_buff[0], buff[0], BUFF_SIZE, pi_task_callback(&fc_tasks[0], end_of_tx, (void *)0));

  pi_ram_write_async(&hyper, hyper_buff[1], buff[1], BUFF_SIZE, pi_task_callback(&fc_tasks[1], end_of_tx, (void *)1));

  while(count != 2) {
    pi_yield();
  }
#else
  pi_ram_write(&hyper, hyper_buff[0], buff[0], BUFF_SIZE);
  pi_ram_write(&hyper, hyper_buff[1], buff[1], BUFF_SIZE);
  pi_ram_read(&hyper, hyper_buff[0], rcv_buff[0], BUFF_SIZE);
  pi_ram_read(&hyper, hyper_buff[1], rcv_buff[1], BUFF_SIZE);
#endif

  for (int j=0; j<2; j++)
  {
    for (int i=0; i<BUFF_SIZE; i++)
    {
      unsigned char expected;
      if (j == 0)
        expected = i & 0x7f;
      else
        expected = i | 0x80;
      if (expected != rcv_buff[j][i])
      {
        printf("Error, buffer: %d, index: %d, expected: 0x%x, read: 0x%x\n", j, i, expected, rcv_buff[j][i]);
        return -6;
      }
    }
  }

  pi_ram_free(&hyper, hyper_buff[0], BUFF_SIZE);
  pi_ram_free(&hyper, hyper_buff[1], BUFF_SIZE);
  pi_ram_close(&hyper);

  printf("TEST SUCCESS\n");

  return 0;
}

void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}

int main()
{
  return pmsis_kickoff((void *)test_kickoff);
}
