#ifndef VTKVIEWER_H
#define VTKVIEWER_H


#include <itkImage.h>
#include <itkCastImageFilter.h>
#include <QuickView.h>
#include <itkCovariantVector.h>

#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkArrowSource.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindowInteractor.h>

#include <itkViewImage.h>





//vtk includes
#include <vtkPlot.h>
#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkIntArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAutoInit.h>

//VTK_MODULE_INIT(vtkRenderingContextOpenGL2)

VTK_MODULE_INIT(vtkRenderingOpenGL2)

VTK_MODULE_INIT(vtkInteractionStyle)




namespace VTKViewer
{






template <typename vectorT>
void PlotBar(const vectorT& data, unsigned size)
{


    vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();

    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    view->GetRenderWindow()->SetSize(400, 300);
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem(chart);

    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();


    vtkSmartPointer<vtkIntArray>  arrMonth = vtkSmartPointer<vtkIntArray>::New();
    arrMonth->SetName("Month");
    table->AddColumn(arrMonth);

    vtkSmartPointer<vtkIntArray>  arr2008 = vtkSmartPointer<vtkIntArray>::New();
    arr2008->SetName("2008");
    table->AddColumn(arr2008);
    table->SetNumberOfRows(size);

    for (int i = 0; i < size; i++)
    {
        table->SetValue(i,0,i+1);

        //std::cout<<data[i]<<std::endl;
        table->SetValue(i,1,data[i]);

    }
    vtkPlot *line = chart->AddPlot(vtkChart::BAR);

    line = chart->AddPlot(vtkChart::BAR);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);


    //Finally render the scene and compare the image to a reference image
    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();


}





template<typename imageT, unsigned imageDim=2>
void visualize(typename imageT::Pointer image, std::string description="")
{
    using pixelT = typename imageT::PixelType;
    if constexpr(std::is_arithmetic<pixelT>::value) // is gray-level
    {


        using imageChar = itk::Image< unsigned char, imageDim >;
        using castFilterCastType = itk::CastImageFilter< imageT, imageChar >;
        typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
        castFilter->SetInput( image );
        castFilter->Update();

        //visualizing
        //itk::ViewImage<imageChar>::View(castFilter->GetOutput());
        QuickView viewer;
        viewer.AddImage(castFilter->GetOutput(), true, description);
        viewer.Visualize();
    }
    else if constexpr (std::is_unsigned<typename pixelT::ComponentType>::value)
    {

        using rgbPixelType = itk::RGBPixel<unsigned char>;
        using rgbImageChar = itk::Image< rgbPixelType, imageDim >;
        using castFilterCastType = itk::CastImageFilter< imageT, rgbImageChar >;
        typename castFilterCastType::Pointer castFilter = castFilterCastType::New();
        castFilter->SetInput( image );

        //visualizing
        QuickView viewer;
        viewer.AddRGBImage(castFilter->GetOutput(), true, description);
        viewer.Visualize();

    }

}

template<typename imageT, typename vectorImageT>
void visualizeVectorImage(typename imageT::Pointer inputImage, typename vectorImageT::Pointer vectorImage)
{

    //only vector images
    using vectorPixelT = typename vectorImageT::PixelType;
    if constexpr(!std::is_unsigned<vectorPixelT>::value) // is rgb or vector
    {
        if constexpr(std::is_floating_point<typename vectorPixelT::ComponentType>::value)
        {
            std::cout<<"vector type"<<std::endl;


            typename vectorImageT::RegionType region  = vectorImage->GetLargestPossibleRegion();
            typename vectorImageT::SizeType imageSize = region.GetSize();

            vtkSmartPointer<vtkImageData> VTKImage =  vtkSmartPointer<vtkImageData>::New();

            VTKImage->SetExtent(0, imageSize[0] -1, 0, imageSize[1] - 1, 0, 0);

            vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
            vectors->SetNumberOfComponents(3);
            vectors->SetNumberOfTuples(imageSize[0] * imageSize[1]);
            vectors->SetName("GradientVectors");

            int counter = 0;
              for(unsigned int j = 0; j < imageSize[1]; j++)
                {
                for(unsigned int i = 0; i < imageSize[0]; i++)
                  {
                  typename vectorImageT::IndexType index;
                  index[0] = i;
                  index[1] = j;

                  typename vectorImageT::PixelType pixel = vectorImage->GetPixel(index);

                  float v[3];
                  v[0] = pixel[0];
                  v[1] = pixel[1];
                  v[2] = 0;
                  vectors->InsertTypedTuple(counter, v);
                  counter++;
                  }
                }
              //std::cout << region << std::endl;

              VTKImage->GetPointData()->SetVectors(vectors);



              using ConnectorType = itk::ImageToVTKImageFilter<imageT>;
              typename ConnectorType::Pointer originalConnector = ConnectorType::New();
              originalConnector->SetInput(inputImage);
              originalConnector->Update();

              vtkSmartPointer<vtkImageActor> originalActor =  vtkSmartPointer<vtkImageActor>::New();
              originalActor->SetInputData(originalConnector->GetOutput());

              vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

              vtkSmartPointer<vtkGlyph3DMapper> gradientMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
              gradientMapper->ScalingOn();
              gradientMapper->SetScaleFactor(.5);
              gradientMapper->SetSourceConnection(arrowSource->GetOutputPort());
              gradientMapper->SetInputData(VTKImage);
              gradientMapper->Update();

              vtkSmartPointer<vtkActor> gradientActor =   vtkSmartPointer<vtkActor>::New();
              gradientActor->SetMapper(gradientMapper);

              // Visualize
              // Define viewport ranges
              // (xmin, ymin, xmax, ymax)
              double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
              double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};

              // Setup both renderers
              vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
              renderWindow->SetSize(imageSize[0]/2,imageSize[1]/2);

              vtkSmartPointer<vtkRenderer> leftRenderer = vtkSmartPointer<vtkRenderer>::New();
              renderWindow->AddRenderer(leftRenderer);
              leftRenderer->SetViewport(leftViewport);

              vtkSmartPointer<vtkRenderer> rightRenderer = vtkSmartPointer<vtkRenderer>::New();
              renderWindow->AddRenderer(rightRenderer);
              rightRenderer->SetViewport(rightViewport);
              rightRenderer->SetBackground(0,0,0);

              leftRenderer->AddActor(originalActor);
              rightRenderer->AddActor(gradientActor);

              vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

              vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
              renderWindowInteractor->SetInteractorStyle(style);

              renderWindowInteractor->SetRenderWindow(renderWindow);
              renderWindowInteractor->Initialize();

              renderWindowInteractor->Start();

        }

    }


}

};


//template class VTKViewer< itk::Image<itk::RGBPixel<unsigned>, 2> >;
//template class VTKViewer< itk::Image<unsigned, 2> >;
//template class VTKViewer< itk::Image<itk::CovariantVector<float, 2>, 2> >;


#endif // VTKVIEWER_H

