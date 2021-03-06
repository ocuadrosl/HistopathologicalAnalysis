#ifndef PLEURADETECTOR_H
#define PLEURADETECTOR_H

#include <numeric>

//ITK includes
#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCannyEdgeDetectionImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelImageToLabelMapFilter.h>
#include <itkLabelMapToRGBImageFilter.h>
#include <itkAdaptiveHistogramEqualizationImageFilter.h>
#include <itkOtsuThresholdImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkScalarImageToTextureFeaturesFilter.h>
#include <itkConstNeighborhoodIterator.h>
#include <itkAttributeOpeningLabelMapFilter.h>
#include <itkLabelImageToShapeLabelMapFilter.h>
#include <itkShapeLabelObject.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkBinaryContourImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkBinaryThinningImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkAddImageFilter.h>
#include <itkScalarImageToCooccurrenceMatrixFilter.h>
#include <itkVectorContainer.h>

//Dlib includes
#include <dlib/array2d.h>
#include <dlib/image_transforms.h>
#include <dlib/clustering.h>
#include <dlib/rand.h>
#include <dlib/svm.h>


//local includes
#include "../util/customprint.h"
#include "../util/vtkviewer.h"
#include "../util/inputOutput.h"
#include "../util/fractaldimensionfilter.h"
#include "../util/extractchannelfilter.h"
#include "../util/colorconverterfilter.h"
#include "../util/imageframeworktypeconverter.h"
#include "../util/ImageOperations.h"

#include <fstream>

template<typename RGBImageT>
class PleuraDetector
{

    using RGBImageP  =  typename RGBImageT::Pointer;

    using GrayImageT = itk::Image<unsigned, 2>;
    using GrayImageP = GrayImageT::Pointer;

    //default pixel values for binary images, i.e., edges, otsu, and etc
    const GrayImageT::PixelType Background = 0;
    const GrayImageT::PixelType Foreground = 255;

    //shape label map
    using LabelType = unsigned;
    using ShapeLabelObjectType = itk::ShapeLabelObject<LabelType, 2>;
    using LabelMapType = itk::LabelMap<ShapeLabelObjectType>;
    using LabelMapP = LabelMapType::Pointer;


    //float type image
    using FloatImageT = itk::Image<float,2>;
    using FloatImageP = FloatImageT::Pointer;

    //texture
    using TextureFilterType = itk::Statistics::ScalarImageToTextureFeaturesFilter<GrayImageT>;
    using FeatureVector = TextureFilterType::FeatureValueVector;

    using LBPHistogramT  = dlib::matrix<float, 58, 1>;
    using LBPHistogramsT =  std::vector<LBPHistogramT>;


    using SCAssignments = std::vector<unsigned long>; // Spectral cluster assignments

    //ITK index vector

    using IndexVector = std::vector<GrayImageT::IndexType>;

    using  CooccurrenceFeatures = std::vector<std::vector<float>>;



    using SampleT = dlib::matrix<double>; //1+58+4 ,1  fractal=1, LBP=58, Co-ocorrence=4


public:

    PleuraDetector();
    void SetInputImage(RGBImageP InputImage);
    void Detect();

    void SetImageName(const std::string& dirPath, const std::string& fileName);
    void SetLabelImage(RGBImageP InputImage);
    void SetCSVFileName(std::string csvFileName);


    void SetNeighborhoodSize(unsigned neighborhoodSize=100);



    void Train();

    GrayImageP RGBToGray(RGBImageP inputImage);


    //last version
    void PreProcessingOn() { PreProcessing = true;  }
    void PreProcessingOff(){ PreProcessing = false; }


    void ReadDataSet(const std::string& path);


private:



    RGBImageP InputImage;

    RGBImageP LabelImage;

    std::string ImageName = "";
    std::string ImageDirPath   = "";
    std::string CSVFilename = "";



    //Last version
    unsigned NeighborhoodSize = 101;
    bool PreProcessing = true;


    //Auxiliary functions
    GrayImageP  HistogramEqualization(GrayImageP grayImage, float alpha=1, float beta=0, unsigned radiusSize=5, bool show=false);
    GrayImageP  EdgeDetectionCanny(GrayImageP grayImage, float variance=5, bool show=false);
    LabelMapP   ConnectedComponets(GrayImageP grayImage, unsigned threhold = 0, unsigned background = 0, bool show=false);
    FloatImageP ComputeFractalDimension(LabelMapP components,  float threshold, bool show=false);
    FloatImageP ComputeRoundness(LabelMapP components, float threshold,  bool show=false);

    RGBImageP   CleanBackground(RGBImageP inputImage, float lThreshold=90.f, float aThreshold = 5.f, float bThresold = 5.f, bool show=false);



    FloatImageP RayFeatures(GrayImageP edges, unsigned raysSize, bool show=false);

    void ComputeLBP(GrayImageP grayImage, GrayImageP edges, const std::vector<GrayImageT::IndexType>& centers,  LBPHistogramsT& lbpHistograms);


    GrayImageP ExtractBoundaries(GrayImageP binaryImage, bool show=false);



    GrayImageP GrayToBinary(GrayImageP grayImage,  bool show=false); //simple threshold assuming background = zero

    GrayImageP ThinningBoundaries(GrayImageP boundaries, bool show=false);


    void ConnectBackground(GrayImageP& grayImage);

    void ComputeGradients(GrayImageP binaryImage, bool show=false);

    void SpectralClustering(LBPHistogramsT& lbpHistograms, SCAssignments& assignments);

    void ComputeCenters(GrayImageP boundaries,   std::vector<GrayImageT::IndexType>& centers);

    void ShowAssignments(const SCAssignments& assignments, const std::vector<GrayImageT::IndexType>& centers);

    void ComputeFractalDimensionCenters(GrayImageP boundaries,

                                        const IndexVector& centers,
                                        std::vector<float>& output,
                                        bool show=false); // put a better name

    void ComputeCooccurrenceMatrices(GrayImageP boundaries,

                                     const IndexVector& centers,
                                     CooccurrenceFeatures& features);




    //methods for testing
    void WriteCSVFile(const std::string& fileName,
                      const IndexVector& centers,
                      const std::vector<float>& fractalDimension,
                      const LBPHistogramsT& LBPHistograms,
                      const CooccurrenceFeatures& cooccurrenceFeatures,
                      const std::vector<float>& labels,
                      bool writeHeader=true);




    void FeaturesToDLibMatrix(const std::vector<float>& fractalDimension,
                              const LBPHistogramsT& LBPHistograms,
                              const CooccurrenceFeatures& cooccurrenceFeatures,
                              std::vector<SampleT>& samples);


    void ReadCSVFile (const std::string& fileName,
                      std::vector<SampleT>& samples,
                      unsigned samplesIndexBegin,
                      unsigned samplesIndexEnd,
                      std::vector<float>& labels,
                      unsigned labelsIndex); //TODO add fractal dimension and LBP histograms

    void ReadAssignmentsFile(const std::string& fileName, std::vector<unsigned>& assignments);
    void DrawAssignments(IndexVector& centers, unsigned neighborhoodSize, std::vector<unsigned>& assignments);

    void MatchCentersWithLabels(const IndexVector& centers, std::vector<float>& labels);

    void KRRTrainer( std::vector<SampleT>& samples, const std::vector<double>& labels);





};

using rgbImageU = itk::Image<itk::RGBPixel<unsigned>, 2>;
template class PleuraDetector<rgbImageU>;

//using rgbImageUChar = itk::Image<itk::RGBPixel<unsigned char>, 2>;
//template class PleuraDetector<rgbImageUChar>;

#endif // PLEURADETECTION_H
