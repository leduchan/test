/**************************************************
Candidate: LE Duc Han 
Senior Software Engineer.
Applied role: Embedded Software Engineer.
Company: GreenWaves Technologies.
Instructions:
    3. Write a C program to implement:
    	- Create and initialize two square matrices in L2 memory.
        - Copy these two matrices from L2 memory to L1 memory. 
	- Addition of the two copied matrices. 
	- Multiplication of the two copied matrices. 
	- The two result matrices allocated in L1 memory. 
	- Copy the two result matrices from L1 memory to L2 memory.
        - do a convolution of the multiplication result matrix with the filter below:
               `unsigned short filter[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1}; 	

    This code should run on GAP8 GVSoc.
**************************************************/
#include <stdio.h>
#include "pmsis.h"

#define MATRIX_SIZE 64

/*===============================================
 The basic functions
================================================*/
void copy_matrix(unsigned short *src, unsigned short *dest, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            dest[i * size + j] = src[i * size + j];
        }
    }
}

// Function to perform matrix addition
void matrix_addition(unsigned short *mat1, unsigned short *mat2, unsigned short *res, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            res[i * size +j] = mat1[i * size +j] + mat2[i * size +j];
        }
    }
}

// Function to perform matrix multiplication
void matrix_multiplication(unsigned short *mat1, unsigned short *mat2, unsigned short *res, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            res[i * size + j] = 0;
            for (int k = 0; k < size; k++) {
                res[i * size +j] += mat1[i * size +j] * mat2[i * size +j];
            }
        }
    }
}

void convolution(unsigned short *input, unsigned short *output, unsigned short filter[3][3], int size) {
    for (int i = 1; i < size - 1; i++) {
        for (int j = 1; j < size - 1; j++) {
            int sum = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    sum += input[(i + x) * size + (j + y)] * filter[x + 1][y + 1];
                }
            }
            output[i * size + j] = (unsigned short)(sum > 0 ? sum : 0); // Apply ReLU activation
        }
    }
}

// Function to compare two matrices for equality
int is_matrix_identical(unsigned short *mat1, unsigned short *mat2, int size) { 

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if ( mat2[i*size +j] != mat1[i * size +j]) {
		return 0; // Matrices are not identical.
            }
        }
    }

    return 1; // Matrices are identical.
}

// Print matrix elements. 
void print_mat_elements(unsigned short *mat, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%5d ", mat[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }
}

/*=====================================================
 Task execution.
=======================================================*/
void taskExecution(void) {
     
    uint32_t errors = 0;
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    
    /*--------------------------------------------------
    Init cluster configuration structure. 
    ---------------------------------------------------*/
    pi_cluster_conf_init(&cl_conf);
    // Set cluster ID.
    cl_conf.id = 0;                
    // Configure & open cluster. 
    pi_open_from_conf(&cluster_dev, &cl_conf);

    pi_perf_conf(1 << PI_PERF_CYCLES | 1 << PI_PERF_ACTIVE_CYCLES);
    pi_perf_reset();
    pi_perf_start();

    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    pi_perf_stop();
    
    /*----------------------------------------------------------------------
    Measure performance: performance counters for cycle level performance.
    -----------------------------------------------------------------------*/
    uint32_t cycles = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    uint32_t tim_cycles = pi_perf_read(PI_PERF_CYCLES);
    printf("Perf : %d cycles Timer : %d cycles\n", cycles, tim_cycles);

    /*--------------------------------------------------------------------
    Cluster task. 
    ---------------------------------------------------------------------*/
    uint32_t cpy_size = MATRIX_SIZE * MATRIX_SIZE * sizeof(unsigned short);

    // Create and initialize two square matrices in L2 memory.
    unsigned short *matrix1 = (unsigned short *)pi_l2_malloc(cpy_size);
    if (matrix1 == NULL) {
        printf("Error: Failed to allocate L2 memory for matrix1\n");
        return;
    }

    unsigned short *matrix2 = (unsigned short *)pi_l2_malloc(cpy_size);
    if (matrix2 == NULL) {
        printf("Error: Failed to allocate L2 memory for matrix2\n");
        // Free the previously allocated L2 memory.
	pi_l2_free(matrix1, cpy_size); 
        return;
    }

    // Initialize matrix1 and matrix2 with some data (e.g., patterns).
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix1[i * MATRIX_SIZE + j] = i * MATRIX_SIZE + j;
            matrix2[i * MATRIX_SIZE + j] = (i * MATRIX_SIZE + j) * 2; // Sample data for matrix2
        }
    }

    // Copy matrix1 from L2 to L1 memory.
    unsigned short *l1_matrix1 = (unsigned short *)pi_cl_l1_malloc(&cluster_dev, cpy_size);
    if (l1_matrix1 == NULL) {
        printf("Error: Failed to allocate L1 memory for matrix1\n");
        pi_l2_free(matrix1, cpy_size); // Free the previously allocated L2 memory.
        pi_l2_free(matrix2, cpy_size); // Free the previously allocated L2 memory.
        return;
    }
    copy_matrix(matrix1, l1_matrix1, MATRIX_SIZE);

    // Copy matrix2 from L2 to L1 memory.
    unsigned short *l1_matrix2 = (unsigned short *)pi_cl_l1_malloc(&cluster_dev, cpy_size);
    if (l1_matrix2 == NULL) {
        printf("Error: Failed to allocate L1 memory for matrix2\n");
        pi_l2_free(matrix1, cpy_size); // Free the previously allocated L2 memory.
        pi_l2_free(matrix2, cpy_size); // Free the previously allocated L2 memory.
        pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size); // Free the previously allocated L1 memory.
        return;
    }
    copy_matrix(matrix2, l1_matrix2, MATRIX_SIZE);

    // Print the copied matricies.
    printf("Copied Matrix (matrix1) in L1 memory:\n");
    print_mat_elements(l1_matrix1, MATRIX_SIZE);

    printf("\nCopied Matrix (matrix2) in L1 memory:\n"); 
    print_mat_elements(l1_matrix2, MATRIX_SIZE);
   
    // Check the copied matrix from L1 memory to L2 memory is indentical to the matrix in L1.
    int is_identical = 0;
    is_identical = is_matrix_identical(l1_matrix1, matrix1, MATRIX_SIZE);
    if (is_identical) {
        printf("The copied matrix (matrix1)  form L2 to L1 and the matrix1 in L2 are identical.\n");
    } else {
        printf("The copied matrix (matrix1) from L2 to L1 and the matrix1 in L2 are not identical.\n");
    }
    
    is_identical = is_matrix_identical(l1_matrix2, matrix2, MATRIX_SIZE);
    if (is_identical) {
        printf("The copied matrix2 (matrix2) from L2 to L1 and the matrix2 in L2 are identical.\n");
    } else {
        printf("The copied matrix2 (matrix2) from L2 to L1 and the matrix2 in L2 are not identical.\n");
    }     

    // Free allocated L2 memory for next usage.
    pi_l2_free(matrix1, cpy_size);
    pi_l2_free(matrix2, cpy_size);

    // Addition of the two matrices.
    // L1 memory allocation for addition result. 
    unsigned short *l1_add_res = (unsigned short *)pi_cl_l1_malloc(&cluster_dev, cpy_size);
    if (l1_add_res == NULL) {
        printf("Error: Failed to allocate L1 memory for the addition result matrix. \n");
	pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size); // Free the previously allocated L1 memory.
    	pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size); // Free the previously allocated L1 memory.
        return;
    }
    matrix_addition(l1_matrix1, l1_matrix2, l1_add_res, MATRIX_SIZE);

    // Multiplication of the two matrices.
    // L1 memory allocation for multiplication.
    unsigned short *l1_mul_res = (unsigned short *)pi_cl_l1_malloc(&cluster_dev, cpy_size);
    if (l1_mul_res == NULL) {
        printf("Error: Failed to allocate L1 memory for the multiplication result matrix. \n");
        pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size); // Free the previously allocated L1 memory.
        pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size); // Free the previously allocated L1 memory.	
        pi_cl_l1_free(&cluster_dev, l1_add_res, cpy_size); // Free the previously allocated L1 memory.
        return;
    }
    matrix_multiplication(l1_matrix1, l1_matrix2, l1_mul_res, MATRIX_SIZE);
    
    // Copy the results matrices from L1 memory and L2 memory.
    // Allocation of L2 memory for the result matrices:
    unsigned short *l2_add_res = (unsigned short *)pi_l2_malloc(cpy_size);  
    if (l2_add_res == NULL) {
        printf("Error: Failed to allocate L2 memory for the addition result matrices.\n");
	// Free the previous allocated memory.
    	pi_cl_l1_free(&cluster_dev, l1_add_res, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_mul_res, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size);
        return;
    }
    copy_matrix(l1_add_res, l2_add_res,MATRIX_SIZE);
    
    unsigned short *l2_mul_res = (unsigned short *)pi_l2_malloc(cpy_size);
     if (l2_mul_res == NULL) {
        printf("Error: Failed to allocate L2 memory for the multiplication result matrices.\n");
	// Free the previous allocated memory.
    	pi_cl_l1_free(&cluster_dev, l1_add_res, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_mul_res, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size);
    	pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size);
    	pi_l2_free(l2_add_res, cpy_size);
        return;
    }
    copy_matrix(l1_mul_res, l2_mul_res,MATRIX_SIZE);

    // Print the copied result matrices. 
    printf("\n The addition result matrix is: \n");
    print_mat_elements(l2_add_res,MATRIX_SIZE);	
    printf("\n The multiplication result matrix is: \n");
    print_mat_elements(l2_mul_res,MATRIX_SIZE);	
    
    
    // Test the copied matrix in L2 is indentical to the matrix in L1
    is_identical = is_matrix_identical(l1_add_res, l2_add_res, MATRIX_SIZE);
    if (is_identical) {
        printf("The addition result matrix in L1 is identical to the copied matrix in L2.\n");
    } else {
        printf("The addition result  matrix in L1 is not identical to the copied matrix in L2.\n");
    }

    // Test the copied matrix in L2 is indentical to the matrix in L1
    is_identical = is_matrix_identical(l1_mul_res, l2_mul_res, MATRIX_SIZE);
    if (is_identical) {
        printf("The multiplication result matrix in L1 is identical to the copied matrix in L2.\n");
    } else {
        printf("The multiplication result  matrix in L1 is not identical to the copied matrix in L2.\n");
    }

    // L1 memory allocation for convolution output.
    unsigned short *l1_convol_output = (unsigned short *)pi_cl_l1_malloc(&cluster_dev, cpy_size);
    if (l1_convol_output == NULL) {
        printf("Error: Failed to allocate L1 memory for the multiplication result matrix. \n");
        pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size); // Free the previously allocated L1 memory.
        pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size); // Free the previously allocated L1 memory.	
        pi_cl_l1_free(&cluster_dev, l1_add_res, cpy_size); // Free the previously allocated L1 memory.
        pi_cl_l1_free(&cluster_dev, l1_mul_res, cpy_size); // Free the previously allocated L1 memory.
        return;
    }

    // filter 
    unsigned short filter[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    convolution(l1_mul_res, l1_convol_output, filter, MATRIX_SIZE);
    printf("\n convolution output: \n");
    print_mat_elements(l1_convol_output, MATRIX_SIZE);

    // Free allocated memory.
    pi_cl_l1_free(&cluster_dev, l1_convol_output, cpy_size);
    pi_cl_l1_free(&cluster_dev, l1_add_res, cpy_size);
    pi_cl_l1_free(&cluster_dev, l1_mul_res, cpy_size);
    pi_cl_l1_free(&cluster_dev, l1_matrix1, cpy_size);
    pi_cl_l1_free(&cluster_dev, l1_matrix2, cpy_size);
    // Free allocated L2 memory for next usage.
    pi_l2_free(l2_add_res, cpy_size);
    pi_l2_free(l2_mul_res, cpy_size);

    pi_cluster_close(&cluster_dev);

    printf("Test success !\n");

    pmsis_exit(errors);
}

/*===================================================
 main function.
 ===================================================*/

int main() {
    printf("\n\n\t *** Matrix Copy on GAP8 GVSoC ***\n\n");
    return pmsis_kickoff((void *) taskExecution);
}

