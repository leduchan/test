/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define NO_PRINTF

#if defined(NO_PRINTF)
#define PRINTF(...)      ( (void) 0 )
#else
#define PRINTF           ( printf )
#endif  /* NO_PRINTF */

#define NB_ITER          ( 4 )
#define NB_FAIL          ( 1 )

PI_L2 static pi_device_t uart;
PI_L2 char hello_tx[20] = {0};
PI_L2 static uint32_t hello_tx_len = 0;
PI_L2 uint32_t timeout_us[NB_ITER] = {2000,   /* OK */
                                      0,      /* OK */
                                      10,     /* KO */
                                      1000000 /* OK */
};

#if (UART_TIMEOUT_MODE == 1)
static uint8_t timeout_mode = (uint8_t) PI_UDMA_TIMEOUT_MODE_TRANSFER;
#else
static uint8_t timeout_mode = (uint8_t) PI_UDMA_TIMEOUT_MODE_RXTX;
#endif  /* UART_TIMEOUT_MODE */

#if (ASYNC)
static pi_task_t wait_tasks[NB_ITER] = {0};

int32_t __timeout_async_exec(uint32_t iter)
{
    int32_t errors = 0;
    int32_t status = -1;
    uint32_t timeout = timeout_us[iter];
    pi_task_t *wait_task = &wait_tasks[iter];

    PRINTF("Enqueueing TX with timeout=%ldus\n", timeout);
    pi_task_block(wait_task);
    pi_task_timeout_set(wait_task, timeout);
    pi_uart_write_async(&uart, hello_tx, hello_tx_len, wait_task);
    pi_task_wait_on(wait_task);
    status = pi_task_transfer_end_result_get(wait_task);
    errors += (status != 0);
    #if !defined(NO_PRINTF)
    if (status)
    {
        PRINTF("Timeout reached %ld us ! Test failed with %ld remaining bytes...\n",
               timeout, status);
    }
    #endif  /* NO_PRINTF */
    return errors;
}

int32_t test_timeout_async(void)
{
    int32_t errors = 0;
    for (uint32_t iter=0; iter<(uint32_t) NB_ITER; iter++)
    {
        errors += __timeout_async_exec(iter);
    }

    return errors;
}

#else

int32_t test_timeout_sync(void)
{
    int32_t errors = 0;
    int32_t status = -1;
    uint32_t timeout = 0;
    for (uint32_t iter=0; iter<(uint32_t) NB_ITER; iter++)
    {
        timeout = timeout_us[iter];
        PRINTF("Sending TX with timeout=%ldus\n", timeout);
        status = pi_uart_write_timeout(&uart, hello_tx, hello_tx_len, timeout);
        errors += (status != 0);
        #if !defined(NO_PRINTF)
        if (status)
        {
            PRINTF("Timeout reached %ld us ! Test failed with %ld remaining bytes...\n",
                   timeout, status);
        }
        #endif  /* NO_PRINTF */
        pi_uart_ioctl(&uart, PI_UART_IOCTL_FLUSH, NULL);
    }
    return errors;
}
#endif  /* ASYNC */


void test_entry(void)
{
    struct pi_uart_conf conf;

    /* Init & open uart. */
    pi_uart_conf_init(&conf);
    conf.uart_id = (uint8_t) UART_ITF;
    conf.enable_tx = 1;
    conf.enable_rx = 0;
    conf.baudrate_bps = (uint32_t) UART_BAUDRATE;
    pi_open_from_conf(&uart, &conf);
    if (pi_uart_open(&uart))
    {
        PRINTF("Uart open failed !\n");
        pmsis_exit(-1);
    }
    PRINTF("Timeout mode=%d\n", timeout_mode);
    int32_t timeout_id = pi_udma_timeout_alloc(timeout_mode);
    if (timeout_id == -1)
    {
        PRINTF("Timeout alloc failed !\n");
        pmsis_exit(-2);
    }
    pi_uart_ioctl(&uart, PI_UART_IOCTL_ATTACH_TIMEOUT_TX, (void *) timeout_id);

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    sprintf(hello_tx, "[%d %d] Hello World !\n", cluster_id, core_id);
    hello_tx_len = strlen(hello_tx);

    int32_t errors = 0;
    #if (ASYNC)
    errors = test_timeout_async();
    #else
    errors = test_timeout_sync();
    #endif  /* ASYNC */

    pi_uart_close(&uart);

    if (errors != (int32_t) NB_FAIL)
    {
        printf("Test failed !\n");
    }
    else
    {
        printf("Test success !\n");
        errors = 0;
    }
    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    return pmsis_kickoff((void *) test_entry);
}
