#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

/* Step Two:
   sequential implementation of the PageRank algorithm with
   CSR representation of matrix A
*/

int main(){

  /*************************** TIME, VARIABLES ***************************/

  // Keep track of the execution time
  clock_t begin, end;
  double time_spent;
  begin = clock();

  /******************* OPEN FILE + NUM OF NODES/EDGES ********************/

  // Open the data set
  //char filename[]="./web-NotreDame.txt";
  //char filename[] = "./email-Eu-core.txt";
  char filename[] = "./case1.txt";
  FILE *fp;
  if((fp = fopen(filename,"r")) == NULL)
  {
    fprintf(stderr,"[Error] Cannot open the file");
    exit(1);
  }

  // Read the data set and get the number of nodes (n) and edges (e)
  int n, e;
  char ch;
  char str[100];
  ch = getc(fp);
  while(ch == '#') {
    fgets(str,100-1,fp);
    //Debug: print title of the data set
    //printf("%s",str);
    sscanf (str,"%*s %d %*s %d", &n, &e); //number of nodes
    ch = getc(fp);
  }
  ungetc(ch, fp);

  // DEBUG: Print the number of nodes and edges, skip everything else
  printf("\nGraph data:\n\n  Nodes: %d, Edges: %d \n\n", n, e);

  /************************* CSR STRUCTURES *****************************/

  /* Compressed sparse row format:
     - Val vector: contains 1.0 if an edge exists in a certain row
     - Col_ind vector: contains the column index of the corresponding value in 'val'
     - Row_ptr vector: points to the start of each row in 'col_ind'
  */

  float *val = calloc(e, sizeof(float));
  int *col_ind = calloc(e, sizeof(int));
  int *row_ptr = calloc(n+1, sizeof(int));

  // The first row always starts at position 0
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

    // DEBUG: print fromnode and tonode
    //printf("From: %d To: %d\n",fromnode, tonode);

    if (fromnode > cur_row) { // change the row
      curel = curel + elrow;
      int k;
      for (k = cur_row + 1; k <= fromnode; k++) {
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
  row_ptr[cur_row+1] = curel + elrow;

  /* DEBUG: Print vectors involved so far
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
  for (i=0; i< n+1; i++){
        printf("%d ", row_ptr[i]);
      }
  printf(" ]\n");*/


  // Fix the stochastization
  int out_link[n];
  for(i=0; i<n; i++){
    out_link[i] =0;
  }

  /* DEBUG: row pointer test*/
    printf("\nRow_ptr:\n");
     for (i=0; i<n+1; i++){
          printf("%d ", row_ptr[i]);
        }
    printf("\n");


  int rowel = 0;
  for(i=0; i<n; i++){
        if (row_ptr[i+1] != 0) {
          rowel = row_ptr[i+1] - row_ptr[i];
          out_link[i] = rowel;
        }
   }

  /* DEBUG: Outlink print test*/
  printf("\nOutlink:\n");
   for (i=0; i<n; i++){
        printf("%d ", out_link[i]);
      }
  printf("\n");


  int curcol = 0;
  for(i=0; i<n; i++){
    rowel = row_ptr[i+1] - row_ptr[i];
    for (j=0; j<rowel; j++) {
      val[curcol] = val[curcol] / out_link[i];
      curcol++;
    }
  }
/*printf("CHECK\n");
printf("\n");
for(i=0;i<e;i++)
    printf("%f ",val[i]);
  printf("\n");
  for(i=0;i<e;i++)
    printf("%d ",col_ind[i]);
  printf("\n");
  for(i=0;i<n+1;i++)
    printf("%d ",row_ptr[i]);
  printf("\n");
  printf("\n");
  printf("CHECK\n");*/

  float *val1 = calloc(e, sizeof(float));
  int *col_ind1 = calloc(e, sizeof(int));
  int *row_ptr1 = calloc(n+1, sizeof(int));

  struct Node {
      float val;
      int col;
      struct Node *next;
  };// *arr[n],*tail[n];

  struct Node **arr = (struct Node**)(calloc(n, sizeof(struct Node *)));
  struct Node **tail = (struct Node**)(calloc(n, sizeof(struct Node *)));

  for(i=0;i<n;i++) {
    arr[i] = tail[i] = NULL;
  }

  row_ptr1[0] = 0;
  i = 0;
  printf("START\n");
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
        //printf("%d: %d %d %f\n",curcol,rw,x->col,x->val);
    }
    else {
            struct Node *x = malloc(sizeof(struct Node));
            x->col = cl;
            x->val = val[curcol];
            x->next = NULL;
        tail[rw]->next = x;
        tail[rw] = x;
        //printf("%d: %d %d %f\n",curcol,rw,x->col,x->val);
    }
  //printf("\n");
  }

  printf("DONE\n");

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

  /*printf("CHECK\n");
printf("\n");
  for(i=0;i<e;i++)
    printf("%f ",val1[i]);
  printf("\n");
  for(i=0;i<e;i++)
    printf("%d ",col_ind1[i]);
  printf("\n");
  for(i=0;i<n+1;i++)
    printf("%d ",row_ptr1[i]);
  printf("\n");
  printf("\n");
printf("CHECK\n");*/

/*  curcol = 0;
  for(i=0;i<n;i++) {
        int prev = curcol;
    for(j=0;j<n;j++) {
        int k;
        for(k=row_ptr[j];k<row_ptr[j+1];k++) {
            if(col_ind[k]==i) {
                val1[curcol] = val[k];
                col_ind1[curcol] = j;
                curcol++;
                break;
            }
        }
    }
    row_ptr1[i+1] = row_ptr1[i] + (curcol-prev);
  }

printf("CHECK\n");
printf("\n");
  for(i=0;i<e;i++)
    printf("%f ",val1[i]);
  printf("\n");
  for(i=0;i<e;i++)
    printf("%d ",col_ind1[i]);
  printf("\n");
  for(i=0;i<n+1;i++)
    printf("%d ",row_ptr1[i]);
  printf("\n");
  printf("\n");
printf("CHECK\n");*/

val = val1;
col_ind = col_ind1;
row_ptr = row_ptr1;

  /* DEBUG: val print test*/
  for(i=0; i<e; i++){
      printf("%f ", val[i]);
  }

  /******************* INITIALIZATION OF P, DAMPING FACTOR ************************/

  // Set the damping factor 'd'
  float d = 0.85;

  // Initialize p[] vector
  float p[n];
  for(i=0; i<n; i++){
    p[i] = 1.0/n;
  }

  /*************************** PageRank LOOP  **************************/

  // Set the looping condition and the number of iterations 'k'
  int looping = 1;
  int k = 0;

  // Initialize new p vector
  float p_new[n];

  while (looping){

    // Initialize p_new as a vector of n 0.0 cells
    for(i=0; i<n; i++){
      p_new[i] = 0.0;
    }

    int rowel = 0;
    int curcol = 0;

    // Page rank modified algorithm
    for(i=0; i<n; i++){
      rowel = row_ptr[i+1] - row_ptr[i];
      float pi = 0.0;
      for (j=row_ptr[i]; j<row_ptr[i+1]; j++) {
        //p_new[col_ind[curcol]] = p_new[col_ind[curcol]] + val[curcol] * p[i];
        pi += val[j] * p[col_ind[j]];
        //curcol++;
      }
      p_new[i] += pi;
    }

    /*DEBUG: print pnew
    for (i=0; i<n; i++){
      printf("%f ", p_new[i]);
    }*/

    // Adjustment to manage dangling elements
    for(i=0; i<n; i++){
      p_new[i] = d * p_new[i] + (1.0 - d) / n;
    }

    /*DEBUG: print pnew after the damping factor multiplication
    for (i=0; i<n; i++){
      printf("%f ", p_new[i]);
    }*/

    // TERMINATION: check if we have to stop
    float error = 0.0;
    for(i=0; i<n; i++) {
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

/*************************** CONCLUSIONS *******************************/

  // Stop the timer and compute the time spent
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  // Sleep a bit so stdout is not messed up
  //Sleep(500);

  // Print results
  FILE *fp21;
fp21=fopen("test2.txt", "w");
if(fp21 == NULL)
    exit(-1);
  printf ("\nNumber of iteration to converge: %d \n\n", k);
  printf ("Final Pagerank values:\n\n[");
  for (i=0; i<n; i++){
    printf("%f ", p[i]);
    fprintf(fp21,"%f ", p[i]);
    if(i!=(n-1)){ printf(", "); }
  }
  fclose(fp21);
  printf("]\n\nTime spent: %f seconds.\n", time_spent);
  return 0;
}
