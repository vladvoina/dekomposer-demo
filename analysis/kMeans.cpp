#include "kMeans.h"

//passed in data must be organized into rows-dimensions, cols-observations and MUST be a matrix in RowMajor
void kMeans::cluster(Matrix<float, Dynamic, Dynamic, RowMajor>* dataa, int clusters)
{
  // ** create new matrix of transposed, cast input data ** //
  temp_data = new Matrix<double, Dynamic, Dynamic, RowMajor>;
  (*temp_data) = dataa->transpose().cast<double>(); 
  const int nrows = temp_data->rows();
  const int ncols = temp_data->cols();
  // ** create pointer to the data within the matrix ** //
  double** data_pointer;
  data_pointer = (double**) malloc(temp_data->rows()*sizeof(double*));
  for (int i=0; i<temp_data->rows(); i++) data_pointer[i] = &(*temp_data)(i, 0);

  // ** create mask ** //
  int** mask = (int**) malloc(nrows * sizeof(int*));
  for(int i=0; i<nrows; i++) mask[i] = (int*) malloc(ncols*sizeof(int));
  for(int i=0; i<nrows; i++)
	  for(int j=0; j<ncols;j++)
		  mask[i][j] = 1;
  
  // ** perform clustering ** //
  kmeans(nrows, ncols, data_pointer, mask, clusters);

  // ** free memory ** //
  temp_data->resize(0,0);
  free(temp_data);
}

int* kMeans::getClusterIDs() {return clusterid; }

void kMeans::kmeans(int nrows, int ncols, double** data, int** mask, int clusters)
{
  int i, j;
  const int nclusters = clusters;
  const int transpose = 0;
  const char dist = 'e'; // euclidean distance
  const char method = 'a';
  int npass = 1;
  int ifound = 0;
  double error;
  double distance;
  int** index;
  int* count;
  double* weight = (double*) malloc(ncols*sizeof(double));
  clusterid = (int*) malloc(nrows*sizeof(int));
  double** cdata = (double**) malloc(nclusters*sizeof(double*));
  int** cmask = (int**) malloc(nclusters*sizeof(int*));
  for (i = 0; i < nclusters; i++)
  { cdata[i] = (double*) malloc(ncols*sizeof(double));
    cmask[i] = (int*) malloc(ncols*sizeof(int));
  }
  for (i = 0; i < ncols; i++) weight[i] = 1.0;
  printf("======================== k-means clustering ====================\n");
  printf("\n");
  printf("----- one pass of the EM algorithm (results may change)\n");
  kcluster(nclusters,nrows,ncols,data,mask,weight,transpose,npass,method,dist, 
    clusterid, &error, &ifound);
  printf ("Solution found %d times; within-cluster sum of distances is %f\n",
    ifound, error);
  printf ("Cluster assignments:\n");
  for (i = 0; i < nrows; i++)
    printf ("Gene %d: cluster %d\n", i, clusterid[i]);

  printf ("\n");
  printf("----- 1000 passes of the EM algorithm (result should not change)\n");
  npass = 1000;
  kcluster(nclusters,nrows,ncols,data,mask,weight,transpose,npass,method,dist, 
    clusterid, &error, &ifound);
  printf ("Solution found %d times; ", ifound);
  printf ("within-cluster sum of distances is %f\n", error);
  printf ("Cluster assignments:\n");
  for (i = 0; i < nrows; i++)
    printf ("Gene %d: cluster %d\n", i, clusterid[i]);
  printf ("\n");
  printf ("------- Distance between clusters:\n");
  index = (int**) malloc(nclusters*sizeof(int*));
  count = (int*) malloc(nclusters*sizeof(int));
  for (i = 0; i < nclusters; i++) count[i] = 0;
  for (i = 0; i < nrows; i++) count[clusterid[i]]++;
  for (i = 0; i < nclusters; i++) index[i] = (int*) malloc(count[i]*sizeof(int));
  for (i = 0; i < nclusters; i++) count[i] = 0;
  for (i = 0; i < nrows; i++)
  { int id = clusterid[i];
    index[id][count[id]] = i;
    count[id]++;
  }  
  distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[0], count[1],
		    index[0], index[1], 'e', 'a', 0); 
  printf("Distance between 0 and 1: %7.3f\n", distance);
  distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[0], count[2],
		    index[0], index[2], 'e', 'a', 0); 
  printf("Distance between 0 and 2: %7.3f\n", distance);
  distance =
    clusterdistance(nrows, ncols, data, mask, weight, count[1], count[2],
		    index[1], index[2], 'e', 'a', 0); 
  printf("Distance between 1 and 2: %7.3f\n", distance);

  printf ("\n");
  printf ("------- Cluster centroids:\n");
  getclustercentroids(nclusters, nrows, ncols, data, mask, clusterid,
                      cdata, cmask, 0, 'a');
  printf("   Microarray:");
  for(i=0; i<ncols; i++) printf("\t%7d", i);
  printf("\n");
  for (i = 0; i < nclusters; i++)
  { printf("Cluster %2d:", i);
    for (j = 0; j < ncols; j++) printf("\t%7.3f", cdata[i][j]);
    printf("\n");
  }
  printf("\n");
  for (i = 0; i < nclusters; i++) free(index[i]);
  free(index);
  free(count);

  for (i = 0; i < nclusters; i++)
  { free(cdata[i]);
    free(cmask[i]);
  }
  free(cdata);
  free(cmask);
  //free(clusterid);
  free(weight);
  return;
}