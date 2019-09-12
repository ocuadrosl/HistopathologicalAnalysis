#!/usr/bin/python

import sys
import javabridge
import bioformats
from bioformats import log4j
from bioformats.omexml import OMEXML
import numpy as np
import cv2 as cv2
import matplotlib.pyplot as plt


def minMax(inputValue, orgMin, orgMax, newMin, newMax):
    '''
    Rescaling:
    min-max normalization function
    '''
    den = 0.00000001 if  orgMax == orgMin else orgMax - orgMin
    return  (((newMax - newMin) * (inputValue - orgMin)) / den) + newMin


def computeResolution(physicalX, physicalY, sizeX, sizeY, inputMagnification, outputMagnification):
    '''
    Compute the scale factors along the [x-y] axis
    It uses the input physical pixels size and the original nominal magnification 
    '''
        
    # Physical rescale factor
    phyFactor = inputMagnification / outputMagnification
    
        
    newPhySizeX = phyFactor * physicalX
    newPhySizeY = phyFactor * physicalY    
    
    return (physicalX / newPhySizeX, physicalY / newPhySizeY)



def readVSI(inFileName, outMag=5, nTilesX = 20, nTilesY=20):

    '''
    Read a vsi image tile by tile and return a resized RGB TIFF image
    param outMag:  output magnification
    '''
    # starting jvm
    javabridge.start_vm(class_path=bioformats.JARS, run_headless=True, max_heap_size='8G')

    try:
        log4j.basic_config()

        ome = OMEXML(bioformats.get_omexml_metadata(path=inFileName))
        sizeX = ome.image().Pixels.get_SizeX()
        sizeY = ome.image().Pixels.get_SizeY()
        
        
        nominalMag = np.float(ome.instrument().Objective.get_NominalMagnification())

        physicalX = ome.image().Pixels.get_PhysicalSizeX()
        physicalY = ome.image().Pixels.get_PhysicalSizeY()

        imageReader = bioformats.formatreader.make_image_reader_class()
        reader = imageReader()
        reader.setId(inFileName)

        # Printing some info
        print('Nominal Magnification: ',nominalMag)
        print('Image size: ', sizeX, sizeY)
        print('Physical pixel size in um: ', physicalX, physicalY) # um = micrometers

       
        # Aux variables
        tileBeginX  = 0
        tileBeginY  = 0
        tileCounter = 0;

        hMosaic = []
        vMosaic = []

        for y in range(0, nTilesY):

            # computing begin and height size
            tileBeginY = minMax(y , 0, nTilesY, 0, sizeY)
            height = minMax(y + 1 , 0, nTilesY, 0, sizeY) - tileBeginY


            for x in range(0, nTilesX):

                tileBeginX = minMax(x , 0, nTilesX, 0, sizeX)
                
                width = minMax(x + 1 , 0, nTilesX, 0, sizeX) - tileBeginX

                tile = reader.openBytesXYWH(0, tileBeginX, tileBeginY, width, height)

                tile.shape = (int(height), int(width), 3)

                xFac, yFac = computeResolution(physicalX, physicalY, width, height, nominalMag , outMag)

                # resize tile 
                tileResized = cv2.resize(tile, None, fx=xFac, fy=yFac, interpolation=cv2.INTER_AREA)


                if(x > 0):

                    hMosaic = np.concatenate((hMosaic, tileResized), axis=1)

                else:

                    hMosaic = tileResized

                tileCounter = tileCounter + 1

            if(y > 0):
                vMosaic = np.concatenate((vMosaic, hMosaic), axis=0)
            else:

                vMosaic = hMosaic

            hMosaic = []
            
            #progress = (tileCounter * 100) / (nTilesX * nTilesY)
            #print("processing", str(progress) + '%')

    finally:
        javabridge.kill_vm()

    #print("Resize microscope magnification OK")


    return vMosaic



if __name__ == '__main__':

    #just for testing
    #inFileName = "/home/oscar/data/biopsy/Dataset 1/B 2009 8854/B 2009 8854 A.vsi"
    #inFileName = "/home/oscar/data/biopsy/B2046-18 B20181107/Image01B2046-18 B.vsi"
    #outFileName = "/home/oscar/image.tiff"
    #print(sys.argv[1], sys.argv[2], sys.argv[3])
    

    # arg1 = input file name
    # arg2 = output file name
    # arg3 = magnification factor
 
    try:
        image = readVSI(sys.argv[1], float(sys.argv[3]))
        cv2.imwrite(sys.argv[2], image)
        #plt.imshow(image)
        #plt.show()
        print("1")
    except:
        print("0")
        quit()

