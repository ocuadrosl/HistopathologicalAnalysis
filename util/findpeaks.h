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
    using vectorF = std::vector<float>;

public:
    FindPeaks();
    void setData(const vectorT& data);
    void find();
    vectorT getPeaks();


private:
    vectorU peaksIndices;
    vectorT inputData;
    vectorT peaks;

    void findIndicesLessThan(const vectorF& data,  vectorU& output);

    void singVector(const vectorF& input, std::vector<int>& output) const;



    template<typename t>
    void selectElements(const std::vector<t>& input, const vectorU& indices, std::vector<t>& output) const;




};

template class FindPeaks<unsigned>;
template class FindPeaks<int>;
template class FindPeaks<long int>;
template class FindPeaks<float>;
template class FindPeaks<double>;
template class FindPeaks<unsigned char>;



#endif // FINDPEAKS_H
