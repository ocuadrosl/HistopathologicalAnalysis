#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 17 11:41:29 2020

@author: oscar
"""

from skimage import (io)
from os import listdir
import matplotlib.pyplot as plt
import numpy as np


#data directory must be organized into directories as:
#boundaries
#ground_truth
#images
#labels

datasetPath="/home/oscar/data/biopsy/tiff/test"


#for loop here

for fileName in listdir(datasetPath+"/images"):
    
    imageName = fileName.split(".")[0] 
    boundariesName = imageName+"_boundaries.tiff"
    labelsName = imageName+".tiff"
        
    try:
        labels = io.imread(datasetPath+"/labels/"+labelsName)
              
        boundaries = io.imread(datasetPath+"/boundaries/"+boundariesName)
                 
        shape = boundaries.shape
        pleuraLabelsImage = np.zeros(shape, np.int8)
        
             
        for row in range(0, shape[0]):
            for col in range(0, shape[1]):
               
                if np.equal(boundaries[row,col], [255, 255, 255]).all(): # is boundary
                 
                    #print(labels[row,col][1])
                    if  np.equal(labels[row,col], [0, 255, 0]).all():# is green, TODO review it
                        #print("Green")       
                        pleuraLabelsImage[row,col] = [0,255,0];
                    #else:
                        #print("No Green")       
                        #pleuraLabelsImage[row,col] = [255,0,0];
                        
        io.imsave(datasetPath+'/ground_truth/'+imageName+'_gt.tiff', pleuraLabelsImage)
        print(imageName+" OK")            
        
    except:
        print("ERROR")
    
    
    
    
    
    
    
    
    
    
    
    









