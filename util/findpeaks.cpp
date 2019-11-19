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

    vectorT diffs = Math::diff<type>(data);

    std::replace(diffs.begin(), diffs.end(), 0.0, -EPS);

    vectorT diff_1(diffs.begin()  , diffs.end()-1);
    vectorT diff_2(diffs.begin()+1, diffs.end()  );
    vectorT diff_3 = Math::vectorProduct<type>(diff_1, diff_2);

    vectorU indices =  findIndicesLessThan(diff_3); // Find where the derivative changes sign

    vectorU indAux(indices.begin(), indices.end());

    vectorT  x =  selectElements<type>(data, indAux);

    x.insert(x.begin(), data[0]);
    x.insert(x.end(), data[data.size()-1]);

    indices.insert(indices.begin(),0);
    indices.insert(indices.end(), data.size());


    unsigned minMagIndex = std::distance(x.begin(), std::min_element( x.begin(),x.end()));
    type minMag = x[minMagIndex];
    type leftMin = minMag;


    int size = x.size();
    type tempMag;
    if(size > 2)
    {
        tempMag = minMag;

    }



    vectorT subVector( x.begin(), x.begin()+3);
    vectorT diff = Math::diff<type>(subVector);


    std::vector<int> singDiff;

    singVector(diff, singDiff);


    if(singDiff[0] <=0 )
    {
        if( singDiff[0] == singDiff[1])
        {
            x.erase(x.begin()+1);
            indices.erase(indices.begin()+1);
            --size;
        }
    }
    else if ( singDiff[0] == singDiff[1] )
    {
             x.erase(x.begin());
             indices.erase(indices.begin());
             --size;

    }

    type maxPeak = std::ceil( static_cast<type>(size/2.0) );

    std::vector<int> peakLock(maxPeak,0);
    vectorT peakMag( maxPeak, 0);

    unsigned cIndex = 1;
    int tempLoc;
    int ii = ( x[0] >= x[1]) ? 0 : 1;
    bool foundPeak = false;
    type sel = (data[maxId] - data[minId])/4;

    while (ii < size)
    {

        ++ii;
        if(foundPeak)
        {
            tempMag = minMag;
            foundPeak = false;

        }

        if(x[ii-1] > tempMag && x[ii-1] > leftMin + sel)
        {

            tempLoc = ii+1;
            tempMag = x[ii-1];
        }

        if(ii ==size) break;

        ++ii;

        if ( !foundPeak && tempMag > sel + x[ii-1] )
        {
            foundPeak = true;
            leftMin = x[ii-1];
            peakLock[cIndex-1] = tempLoc;
            peakMag[cIndex-1] = tempMag;
            ++cIndex;

        }
        else if (  x[ii-1] < leftMin )
        {
            leftMin = x[ii-1];

        }


    }



    if( *x.rbegin() > tempMag && *x.rbegin() > leftMin +sel)
    {
        peakLock[cIndex -1 ] = size-1;
        peakMag[cIndex-1]  = *x.rbegin();
        ++cIndex;

    }
    else if( !foundPeak && tempMag > minMag)
    {
        peakLock[cIndex-1] = tempLoc;
        peakMag[cIndex-1] = *x.rbegin();
        ++cIndex;

    }


    if(cIndex>0)
    {
        std::vector<unsigned> peakLocTmp(  peakLock.begin(), peakLock.begin()+cIndex-1);
        peaksIndices = selectElements<unsigned>(indices, peakLocTmp);
    }



    for(auto it = peaksIndices.begin();it!=peaksIndices.end();++it)
    {
        std::cout<< *it <<",";

    }


    std::cout<<std::endl;



}


template<typename type>
void FindPeaks<type>::singVector(const vectorT& input, std::vector<int>& output) const
{


    for(auto it = input.begin(); it != input.end(); ++it)
    {
        output.push_back( (*it > 0) ? 1 : (*it < 0) ? -1 : 0 );
    }


}

template<typename type>
template<typename t>
std::vector<t> FindPeaks<type>::selectElements(const std::vector<t>& input, const vectorU& indices) const
{

    std::vector<t> output;
    for(unsigned i=0;i< indices.size();++i)
    {
        output.push_back(input[indices[i]]);

    }

    return output;

}

template<typename type>
typename FindPeaks<type>::vectorU
FindPeaks<type>::findIndicesLessThan(vectorT diffs, type threshold)
{

    vectorU output;

    for(unsigned i=0;i< diffs.size(); ++i)
    {
        if(diffs[i]<threshold)
        {
            output.push_back(i+1);
           // std::cout<<1+i<<", ";
        }

    }

    return output;
}























