#ifndef ILLUMINANT_H
#define ILLUMINANT_H


#include <itkRGBPixel.h>
#include <map>
#include <locale>


class  Illuminant
{
public:
    using pixelT = itk::RGBPixel<double>;

    //standard codes
    enum  index
    {
        a,
        b,
        c,
        d50,
        d55,
        d65,
        d75,
        e,
        f2,
        f7,
        f11
    };

    static inline pixelT getWhitePoint(unsigned index)
    {
        pixelT white;
        white.Set(0.96422, 1.00000, 0.82521); //default
        switch (index)
        {
            case 0 : white.Set(1.09850, 1.00000, 0.35585); break; //a
            case 1 : white.Set(0.99072, 1.00000, 0.85223); break; //b
            case 2 : white.Set(0.98074, 1.00000, 1.18232); break; //c
            case 3 : white.Set(0.96422, 1.00000, 0.82521); break; //d50
            case 4 : white.Set(0.95682, 1.00000, 0.92149); break; //d55
            case 5 : white.Set(95.0489, 100.000, 108.884); break; //d65
            case 6 : white.Set(0.94972, 1.00000, 1.22638); break;
            case 7 : white.Set(1.00000, 1.00000, 1.00000); break;
            case 8 : white.Set(0.99186, 1.00000, 0.67393); break;
            case 9 : white.Set(0.95041, 1.00000, 1.08747); break;
            case 10: white.Set(1.00962, 1.00000, 0.64350); break;
       }

        return white;

    }


    static inline pixelT getRgbToXyzM(unsigned index)
    {
        pixelT white;
        white.Set(0.96422, 1.00000, 0.82521); //default
        switch (index)
        {
            case 0 : white.Set(1.09850, 1.00000, 0.35585); break;
            case 1 : white.Set(0.99072, 1.00000, 0.85223); break;
            case 2 : white.Set(0.98074, 1.00000, 1.18232); break;
            case 3 : white.Set(0.96422, 1.00000, 0.82521); break;
            case 4 : white.Set(0.95682, 1.00000, 0.92149); break;
            case 5 : white.Set(0.95047, 1.00000, 1.08883); break;
            case 6 : white.Set(0.94972, 1.00000, 1.22638); break;
            case 7 : white.Set(1.00000, 1.00000, 1.00000); break;
            case 8 : white.Set(0.99186, 1.00000, 0.67393); break;
            case 9 : white.Set(0.95041, 1.00000, 1.08747); break;
            case 10: white.Set(1.00962, 1.00000, 0.64350); break;
       }

        return white;

    }


};

#endif // ILLUMINANT_H
