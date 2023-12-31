#include "pmsis.h"
#include "omp.h"

__attribute__((weak)) int omp_get_thread_num(void)
{
    return pi_core_id();
}

__attribute__((weak)) int omp_get_num_threads(void)
{
    return pi_cl_team_nb_cores();
}
