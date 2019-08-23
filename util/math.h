#ifndef MATH_H
#define MATH_H


namespace Math
{

template<typename inputType = double, typename outputType = double>
outputType minMax(inputType inputValue, inputType orgMin, inputType orgMax, outputType newMin, outputType newMax)
{

    double den = ( orgMax == orgMin) ?  static_cast<double>(orgMax - orgMin): 0.00000001;

    return  static_cast<outputType>((( static_cast<double>(newMax - newMin) * static_cast<double>(inputValue - orgMin)) / den) + static_cast<double>(newMin));

}

};




#endif // MATH_H
