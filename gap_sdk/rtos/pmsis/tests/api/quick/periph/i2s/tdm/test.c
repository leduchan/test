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

#include "bsp/bsp.h"
#include "pmsis.h"
#ifdef USE_FFT
#include "kiss_fft.h"
#endif



#ifndef NB_ITF
#define NB_ITF 1
#endif

#ifndef NB_CHANNELS
#define NB_CHANNELS 1
#endif

#define ERROR_RATE 20
#if WORD_SIZE == 32 || WORD_SIZE == 24
#define ELEM_SIZE 4
#define ELEM_TYPE uint32_t
#define MEM_WORD_SIZE 32
#elif WORD_SIZE == 16
#define ELEM_SIZE 2
#define ELEM_TYPE uint16_t
#define MEM_WORD_SIZE 16
#elif WORD_SIZE == 8
#define ELEM_SIZE 1
#define ELEM_TYPE uint8_t
#define MEM_WORD_SIZE 8
#else
#error Unsupported word size
#endif
#define BUFF_SIZE  (NB_ELEM*ELEM_SIZE)
#define BLOCK_SIZE (NB_ELEM*ELEM_SIZE)

static uint8_t *buff[NB_ITF][2];
static uint8_t *tx_buff[NB_ITF][2];
static uint8_t *ch_buff[NB_ITF][NB_CHANNELS];
static uint8_t *ch_tx_buff[NB_ITF][NB_CHANNELS];
static struct pi_device i2s[NB_ITF];
static int max[NB_ITF][NB_CHANNELS];
static int min[NB_ITF][NB_CHANNELS];

static pi_task_t i2s_tasks[NB_ITF][NB_CHANNELS];



#ifdef USE_FFT
static kiss_fft_cpx buff_complex[NB_ELEM];
static kiss_fft_cpx buff_complex_out[NB_ELEM];
static kiss_fft_cfg cfg;
#endif


#ifdef TX_ENABLED

static int tx_buffer_current_val[NB_ITF][NB_CHANNELS];
static int tx_buffer_current_index[NB_ITF][NB_CHANNELS];

static int get_write_buffer_start(int itf, int channel)
{
  return TX_BUFFER_START0_0;
}

static int get_write_buffer_next(int value, int itf, int channel)
{
  return value + TX_BUFFER_NEXT0_0;
}

void fill_tx_buffer(int i, int j)
{
  int16_t *short_buff = (int16_t *)tx_buff[i][tx_buffer_current_index[i][j]];
  tx_buffer_current_index[i][j] ^= 1;
  for (int k=0; k<BUFF_SIZE/2; k++)
  {
    short_buff[j*BUFF_SIZE + k] = tx_buffer_current_val[i][j];
    tx_buffer_current_val[i][j] = get_write_buffer_next(tx_buffer_current_val[i][j], i, j);
  }
}

#endif

static unsigned int bit_inverse(unsigned int val, unsigned char word_size)
{
    unsigned int v = val;     // input bits to be reversed
    unsigned int r = v & 1; // r will be reversed bits of v; first get LSB of v
    int s = 32 - 1; // extra shift needed at end

    for (v >>= 1; v; v >>= 1)
    {
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s; // shift when v's highest bits are zero

    return (r>>(32-word_size));
}

void i2s_reg_dump( unsigned char itf )
{
    printf("I2S%d reg dump \n =============================================\n", itf);
#ifdef __FREERTOS__
#define I2S_BASE	( UDMA_I2S(itf) )
#else
#define I2S_BASE	( UDMA_I2S_ADDR(itf) )
#endif
    printf("I2S BASE: 0x%8x \n",   I2S_BASE);
    for (int i=0; i<16; i++)
        printf("I2S SLOT_CFG_%d: 0x%8X\n", i, *(volatile unsigned int *)(long)(I2S_BASE + (i*0x4)));
    printf("I2S WORD_SIZE_0_1:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x40));
    printf("I2S WORD_SIZE_2_3:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x44));
    printf("I2S WORD_SIZE_4_5:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x48));
    printf("I2S WORD_SIZE_6_7:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x4C));
    printf("I2S WORD_SIZE_8_9:   0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x50));
    printf("I2S WORD_SIZE_10_11: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x54));
    printf("I2S WORD_SIZE_12_13: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x58));
    printf("I2S WORD_SIZE_14_15: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x5C));

    printf("I2S SLOT_EN: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x60));
    printf("I2S CLKCFG_SETUP: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x64));
    printf("I2S GLB_SETUP: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x68));
    printf("I2S CLK_FAST: 0x%8X\n", *(volatile unsigned int *)(long)(I2S_BASE + 0x6C));
    printf("=============================================");
}

#ifndef USE_FFT
static int get_incr_start(int start_value, int itf, int channel, int *error, uint32_t *r_start)
{
  uint32_t incr = 0;
  uint32_t start = 0;
  if (itf == 0)
  {
    if (channel == 0)
    {
      incr = INCR_VALUE0_0;
      start = INCR_START0_0;
    }
    else if (channel == 1)
    {
      incr = INCR_VALUE0_1;
      start = INCR_START0_1;
    }
    else if (channel == 2)
    {
      incr = INCR_VALUE0_2;
      start = INCR_START0_2;
    }
    else if (channel == 3)
    {
      incr = INCR_VALUE0_3;
      start = INCR_START0_3;
    }
  }

  uint64_t mask = (1ULL << WORD_SIZE) - 1;
  incr &= (1ULL << WORD_SIZE) - 1;
  start &= (1ULL << WORD_SIZE) - 1;

#if defined(LSB_MODE) && LSB_MODE == 1
  start_value = bit_inverse(start_value, WORD_SIZE);
#endif

  //printf("Looking for value %x, incr: %x, start: %x\n", start_value, incr, start);
  *r_start = start;

  if (((start_value - start) % incr) != 0)
  {
    *error = 1;
    return 0;
   }
  return start_value;
}

static int __attribute__((noinline)) get_incr_next(uint32_t value, int itf, int channel, uint32_t start)
{

  uint32_t val_incr = 0;

  if (itf == 0)
  {
    uint32_t incr;

    if (channel == 0)
      val_incr = INCR_VALUE0_0;
    else if (channel == 1)
      val_incr = INCR_VALUE0_1;
    else if (channel == 2)
      val_incr = INCR_VALUE0_2;
    else if (channel == 3)
      val_incr = INCR_VALUE0_3;

/*
#if defined(WS_DELAY) && WS_DELAY < 1
    uint64_t incr_value = (uint64_t)value + (val_incr>>1);
#else
    uint64_t incr_value = (uint64_t)value + (val_incr<<(WS_DELAY-1));
#endif
*/

    uint64_t incr_value = (uint64_t)value + val_incr;
    uint64_t incr_value_trunc = incr_value & ((1ULL<<WORD_SIZE) - 1);

    if (incr_value_trunc != incr_value)
    {

      value = start;
      return value;
    }

    return incr_value;
  }

  return value + 1;
}
#endif


static int get_sampling_freq(int itf)
{
  if (itf == 0)
    return SAMPLING_FREQ_0;
  else
    return SAMPLING_FREQ_1;
}


static int check_error(int bound, int value, float error_margin)
{
  float error = (float)(bound - value) / bound;
  if (error < 0)
    error = -error;

  printf("    Got bound error rate %f (bound: 0x%x, got: 0x%x)\n", error, bound, value);

  return error > error_margin;
}


static int get_signal_freq(int itf, int channel)
{
  if (itf == 0)
  {
//#if defined(MODE) && MODE == USE_2CH
//    if (channel == 0)
//      return SIGNAL_FREQ_0_0;
//    else if (channel == 2)
//      return SIGNAL_FREQ_0_1;
//    else if (channel == 1)
//      return SIGNAL_FREQ_1_0;
//    else
//      return SIGNAL_FREQ_1_1;
//#else
    if (channel == 0)
      return SIGNAL_FREQ_0_0;
    else if (channel == 1)
      return SIGNAL_FREQ_0_1;
    else if (channel == 2)
      return SIGNAL_FREQ_0_2;
    else if (channel == 3)
      return SIGNAL_FREQ_0_3;
//#endif
  }

  return SIGNAL_FREQ_0_0;
}

static int check_buffer(uint8_t *buff, int sampling_freq, int signal_freq, int itf, int channel)
{
#ifdef USE_FFT
  for (int i=0; i<NB_ELEM; i++)
  {
    //printf("%x\n", (((int16_t *)buff)[i+16]));

#if WORD_SIZE <= 16
    buff_complex[i].r = (float)(((int16_t *)buff)[i]);
#else
    buff_complex[i].r = (float)(((int32_t *)buff)[i]);
#endif
    buff_complex[i].i = 0.0;
  }

  // Get signal frequencies
  kiss_fft(cfg, buff_complex, buff_complex_out);

  float max = 0;
  int index = -1;

  // Find the most important one
  for (int i=0; i<NB_ELEM; i++)
  {
    float val = buff_complex_out[i].r*buff_complex_out[i].r*1.0/sampling_freq + buff_complex_out[i].i*buff_complex_out[i].i*1.0/sampling_freq;
    if (index == -1 || val > max)
    {
      index = i;
      max = val;
    }
  }

  float freq = (int)((float)index / NB_ELEM * sampling_freq);
  float error = (freq - signal_freq) / signal_freq * 100;
  if (error < 0)
    error = -error;

  printf("    Got error rate %d %% (expected: %d, got %d)\n", (int)error, (int)signal_freq, (int)freq);

  return error > ERROR_RATE;
#else

  ELEM_TYPE *check_buff = (ELEM_TYPE *)buff;
  ELEM_TYPE expected;

  int error = 0;
  uint32_t start;

  expected = get_incr_start(check_buff[0], itf, channel, &error, &start);
  if (error)
  {
    printf("Could not find initial value from buffer (value: 0x%x)\n", check_buff[0]);
    //return -1;
  }

  for (int i=0; i<NB_ELEM; i++)
  {
    expected &= (1ULL<<WORD_SIZE) - 1;

#if defined(LSB_MODE) && LSB_MODE == 1
    check_buff[i] = bit_inverse(check_buff[i], WORD_SIZE);
#endif

    if (expected != check_buff[i])
    {
      printf("Error at index %d, expected %x, got %x\n", i, expected, check_buff[i]);
      return -1;
    }

    expected = get_incr_next(expected, itf, channel, start);
    //printf("expected value: %x\n", expected);
  }

  //printf("Buffer check success\n");

  return 0;

#endif
}

static int test_entry()
{
  int errors = 0;

  //printf("Test I2S with TDM=%d NB_CHANNELS=%d NB_ELEM=%d WORD_SIZE=%d LSB_MODE=%d WS_DELAY=%d\n", TDM, NB_CHANNELS, NB_ELEM, WORD_SIZE, LSB_MODE, WS_DELAY);

  for (int i=0; i<NB_ITF; i++)
  {
    buff[i][0] = pi_l2_malloc(BUFF_SIZE*NB_CHANNELS);
    buff[i][1] = pi_l2_malloc(BUFF_SIZE*NB_CHANNELS);

    tx_buff[i][0] = pi_l2_malloc(BUFF_SIZE*NB_CHANNELS);
    tx_buff[i][1] = pi_l2_malloc(BUFF_SIZE*NB_CHANNELS);

    for (int j=0; j<NB_CHANNELS; j++)
    {
      ch_buff[i][j] = pi_l2_malloc(BUFF_SIZE);
#ifdef TX_ENABLED
      ch_tx_buff[i][j] = pi_l2_malloc(BUFF_SIZE);
#endif
    }
  }



  for (int i=0; i<NB_ITF; i++)
  {
    struct pi_i2s_conf i2s_conf;
    pi_i2s_conf_init(&i2s_conf);

//#if (defined(RX_ENABLED) && defined(TX_ENABLED)) || defined (LOOPBACK_ENABLED)
    i2s_conf.options = PI_I2S_OPT_FULL_DUPLEX;
//#else
    //i2s_conf.options = 0;
//#endif
//    i2s_conf.pingpong_buffers[0] = buff[i][0];
//    i2s_conf.pingpong_buffers[1] = buff[i][1];
//    i2s_conf.block_size = BLOCK_SIZE;
    i2s_conf.frame_clk_freq = get_sampling_freq(i);
    i2s_conf.itf = i;
    i2s_conf.format = PI_I2S_FMT_DATA_FORMAT_I2S;

#if defined(LSB_MODE) && LSB_MODE == 1
    i2s_conf.format |= PI_I2S_CH_FMT_DATA_ORDER_LSB;
#else
    i2s_conf.format |= PI_I2S_CH_FMT_DATA_ORDER_MSB;
#endif

    i2s_conf.format |= PI_I2S_CH_FMT_DATA_ALIGN_RIGHT;
    i2s_conf.format |= PI_I2S_CH_FMT_DATA_SIGN_NO_EXTEND;

#if defined(WS_DELAY) && WS_DELAY != 1
    i2s_conf.ws_delay = WS_DELAY;
#endif
    i2s_conf.word_size = WORD_SIZE;
    i2s_conf.mem_word_size = MEM_WORD_SIZE;
    i2s_conf.channels = NB_CHANNELS;
#if defined(TDM) && TDM == 1
    i2s_conf.options |= PI_I2S_OPT_TDM;
#endif

#ifdef USE_EXT_CLOCK
    i2s_conf.options |= PI_I2S_OPT_EXT_CLK;
#endif

#ifdef USE_EXT_WS
    i2s_conf.options |= PI_I2S_OPT_EXT_WS;
#endif

#ifdef USE_REF_CLK
    i2s_conf.options |= PI_I2S_OPT_REF_CLK_FAST;
    i2s_conf.ref_clk_freq = REF_CLK_FREQ;
#endif

    pi_open_from_conf(&i2s[i], &i2s_conf);

    if (pi_i2s_open(&i2s[i]))
      return -1;

#if defined(TDM) && TDM == 1
    struct pi_i2s_channel_conf channel_conf;
    pi_i2s_channel_conf_init(&channel_conf);

#ifdef LOOPBACK_ENABLED
    channel_conf.options |= PI_I2S_OPT_LOOPBACK;
#endif

    for (int j=0; j<NB_CHANNELS; j++)
    {
#ifdef RX_ENABLED
      channel_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_RX | PI_I2S_OPT_ENABLED;
      channel_conf.block_size = BLOCK_SIZE;
      channel_conf.word_size = WORD_SIZE;
      channel_conf.mem_word_size = MEM_WORD_SIZE;

      channel_conf.pingpong_buffers[0] = &buff[i][0][j*BUFF_SIZE];
      channel_conf.pingpong_buffers[1] = &buff[i][1][j*BUFF_SIZE];
      pi_i2s_channel_conf_set(&i2s[i], j, &channel_conf);
#endif
    }

    for (int j=0; j<NB_CHANNELS; j++)
    {
#ifdef TX_ENABLED
      channel_conf.options = PI_I2S_OPT_PINGPONG | PI_I2S_OPT_IS_TX | PI_I2S_OPT_ENABLED;
      channel_conf.block_size = BLOCK_SIZE;
      channel_conf.word_size = WORD_SIZE;
      channel_conf.mem_word_size = MEM_WORD_SIZE;

      tx_buffer_current_val[i][j] = get_write_buffer_start(i, j);
      tx_buffer_current_index[i][j] = 0;
      fill_tx_buffer(i, j);

      channel_conf.pingpong_buffers[0] = &tx_buff[i][0][j*BUFF_SIZE];
      channel_conf.pingpong_buffers[1] = &tx_buff[i][1][j*BUFF_SIZE];
      pi_i2s_channel_conf_set(&i2s[i], j, &channel_conf);
#endif
    }

#endif
  }

  for (int i=0; i<NB_ITF; i++)
  {
    if (pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_START, NULL))
      return -1;
//    i2s_reg_dump(i);
  }

#if defined(TDM) && TDM == 1
  void *chunk[NB_ITF][NB_CHANNELS];
  for (int i=0; i<NB_ITF; i++)
  {
    unsigned int size;

    for (int j=0; j<NB_CHANNELS; j++)
    {
#ifdef TX_ENABLED
      pi_i2s_channel_write_async(&i2s[i], j, NULL, size, pi_task_block(&i2s_tasks[i][j]));
#endif
#ifdef RX_ENABLED
      pi_i2s_channel_read(&i2s[i], j, &chunk[i][j], &size);
#endif
    }
  }

  for (int i=0; i<NB_ITF; i++)
  {
    unsigned int size;

    for (int j=0; j<NB_CHANNELS; j++)
    {
#ifdef TX_ENABLED
      pi_task_wait_on(&i2s_tasks[i][j]);
#endif
    }
  }



#else
  void *chunk[NB_ITF];
  for (int i=0; i<NB_ITF; i++)
  {
    unsigned int size;


    pi_i2s_read(&i2s[i], &chunk[i], &size);
  }
#endif


#ifdef TX_ENABLED
  // FIXME : Working without this delay, so it's Fixed ????
  // There is no HW support to know when the pending samples are flushed, so just wait a bit for now
  for (volatile int i=0; i<1000; i++);
#endif


  for (int i=0; i<NB_ITF; i++)
  {
    pi_i2s_ioctl(&i2s[i], PI_I2S_IOCTL_STOP, NULL);
    pi_i2s_close(&i2s[i]);
  }

#if 0
#if defined(TDM) && TDM == 1
  for (int i=0; i<NB_CHANNELS; i++)
  {
    for (int j=0; j<BLOCK_SIZE/ELEM_SIZE; j++)
    {
      if (WORD_SIZE <= 16)
      {
        short *buff0 = (short *)&buff[0][0][i*BUFF_SIZE];
        printf("%d: %d: %p: %d %x\n", i, j, &buff0[j], buff0[j], buff0[j]);
      }
      else
      {
        int *buff0 = (int *)&buff[0][0][i*BUFF_SIZE];
        printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
      }
    }
  }
#else
  printf("Raw buffer size\n");
  for (int j=0; j<BLOCK_SIZE/ELEM_SIZE; j++)
  {
    if (WORD_SIZE <= 16)
    {
      short *buff0 = buff[0][0];
      printf("%d: %p: %d %x\n", j, &buff0[j], buff0[j], buff0[j]);
    }
    else
    {
      int *buff0 = buff[0][0];
      printf("%p: %d %x\n", &buff0[j], buff0[j], buff0[j]);
    }
  }
#endif
#endif


#ifdef RX_ENABLED

#ifdef USE_FFT
  cfg = kiss_fft_alloc(NB_ELEM, 0, NULL, NULL);

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {
      min[i][k] = 0;
      max[i][k] = 0;

      for (int j=0; j<NB_ELEM; j++)
      {
        int elem_value = 0;

        for (int l=0; l<ELEM_SIZE; l++)
        {
          int value = buff[i][0][k*BUFF_SIZE+j*ELEM_SIZE+l];

          elem_value |= value << (l*8);
        }

        if (WORD_SIZE == 16)
        {
          elem_value = (elem_value << 16) >> 16;
        }

        if (elem_value > max[i][k])
          max[i][k] = elem_value;
        if (elem_value < min[i][k])
          min[i][k] = elem_value;
      }
    }
  }
#endif

#if 0
  printf("Sorted buffer\n");
  for (int j=0; j<BUFF_SIZE/2; j++)
  {
    short *buff0 = ch_buff[0][0];
    printf("%d\n", buff0[j]);
  }
#endif

  for (int i=0; i<NB_ITF; i++)
  {
    for (int k=0; k<NB_CHANNELS; k++)
    {
      //printf("Checking itf %d channel %d\n", i, k);

#ifdef USE_FFT
      if (check_error((1ULL<<(WORD_SIZE-1)) - 1, max[i][k], 0.2))
        return -1;

      if (check_error(-(1LL<<(WORD_SIZE-1)), min[i][k], 0.2))
        return -1;
#endif

      errors += check_buffer(chunk[i][k], get_sampling_freq(i), get_signal_freq(i, k), i, k);
      if (errors)
      return -1;
    }
  }
#endif

  if (errors)
    printf("TEST FAILURE\n");
  else
    printf("TEST SUCCESS\n");

  return errors;
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
