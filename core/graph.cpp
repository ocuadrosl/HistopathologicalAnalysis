#include "graph.h"

Graph::Graph()
{

}


void Graph::SetFeaturesVector(const featuresVectorT &featuresVector)
{
    this->featuresVector = featuresVector;
}


void Graph::SetSuperPixelsLabels(const grayImageP& superPixelsLabels)
{
    this->superPixelsLabels = superPixelsLabels;
}

void Graph::findSPNeighbors(std::vector<unsigned>& labels, unsigned radius)
{



}


void Graph::Build()
{





    IO::printOK("Building graph");


}
