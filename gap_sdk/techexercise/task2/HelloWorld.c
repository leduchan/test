/**************************************************
Candidate: LE Duc Han
Current role: Senior Software Engineer 
Applied role: Embedded Software Engineer.
Company: GreenWaves Technologies.
Instructions:
    Task 1:
  	- Write a program which would make all the cores print
   	“hello world [cluster_id, core_id]”.
    	- This code should run on GAP8 GVSoc.

**************************************************/
// PMSIS includes.
#include "pmsis.h"

// Task executed by cluster cores.
void cluster_entry(void *arg) {
    // Get the core ID.
    int core_id = pi_core_id();
    // Get the cluster ID. 
    int cluster_id = pi_cluster_id(); 

    printf("Hello world [Cluster %d, Core %d]\n", cluster_id, core_id);
}

// Cluster main entry, executed by core 0. 
void cluster_delegate(void *arg)
{
    printf("Cluster master core entry\n");
    // Task dispatch to cluster cores. 
    pi_cl_team_fork(pi_cl_cluster_nb_cores(), cluster_entry, arg);
    printf("Cluster master core exit\n");
}

void helloWorld(void) {
    
    printf("Entering main controller\n");

    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();
    printf("[%d %d] Hello World!\n", cluster_id, core_id);

    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;

    // Init cluster configuration structure. 
    pi_cluster_conf_init(&cl_conf);
    // Set cluster ID.
    cl_conf.id = 0;

    // Configure & open cluster. 
    pi_open_from_conf(&cluster_dev, &cl_conf);

    if (pi_cluster_open(&cluster_dev)) {
        printf("Cluster open failed!\n");
        pmsis_exit(-1);
    }

    // Prepare cluster task and send it to cluster. 
    struct pi_cluster_task cl_task;
    pi_cluster_send_task_to_cl(&cluster_dev, pi_cluster_task(&cl_task, cluster_delegate, NULL));

    // Close the cluster.
    pi_cluster_close(&cluster_dev);

    printf("Test success!\n");

    // Exit with status 0.
    pmsis_exit(0);
}

int main() {

    printf("\n\n\t *** PMSIS HelloWorld ***\n\n"); 
    return pmsis_kickoff((void *) helloWorld);
}

