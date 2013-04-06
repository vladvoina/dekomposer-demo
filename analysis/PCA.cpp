#include "PCA.h"


PCA::PCA()
{
  mean = 0;
  standard_deviation = 0;
  covariance_progres = 0;
}

void PCA::process(MatrixXf* input)
{
  // very inefficient atm
  mean = input->mean();
  zero_mean_data = input->array() - mean;  // <OPTIM> because memory is allocated every time (?) this assignment might be very computationally expensive
  standard_deviation = sqrt(zero_mean_data.squaredNorm()/zero_mean_data.size());
  //cout << data << endl;
}
float PCA::getMean()	             { return mean; }
float PCA::getStandardDeviation() { return standard_deviation; }

// it is assumeed that the passed in vectors have the same length
// can be performed as a vector by vector multiplication
float PCA::getCovariance(Matrix<float, 1, Dynamic>* X, Matrix<float, 1, Dynamic>* Y)
{
	//covariance_progres++;
	//cout << " " << covariance_progres;
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

void PCA::computeMeanVector(MatrixXf* data)
{
	data_mean = VectorXf(data->rows());
	for (int i=0; i<data->rows(); i++)
	{
	 data_mean(i) = data->row(i).mean();
	}
}
// much butter optimized than the previous one
void PCA::computeCovarianceMatrix2(MatrixXf* data_, bool bijection)
{
	computeMeanVector(data_);
	covariance_matrix = MatrixXf(data_->rows(), data_->rows()); 
	zero_mean_data = MatrixXf(data_->rows(), data_->cols());
	for (int i=0; i<data_->rows(); i++)
	{
	 zero_mean_data.row(i) = data_->row(i).array() - data_mean(i);
	}
	covariance_matrix = (zero_mean_data * zero_mean_data.transpose()); //.array() / data_->cols();
}

void PCA::computeCovarianceMatrix2T (MatrixXf* data_)
{
	computeMeanVector(data_);
	covariance_matrix = MatrixXf(data_->cols(), data_->cols());
	zero_mean_data = MatrixXf(data_->rows(), data_->cols());
	for (int i=0; i<data_->rows(); i++)
	{
	 zero_mean_data.row(i) = data_->row(i).array() - data_mean(i);
	}
	 covariance_matrix = (zero_mean_data.transpose() * zero_mean_data);//.array() / data_->cols();
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
	//eigen_values = es.eigenvalues().reverse();
	
	// can be reversed via Eigen API
	if (components == 0)
	{
	// reverse feature_vector so that the vector with the corresponding highest eigenvalue is on top
      eigen_values = es.eigenvalues().reverse();
	  feature_vector = MatrixXf(covariance_matrix.rows(), covariance_matrix.rows());
      for (int i=0; i<covariance_matrix.rows(); i++)
	  {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	  }
	} else
	if (components < 0)
	{
		// <!> might be problematic. Head() is a vector methdod and eigen_values is a matrix
	   eigen_values = es.eigenvalues().reverse().head(es.eigenvalues().cols()+components);
	   feature_vector = MatrixXf(covariance_matrix.rows(), covariance_matrix.rows()+components);
	   for (int i=0; i<covariance_matrix.rows()+components; i++)
	   {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	   }

	} else
	if (components > 0)
	{
	   eigen_values = es.eigenvalues().reverse().head(components);  
	   feature_vector = MatrixXf(covariance_matrix.rows(), components);
	   for (int i=0; i<components; i++)
	   {
		 feature_vector.col(i) = es.eigenvectors().col(covariance_matrix.rows()-1 - i);
	   }   
	}

	//cout << "Eigenvalues are: " << endl << eigen_values << endl;
	//cout << "Eigenvectors are: " << endl << feature_vector << endl;
}

void PCA::computeFeatureVectorT(int components)
{
	//<OPTIM> no need to create a new feature_vector in the header, you can just use it temporarly
	computeFeatureVector(components);
	//<OPTIM> maybe it can be turned into a single matrix multiplication
	feature_vector_t = MatrixXf (zero_mean_data.rows(), feature_vector.cols());
	for(int i=0; i<feature_vector.cols();i++)
	{
	feature_vector_t.col(i) = zero_mean_data * feature_vector.col(i);
	feature_vector_t.col(i).normalize();
	}
	//cout << "Eigenvectors are: " << endl << feature_vector_t << endl;
	//cout << "Eigenvalues are: " << endl << eigen_values << endl;
	//cout << "Eigenvectors are: " << endl << feature_vector << endl;
}

// pass in the number of principal components to drop
void PCA::computeFeatureVectorRedux(int components)
{
	computeFeatureVector();
	// <OPTIM> bad memory management
	MatrixXf temp; VectorXf temp2;
	temp = feature_vector.block(0, components, feature_vector.rows(), feature_vector.cols()-components);
	temp2 = eigen_values.bottomRows(eigen_values.rows()-components); //tail(eigen_values.size()-components);
	feature_vector = temp; eigen_values = temp2; //feature_vector.rightCols(feature_vector.cols()-components);
	//cout << "REDUX >>" << endl << "Eigenvectors are: " << endl << feature_vector << endl;
}

void PCA::computeFeatureVectorReduxT(int components)
{
	computeFeatureVectorT();
	MatrixXf temp; VectorXf temp2;
	temp = feature_vector_t.block(0, components, feature_vector_t.rows(), feature_vector_t.cols()-components);
	temp2 = eigen_values.bottomRows(eigen_values.rows()-components);
	feature_vector_t = temp; eigen_values = temp2;
}


MatrixXf* PCA::getFeatureVector() { return &feature_vector; }


void PCA::transformData(Matrix<float, Dynamic, Dynamic>* data)
{
	// <OPTIM> zero mean data has been previously computed, store that and use, instead of computing again
	MatrixXf zero_mean_data_(data->rows(), data->cols());
	// Compute means and store them to be added at a later stage
	data_mean = VectorXf(data->rows()); 
	for (int i=0; i<data->rows(); i++)
	{
		data_mean(i) = data->row(i).mean();
		zero_mean_data_.row(i) = data->row(i).array() - data_mean(i);
	}

    transformed_data = feature_vector.transpose() * zero_mean_data_;
}

void PCA::transformData2()
{
    transformed_data = feature_vector.transpose() * zero_mean_data;
}

void PCA::transformData2T()
{
	transformed_data = feature_vector_t.transpose() * zero_mean_data;
}

MatrixXf* PCA::getTransformedData() { return &transformed_data; }

void PCA::reexpressData()
{
	reexpressed_data = feature_vector * transformed_data;
	// add original mean
	for(int i=0; i<reexpressed_data.rows(); i++)
	{
	reexpressed_data.row(i).array() += data_mean(i);
	}	
}

MatrixXf* PCA::getReexpressedData() { return &reexpressed_data; }

void PCA::projectData(MatrixXf* data)
{
	projected_data = feature_vector_t.transpose() * ((*data) - data_mean);
}

void PCA::distanceToSpace(MatrixXf* data)
{
	float distance = 0;
	projectData(data);
    distance = ((feature_vector_t * projected_data) - ((*data) - data_mean)).norm();
	cout << "Distance to space is: " << distance << endl;
}

void PCA::computeDistances()
{
	float distance = 0;
	for (int i=0; i<transformed_data.cols(); i++)
	{
		distance = (float) (projected_data - transformed_data.col(i)).norm();
		cout << "Distance to sample " << i+1 << "is: " << distance << endl;
	}
}




