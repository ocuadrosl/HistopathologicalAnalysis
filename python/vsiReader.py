#!/usr/bin/python

import sys
import javabridge
import bioformats
from bioformats import log4j
from bioformats.omexml import OMEXML
import numpy as np
import math
import cv2 as cv2
import matplotlib.pyplot as plt

def minMax(inputValue, orgMin, orgMax, newMin, newMax):
    den = 0.00000001 if  orgMax == orgMin else orgMax - orgMin
    # print inputValue, ( ((newMax - newMin) * (inputValue - orgMin)) / den) + newMin
    return  (((newMax - newMin) * (inputValue - orgMin)) / den) + newMin


def computeResolution(physicalX, physicalY, sizeX, sizeY, inputMagnification, outputMagnification):

    eyePice = 10.0
    magnificationDifference = ((inputMagnification * eyePice) - (outputMagnification * eyePice)) + 0.00000000001

    outputPhysicalX = (physicalX * magnificationDifference) + physicalX
    outputPhysicalY = (physicalY * magnificationDifference) + physicalY

    # print outputPhysicalY, outputPhysicalY

    sizeXOutput = sizeX / outputPhysicalX
    sizeYOutput = sizeY / outputPhysicalY

    # print sizeXOutput, sizeYOutput

    return sizeXOutput * sizeYOutput


def imageResize(image, resolution=250000.0):
    '''
    Resize to 250000 (500x500) pixels aprox resolution
    default resolution 500x500
    '''

    height, width = image.shape[:2]

    factor = math.sqrt(resolution / (height * width))

    if factor < 1.0:
        # print ( "Image resized by scale factor " + str(factor))
        return cv2.resize(image, (0, 0), fx=factor, fy=factor, interpolation=cv2.INTER_AREA)
    else:
        return image


def readVSI(inFileName, outMag=5, nTilesX = 20, nTilesY=20):

    '''
    Reading a vsi image tile by tile and return an small TIFF image
    :param outMag:  output magnification
    '''
        # starting jvm
    javabridge.start_vm(class_path=bioformats.JARS, run_headless=True, max_heap_size='8G')

    try:
        log4j.basic_config()

        ome = OMEXML(bioformats.get_omexml_metadata(path=inFileName))
        sizeX = ome.image().Pixels.get_SizeX()
        sizeY = ome.image().Pixels.get_SizeY()

        physicalX = ome.image().Pixels.get_PhysicalSizeX()
        physicalY = ome.image().Pixels.get_PhysicalSizeY()

        imageReader = bioformats.formatreader.make_image_reader_class()
        reader = imageReader()
        reader.setId(inFileName)

        print('Original image size: ', sizeX, sizeY)
        print('Original physical pixel size in micrometers: ', physicalX, physicalY)

        # computing input magnification
        inMag = np.round(np.float(ome.instrument(0).Objective.get_NominalMagnification()), 0)

        # initialize variables
        tileBeginX = 0
        tileBeginY = 0
        tileCounter = 0;

        hMosaicGray = []
        vMosaicGray = []

        for y in range(0, nTilesY):

            # computing begin and height size
            tileBeginY = minMax(y , 0, nTilesY, 0, sizeY)
            height = minMax(y + 1 , 0, nTilesY, 0, sizeY) - tileBeginY


            for x in range(0, nTilesX):

                tileBeginX = minMax(x , 0, nTilesX, 0, sizeX)
                width = minMax(x + 1 , 0, nTilesX, 0, sizeX) - tileBeginX

                tile = reader.openBytesXYWH(0, tileBeginX, tileBeginY, width, height)

                tile.shape = (int(height), int(width), 3)

                newResolution = computeResolution(physicalX, physicalY, width, height, inMag , outMag)

                # resize tile
                tileResized = imageResize(tile, newResolution)
                tileGray =   tileResized# cv2.cvtColor(tileResized, cv2.COLOR_BGR2GRAY)



                if(x > 0):

                    hMosaicGray = np.concatenate((hMosaicGray, tileGray), axis=1)

                else:

                    hMosaicGray = tileGray

                tileCounter = tileCounter + 1

            if(y > 0):
                vMosaicGray = np.concatenate((vMosaicGray, hMosaicGray), axis=0)
            else:

                vMosaicGray = hMosaicGray

            hMosaicGray = []
            progress = (tileCounter * 100) / (nTilesX * nTilesY)
            #print("processing", str(progress) + ' %')

    finally:
        javabridge.kill_vm()

    print("Resize microscope magnification OK")


    return vMosaicGray



if __name__ == '__main__':

    inFileName = "/home/oscar/data/biopsy/Dataset 1/B 2009 8854/B 2009 8854 A.vsi"
    outFileName = "/home/oscar/image.tiff"
    #readVSI(sys.argv[1], sys.argv[2])

    print(sys.argv[1], sys.argv[2], sys.argv[3])

    image = readVSI(sys.argv[1], int(sys.argv[3]))

    cv2.imwrite(sys.argv[2], image)

    plt.imshow(image)
    plt.show()


