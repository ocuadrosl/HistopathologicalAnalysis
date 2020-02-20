#ifndef GRAPH_H
#define GRAPH_H

//local includes
#include "../util/customprint.h"

#include <itkImage.h>
//#include <Eigen/SparseCholesky>
//using Eigen::MatrixXd;


class Graph
{

    //using sparseMatrixT = Eigen::SparseMatrix<float>;

    using featuresVectorT = std::vector<std::vector<float>>;

    using grayImageT = itk::Image<unsigned, 2>;
    using grayImageP =  grayImageT::Pointer;

public:
    Graph();

    void Build();

    void SetSuperPixelsLabels(const grayImageP& superPixelsLabels);
    void SetFeaturesVector(const featuresVectorT &featuresVector);




private:
    //sparseMatrixT adjacencyMatrix;
    featuresVectorT featuresVector;
    grayImageP superPixelsLabels;

    void findSPNeighbors(std::vector<unsigned>& labels, unsigned radius = 1);

};

#endif // GRAPH_H
