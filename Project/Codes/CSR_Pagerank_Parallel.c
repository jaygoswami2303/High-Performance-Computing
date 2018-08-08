/*
Author:
Vishal Shingala	(201501450)
Jay Goswami	(201501037)

Parallel Implementetion of PageRank Algorithm
*/
#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


int main(int argc, char *argv[]){
   

  // Keep track of the execution time
  double pwtime;

	
  int numthreads = atoi(argv[1]);
	//printf("Threads : %d\n",numthreads);
  

  // Open the data set 
  char filename[]="./test.txt";		//all database are from http://snap.stanford.edu/data/index.html
  FILE *fp;
  if((fp=fopen(filename,"r"))==NULL) {
    fprintf(stderr,"[Error] cannot open file");
    exit(1);
  }
  
  // Read the data set and get the number of nodes (n) and edges (e)
  int n, e;
  char ch;
  char str[100];
  ch = getc(fp);
  while(ch == '#') {
    fgets(str, 100-1, fp);
    
    sscanf (str,"%*s %d %*s %d", &n, &e); //number of nodes and edges
    ch = getc(fp);
  }
  ungetc(ch,fp);
  
 // printf("\nGraph data:\n\n  Nodes: %d, Edges: %d \n\n", n, e);  
  
  /************************* CSR STRUCTURES *****************************/
    
  /* Compressed sparse row format: 
     - Val vector: contains 1.0 if an edge exists in a certain row
     - Col_ind vector: contains the column index of the corresponding value in 'val'
     - Row_ptr vector: points to the start of each row in 'col_ind'
  */

  float *val = calloc(e, sizeof(float));
  int *col_ind = calloc(e, sizeof(int));
  int *row_ptr = calloc(n+1, sizeof(int));
 
  // The first row always starts at position 0 with 0
  row_ptr[0] = 0;

  int fromnode, tonode;
  int cur_row = 0;
  int i = 0;
  int j = 0;
  // Elements for row
  int elrow = 0;
  // Cumulative numbers of elements
  int curel = 0;
  
  while(!feof(fp)){
    
    fscanf(fp,"%d%d",&fromnode,&tonode);
    
    
    if (fromnode > cur_row) { // change the row
      curel = curel + elrow;
      int k;
      for ( k = cur_row + 1; k <= fromnode; k++) {
        row_ptr[k] = curel;
      }
      elrow = 0;
      cur_row = fromnode;
    }
    val[i] = 1.0;
    col_ind[i] = tonode;
    elrow++;
    i++;
  }
  row_ptr[cur_row+1] = curel + elrow - 1;

  /*Print vectors involved so far
  printf("\nVal vector:\n  [ ");
  for (i=0; i<e; i++){
        printf("%f ", val[i]);
      }
  printf(" ]\n");
  printf("\nCol_ind vector :\n  [ ");
  for (i=0; i<e; i++){
        printf("%d ", col_ind[i]);
      }
  printf(" ]\n");
  int size = sizeof(row_ptr) / sizeof(int);
  printf("\nrow_ptr vector (size = %d):\n  [ ", size);
  for (i=0; i< n; i++){
        printf("%d ", row_ptr[i]);
      }
  printf(" ]\n");*/


  // Fix the stochastization
  int out_link[n];
  for(i=0; i<n; i++) {
    out_link[i] = 0;
  }


  int rowel = 0;
  for(i=0; i<n; i++){
        if (row_ptr[i+1] != 0) {
          rowel = row_ptr[i+1] - row_ptr[i];
          out_link[i] = rowel;
        }
   }

  int curcol = 0;
  for(i=0; i<n; i++) {
    rowel = row_ptr[i+1] - row_ptr[i];
    for (j=0; j<rowel; j++) {
      val[curcol] = val[curcol] / out_link[i];
      curcol++;
    }
  }

  //Find transpose of the matrix
  
  //Initialization of transposed matrix
	float *val1 = calloc(e, sizeof(float));
  int *col_ind1 = calloc(e, sizeof(int));
  int *row_ptr1 = calloc(n+1, sizeof(int));

  //Structure to find transpose using bucket sort
	struct Node {
      float val;
      int col;
      struct Node *next;
  };

	struct Node** arr = (struct Node**)(calloc(n, sizeof(struct Node*))) ;
	struct Node** tail = (struct Node**)(calloc(n, sizeof(struct Node*))) ;

  for(i=0;i<n;i++) {
    arr[i] = tail[i] = NULL;
  }

  //Transform the CSR structure into a row linked list structure with transposed value
  row_ptr1[0] = 0;
  i = 0;
  for(curcol = 0;curcol<e;curcol++) {
    while(curcol>=row_ptr[i+1]) {
        i++;
    }
    int rw = col_ind[curcol];
    int cl = i;
    if(arr[rw]==NULL) {
            struct Node *x = malloc(sizeof(struct Node));
            x->col = cl;
            x->val = val[curcol];
            x->next = NULL;
        arr[rw] = x;
        tail[rw] = x;
    }
    else {
            struct Node *x = malloc(sizeof(struct Node));
            x->col = cl;
            x->val = val[curcol];
            x->next = NULL;
        tail[rw]->next = x;
        tail[rw] = x;
    }
  }

  //Convert transposed matrix to CSR structure
  curcol = 0;
  for(i=0;i<n;i++) {
        int prev = curcol;
    struct Node *temp = arr[i];
    while(temp!=NULL) {
        val1[curcol] = temp->val;
        col_ind1[curcol] = temp->col;
        curcol++;
        temp = temp->next;
    }
    row_ptr1[i+1] = row_ptr1[i] + curcol - prev;
  }

  //Store the transposed CSR matrix into the original CSR matrix
	val = val1;
	col_ind = col_ind1;
	row_ptr = row_ptr1;


  // Set the damping factor 'd'
  float d = 0.50;
  
  // Initialize p[] vector
  float p[n];
  for(i=0; i<n; i++){
    p[i] = 1.0/n;
  }
  
  // Set the looping condition and the number of iterations 'k'
  int looping = 1;
  int k = 0;

  // Set 'parallel' depending on the number of threads
  int parallel = 0;
  if (numthreads >= 2) {
          parallel = 1;
  }

  // Initialize new p vector
  float p_new[n];
  
  pwtime = omp_get_wtime();
  while (looping){

    // Initialize p_new as a vector of n 0.0 cells
    for(i=0; i<n; i++){
      p_new[i] = 0.0;
    }
    
    int rowel = 0;
    int curcol = 0;
    
    // Page rank modified algorithm + parallelization
  #pragma omp parallel for private(j) schedule(static) if(parallel) num_threads(numthreads)
      for(i=0; i<n; i++){
  			float pi = 0.0;
				int rp1 = row_ptr[i+1];
      	for (j=row_ptr[i]; j<rp1; j++) {
   	     pi = pi + val[j]*p[col_ind[j]];
				}
				p_new[i] += pi;
    	}	

    // Adjustment to manage dangling elements 
    for(i=0; i<n; i++){
      p_new[i] = d * p_new[i] + ((1.0 - d) / n);
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
  //****************RESULTS***************
  
  // Stop the timer and compute the time spent
  pwtime = omp_get_wtime() - pwtime;
  
  // Print results
  printf ("\nNumber of iteration to converge: %d \n\n", k); 
  printf ("Final Pagerank values:\n\n[");
  for (i=0; i<n; i++){
    printf("%f ", n*p[i]);
    if(i!=(n-1)){ printf(", "); }
  }
  printf("]\n\nTime spent: %f seconds.\n", pwtime);
 
  //for graph plot only below print line is used
  //   printf("%d,%d,0,%f,%f\n",e,numthreads,pwtime,pwtime);  
return 0;
}
