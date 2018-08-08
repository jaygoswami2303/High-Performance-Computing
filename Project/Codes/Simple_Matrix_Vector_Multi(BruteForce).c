/*
Author:
Vishal Shingala	(201501450)
Jay Goswami		(201501037)

Brute Force serial Implementetion of PageRank Algorithm
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


int main(){

	// Keep track of the execution time
	double swtime;

	// Set the damping factor 'd'
	float d = 0.50;

	// Open the data set
    char filename[] = "./test.txt";
    FILE *fp;
    if((fp = fopen(filename,"r")) == NULL) {
        fprintf(stderr,"[Error] Cannot open the file");
        exit(1);
    }

    // Read the data set and get the number of nodes (n)
    int n;
    char ch;
    char str[100];
    ch = getc(fp);
    while (ch =='#') {
        fgets(str,100-1,fp);
        sscanf (str,"%*s %d %*s %*d", &n); 
        ch = getc(fp);
    }
    ungetc(ch, fp);

    printf("\nNumber of nodes = %d\n",n);

    float **a = malloc(sizeof *a * n);
	int i, j, node1, node2;
	
	// Preallocate the adjacency matrix 'a'    
	for (i = 0; i < n; i++) {
	  	a[i] = malloc(sizeof *a[i] * n);
	}

	// Initialize all the adjacency matrix to 0.0
	for(i = 0; i < n; i++){ 
        for(j = 0; j < n; j++){ 
        	a[i][j] = 0.0;
        }
    }

	// Update the matrix to 1.0 if there's an edge between nodes
	while(!feof(fp)){
	    fscanf(fp,"%d%d", &node1, &node2);
	    //printf("Node 1: %d, Node 2: %d\n", node1, node2);
	    a[node1][node2] = 1.0;
	    //printf("In matrix a[%d][%d]: %f\n", node1, node2, a[node1][node2]);
	}
    

    float **at = malloc(sizeof *at * n);

    // Preallocate space for the transposed matrix 'at'
	for (i = 0; i < n; i++) {
	    at[i] = malloc(sizeof *at[i] * n);
	}

	// Initialize all the transposed matrix to 0.0
	for (i=0; i<n; i++){
		for (j=0; j<n; j++){
			at[i][j] = 0.0;
		}
	}

	float p[n];
	
	// Initialize the p[] vector
	for(i=0; i<n; i++) {
		p[i] = 1.0 / n;
	}

	swtime = omp_get_wtime();
	
	int out_link[n];

	// Initialize the output link vector
	for (i=0; i<n; i++) {
		out_link[i] = 0;
	}

	// Manage dangling nodes   
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			if (a[i][j] != 0.0) {
				out_link[i] = out_link[i] + 1;
			}
		}
	}

	// Print outlink vector
	printf("\nPer-node out link: \n\n     [");
	for (i=0; i<n; i++){
		printf("%d", out_link[i]);
		if(i != (n-1)){
			printf(", "); 
		}
	}
	printf("]\n\n");

	// Make the matrix stochastic
	for (i=0; i<n; i++){
		if (out_link[i] == 0){
			// Deal with dangling nodes
			for (j=0; j<n; j++){
				a[i][j] = 1.0 / n;
			}
		} else {
			for (j=0; j<n; j++){
				if (a[i][j] != 0) {
					a[i][j] = a[i][j] / out_link[i];
				}
			}
		}
	}

	// print the stochastic matrix
/*	printf("\nStochastic matrix: \n");
	for (i=0; i<n; i++){
		for (j=0; j<n; j++){
			printf("%f ", a[i][j]);
		}
		printf("\n");
	}*/

	// Transpose the matrix 
	for (i=0; i<n; i++){
		for (j=0; j<n; j++){
			at[j][i] = a[i][j];
		}
	}

	/* print transposed matrix*/
/*	printf("\nTransposed matrix: \n");
	printf("[");
	for (i=0; i<n; i++){
		printf("[");
		for (j=0; j<n; j++){
			printf("%f", at[i][j]);
			if(j!=(n-1)){ printf(", "); }
		}
		printf("], ");
	}
	printf("]\n");
	*/
	
	// Set the looping condition and the number of iterations 'k'
	int looping = 1;
	int k = 0;

	// Initialize new p vector
	float p_new[n];

	while (looping) {

		// Initialize p_new as a vector of n 0.0 cells
		for (i=0; i<n; i++){
			p_new[i] = 0.0;
		}
		
		// Update p_new (without using the damping factor)
		for (i=0; i<n; i++){
			for (j=0; j<n; j++){
				p_new[i] = p_new[i] + (at[i][j] * p[j]);
			}
		} 

		// Update p_new (using the damping factor)
		for(i=0; i<n; i++){
		 	p_new[i] = d * p_new[i] + (1.0 - d) / n;
		}

		// TERMINATION: check if we have to stop
	    float error = 0.0;
	    for(i=0; i<n; i++){
	        error =  error + fabs(p_new[i] - p[i]);
	    }

	    //if two consecutive instances of pagerank vector are almost identical, stop
	    if (error < 0.000001){
	        looping = 0;
	    }

	    // Update p[]
	    for (i=0; i<n;i++){
	    	p[i] = p_new[i];
		}

		// Increase the number of iterations
	    k = k + 1;
	}

	// Stop the timer and compute the time spent
	
	swtime = omp_get_wtime() - swtime;

		
	// Print results
	printf ("\nNumber of iteration to converge: %d \n\n", k); 
	printf ("Final Pagerank values:\n\n[");
	for (i=0; i<n; i++){
		printf("%f ", n*p[i]);
		if(i!=(n-1)){ printf(", "); }
	}
	printf("]\n\nTime spent: %f seconds.\n", swtime);
	return 0;
}
