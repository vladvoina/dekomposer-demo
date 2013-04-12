#pragma once

#include "ofMain.h"
#include <Eigen/Dense>
#include <iostream>

using namespace Eigen;

class Utils {

public:

	/*
	static void eigenToArray(Matrix <double, Dynamic, Dynamic, RowMajor>* data, double** &data_pointer)
	{
	  **data_pointer = (double**) malloc(data->rows()*sizeof(double*));
	  for (int i=0; i<data->rows(); i++)
	  {
	   data_pointer[i] = &((*data)(i, 0));
	  }
	}
	*/

	static double ofMax(double * ar, int ar_length)
	{
	  double temp = ar[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (ar[i] > temp)
		{
			temp = ar[i];
		}
	  }
	  return temp;
	}

	static float ofMax(float * ar, int ar_length)
	{
	  float temp = ar[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (ar[i] > temp)
		{
			temp = ar[i];
		}
	  }
	  return temp;
	}

	static int ofMax(int * ar, int ar_length)
	{
	  int temp = ar[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (ar[i] > temp)
		{
			temp = ar[i];
		}
	  }
	  return temp;
	}

	static int ofMax(vector <int>  vec)
	{
	  int temp = vec[0];
	  for (int i=1; i<vec.size(); i++)
	  {
		if (vec[i] > temp)
		{
			temp = vec[i];
		}
	  }
	  return temp;
	}

	static short ofMax(short * vec, int ar_length)
	{
	  int temp = vec[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (vec[i] > temp)
		{
			temp = vec[i];
		}
	  }
	  return temp;
	}

	static short ofMin(short * vec, int ar_length)
	{
	  short temp = vec[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (vec[i] < temp)
		{
			temp = vec[i];
		}
	  }
	  return temp;
	}

	static float ofMin(float * vec, int ar_length)
	{
	  float temp = vec[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (vec[i] < temp)
		{
			temp = vec[i];
		}
	  }
	  return temp;
	}

	static double ofMin(double * vec, int ar_length)
	{
	  double temp = vec[0];
	  for (int i=1; i<ar_length; i++)
	  {
		if (vec[i] < temp)
		{
			temp = vec[i];
		}
	  }
	  return temp;
	}



	static float ofAverage(int * a, int length)
    {
	int sum = 0;
	float avg = 0.0;

	for (int i=0; i<length; i++)
	{
		sum += a[i];
	}

	avg = (float) sum / length;

	return avg;
    }

	
};