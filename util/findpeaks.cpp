#include "findpeaks.h"

template<typename type>
FindPeaks<type>::FindPeaks()
{

}


template<typename type>
void FindPeaks<type>:: setData(const vectorT& data)
{
    inputData = data;
}

template<typename type>
void FindPeaks<type>::find()
{

    const vectorT &data = inputData;

    int minId = std::distance(data.begin(), std::min_element(data.begin(), data.end()));
    int maxId = std::distance(data.begin(), std::max_element(data.begin(), data.end()));

    type sel = (data[maxId] - data[minId])/4;

    vectorT diffs = Math::diff<type>(data);

    std::replace(diffs.begin(), diffs.end(), 0.0, -EPS);

    vectorT diff_1(diffs.begin()  , diffs.end()-1);
    vectorT diff_2(diffs.begin()+1, diffs.end()  );
    vectorT diff_3 = Math::vectorProduct<type>(diff_1, diff_2);

    std::cout<<"here"<<std::endl;

}
