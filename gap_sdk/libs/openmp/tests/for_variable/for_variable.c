/* PMSIS includes */
#include "pmsis.h"
#include "omp.h"

static uint32_t errors = 0;

/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");

    uint32_t a[64] = {0};
    int32_t sum = 0;
    int32_t max = 0;

    #pragma omp parallel num_threads(4)
    {
        printf("[%d %d] Fork entry\n", pi_cluster_id(), omp_get_thread_num() );

        #pragma omp for
        for (int i=0; i<64; i++)
        {
            a[i] = omp_get_thread_num();
        }

        #pragma omp for
        for (int i = 0; i < 64; i++)
        {
            #pragma omp atomic
            sum += a[i];
        }

        #pragma omp barrier
        #pragma omp for
        for (int j = 0; j < sum; j++)
        {
            printf("[%d] - for index %d\n", omp_get_thread_num(), j);
            #pragma omp critical
            {
                if (j > max)
                {
                    max = j;
                }
            }
        }

    }

    printf("Sum: %d\n", sum);
    printf("max: %d\n", max);

    if (sum - 1 != max)
    {
        errors++;
    }

    printf("Cluster master core exit\n");
}

void helloworld(void)
{
    printf("Entering main controller\n");
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task;
    pi_cluster_task(&cl_task, cluster_delegate, NULL);

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    if (errors)
    {
        printf("Test failed!\n");
    }
    else
    {
        printf("Test success !\n");
    }

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS HelloWorld ***\n\n");
    return pmsis_kickoff((void *) helloworld);
}

