#include "PCA.h"


PCA::PCA()
{
  mean = 0;
  standard_deviation = 0;
}

void PCA::process(MatrixXf* input)
{
  // very inefficient atm
  mean = input->mean();
  data = input->array() - mean;  // <OPTIM> because memory is allocated every time (?) this assignment might be very computationally expensive
  standard_deviation = sqrt(data.squaredNorm()/data.size());
  //cout << data << endl;
}
float PCA::getMean()	             { return mean; }
float PCA::getStandardDeviation() { return standard_deviation; }

// it is assumeed that the passed in vectors have the same length
float PCA::getCovariance(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y)
{
	float x_mean = X->mean();
	float y_mean = Y->mean();
	float covariance = 0;

	for (int i=0; i<X->size(); i++)
	{
	covariance += ((*X)(i)-x_mean)*((*Y)(i)-y_mean);
	}

	covariance /= X->size() - 1;
	
	return covariance;
}

// <OPTIM> only calculate the lower triangular side
// <OPTIM> only calculate the mean once for each dimension
float PCA::getCorrelation(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y)
{
	float x_mean = X->mean();
	float y_mean = Y->mean();
	float zero_mean_x = 0;
	float zero_mean_y = 0;
	float correlation = 0;
	float std_dev_x = 0;
	float std_dev_y = 0;

	for (int i=0; i<X->size(); i++)
	{
    zero_mean_x = (*X)(i)-x_mean;
	zero_mean_y = (*Y)(i)-y_mean;
	std_dev_x += zero_mean_x*zero_mean_x;
	std_dev_y += zero_mean_y*zero_mean_y;
	correlation += zero_mean_x*zero_mean_y;
	}

	std_dev_x = sqrt(std_dev_x/X->size());
	std_dev_y = sqrt(std_dev_y/X->size());
	correlation = correlation/(X->size()*std_dev_x*std_dev_y);
	return correlation;
}

// the data is assumed to be organized into rows->dimensions, columns->observations
// <!> escape duplicate code by adding a boolean to choose between covariance and correlation
void PCA::computeCovarianceMatrix(Matrix<float, Dynamic, Dynamic>* data)
{
	covariance_matrix = MatrixXf(data->rows(), data->rows());
	
	for(int i=0; i<data->rows(); i++)
	{
      for(int j=0; j<data->rows(); j++)
	  {
		 RowVectorXf row1 = data->row(i); /// <OPTIM> allocating an array each calculation can be very expensive  
		 RowVectorXf row2 = data->row(j);
		 covariance_matrix(i,j) = getCovariance(&row1, &row2);
	  }
	}	
}

void PCA::computeCorrelationMatrix(Matrix<float, Dynamic, Dynamic>* data)
{
	covariance_matrix = MatrixXf(data->rows(), data->rows());
	
	for(int i=0; i<data->rows(); i++)
	{
      for(int j=0; j<data->rows(); j++)
	  {
		 RowVectorXf row1 = data->row(i); /// <OPTIM> allocating an array each calculation can be very expensive  
		 RowVectorXf row2 = data->row(j);
		 covariance_matrix(i,j) = getCorrelation(&row1, &row2);
	  }
	}	
}

// error message if not computed
MatrixXf* PCA::getCovarianceCorrelationMatrix() { return &covariance_matrix; }


void PCA::computeFeatureVector(int components)
{
	SelfAdjointEigenSolver<MatrixXf> es(covariance_matrix);
	eigen_values = es.eigenvalues().reverse();

	if (components == 0)
	{
	// reverse feature_vector so that the vector with the corresponding highest eigenvalue is on top
	  feature_vector = MatrixXf(covariance_matrix.rows(), covariance_matrix.rows());
      for (int i=0; i<covariance_matrix.rows(); i++)
	  {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	  }
	} else
	if (components < 0)
	{
	   feature_vector = MatrixXf(covariance_matrix.rows(), covariance_matrix.rows()+components);
	   for (int i=0; i<covariance_matrix.rows()+components; i++)
	   {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	   }

	} else
	if (components > 0)
	{
	   feature_vector = MatrixXf(covariance_matrix.rows(), components);
	   for (int i=0; i<components; i++)
	   {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	   }
	}
	cout << "Eigenvalues are: " << endl << eigen_values << endl;
	cout << "Eigenvectors are: " << endl << feature_vector << endl;
}


void PCA::transformData(Matrix<float, Dynamic, Dynamic>* data)
{
	// <OPTIM> zero mean data has been previously computed, store that and use, instead of computing again
	MatrixXf zero_mean_data(data->rows(), data->cols());
	// Compute means and store them to be added at a later stage
	data_mean = RowVectorXf(data->rows()); 
	for (int i=0; i<data->rows(); i++)
	{
		data_mean(i) = data->row(i).mean();
		zero_mean_data.row(i) = data->row(i).array() - data_mean(i);
	}

    transformed_data = feature_vector.transpose() * zero_mean_data;
}

MatrixXf* PCA::getTransformedData() { return &transformed_data; }

void PCA::reexpressData()
{
	reexpressed_data = feature_vector * transformed_data;
	// <!> add original mean
	for(int i=0; i<reexpressed_data.rows(); i++)
	{
	reexpressed_data.row(i).array() += data_mean(i);
	}	
}

MatrixXf* PCA::getReexpressedData() { return &reexpressed_data; }




