#ifndef FINDPEAKS_H
#define FINDPEAKS_H

#include <vector>
#include <algorithm>
#include "../util/math.h"



#define EPS 2.2204e-16

template<typename type>
class FindPeaks
{

    using vectorT = std::vector<type>;
    using vectorU = std::vector<unsigned>;

public:
    FindPeaks();
    void setData(const vectorT& data);
    void find();
    vectorT getPeaks();


private:
    vectorU peaksIndices;
    vectorT inputData;
    vectorT peaks;

    vectorU findIndicesLessThan(vectorT diffs, type threshold=0);

    void singVector(const vectorT& input, std::vector<int>& output) const;



    template<typename t>
    std::vector<t> selectElements(const std::vector<t>& input, const vectorU& indices) const;




};

template class FindPeaks<unsigned>;
template class FindPeaks<int>;
template class FindPeaks<long int>;
template class FindPeaks<float>;
template class FindPeaks<double>;
template class FindPeaks<unsigned char>;



#endif // FINDPEAKS_H
