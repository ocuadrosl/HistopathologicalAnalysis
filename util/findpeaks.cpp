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

    const vectorF data(inputData.begin(), inputData.end());


    vectorF dx;
    math::diff<float>(data, dx);


    std::replace(dx.begin(), dx.end(), 0.0, -EPS);

    vectorF dx_1(dx.begin()  , dx.end()-1);
    vectorF dx_2(dx.begin()+1, dx.end()  );
    vectorF dx_3;
    math::vectorProduct<float>(dx_1, dx_2, dx_3);


    vectorU indices;
    findIndicesLessThan(dx_3, indices); // Find where the derivative changes sign

    vectorU indAux(indices.begin(), indices.end());

    vectorF  x;
    selectElements<float>(data, indAux, x);

    x.insert(x.begin(), *data.begin());
    x.insert(x.end(), *data.rbegin());

    indices.insert(indices.begin(),0);
    indices.insert(indices.end(), data.size()-1);


    unsigned minMagIndex = std::distance(x.begin(), std::min_element(x.begin(),x.end()));
    float minMag  = x[minMagIndex];
    float leftMin = minMag;


    unsigned size = x.size();


    if(size > 2)
    {
        float tempMag = minMag;

        vectorF subVector(x.begin(), x.begin() + 3);
        vectorF diff;
        math::diff<float>(subVector, diff);


        std::vector<int> singDx;

        singVector(diff, singDx);


        if(singDx[0] <= 0 )
        {
            if( singDx[0] == singDx[1])
            {
                x.erase(x.begin()+1);
                indices.erase(indices.begin()+1);
                --size;
            }
        }
        else if ( singDx[0] == singDx[1] )
        {
            x.erase(x.begin());
            indices.erase(indices.begin());
            --size;
        }

        unsigned maxPeak = static_cast<unsigned>(std::ceil( size / 2.f ));

        std::vector<unsigned> peakLock(maxPeak, 0);
        vectorF peakMag(maxPeak, 0.f);

        unsigned cIndex = 0;
        unsigned tempLoc;
        int ii = ( x[0] >= x[1]) ? -1 : 0;
        bool foundPeak = false;
        float sel = (std::max_element(data.begin(), data.end()) - std::min_element(data.begin(), data.end())) / 4.f;

        while (ii < size)
        {

            ++ii;

            if(foundPeak)
            {
                tempMag = minMag;
                foundPeak = false;

            }

            if(x[static_cast<unsigned>(ii)] > tempMag && x[static_cast<unsigned>(ii)] > (leftMin + sel))
            {

                tempLoc = static_cast<unsigned>(ii);
                tempMag = x[static_cast<unsigned>(ii)];
            }

            if(ii == size) break;

            ++ii;

            if (!foundPeak && tempMag > (sel + x[static_cast<unsigned>(ii)]))
            {
                foundPeak = true;
                leftMin = x[static_cast<unsigned>(ii)];
                peakLock[cIndex] = tempLoc;
                peakMag[cIndex] = tempMag;
                ++cIndex;

            }
            else if ( x[static_cast<unsigned>(ii)] < leftMin )
            {
                leftMin = x[static_cast<unsigned>(ii)];

            }


        }



        if( *x.rbegin() > tempMag && *x.rbegin() > (leftMin +sel))
        {
            peakLock[cIndex] = size - 1;
            peakMag[cIndex]  = *x.rbegin();
            ++cIndex;

        }
        else if( !foundPeak && tempMag > minMag)
        {
            peakLock[cIndex] = tempLoc;
            peakMag[cIndex] = tempMag;
            ++cIndex;

        }


        if(cIndex > 0)
        {
            std::vector<unsigned> peakLocTmp(  peakLock.begin(), peakLock.begin()+cIndex-1);
            selectElements<unsigned>(indices, peakLocTmp, peaksIndices);
        }



        for(auto it = peaksIndices.begin();it!=peaksIndices.end();++it)
        {
            std::cout<< *it <<",";

        }


        std::cout<< "end" <<std::endl;

    }

}


template<typename type>
void FindPeaks<type>::singVector(const vectorF& input, std::vector<int>& output) const
{


    for(auto it = input.begin(); it != input.end(); ++it)
    {
        output.push_back( (*it > 0.f) ? 1 : (*it < 0.f) ? -1 : 0 );
    }


}

template<typename type>
template<typename t>
void FindPeaks<type>::selectElements(const std::vector<t>& input, const vectorU& indices, std::vector<t>& output) const
{


    for(unsigned i=0;i< indices.size(); ++i)
    {
        output.push_back(input[indices[i]]);

    }



}

template<typename type>
void FindPeaks<type>::findIndicesLessThan(const vectorF& data,  vectorU& output)
{


    for(unsigned i=0;i< data.size(); ++i)
    {
        if(data[i] < 0.f)
        {
            output.push_back(i+1);
            // std::cout<<1+i<<", ";
        }

    }


}























