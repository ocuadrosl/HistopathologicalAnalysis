import pandas as pd
import numpy as np
import math
import sys
import random
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Convolution2D, MaxPooling2D, Conv2D
from keras.utils import np_utils
from keras.optimizers import SGD

from sklearn.preprocessing import LabelEncoder

#import os.path

#from tensorflow import keras

import tensorflow as tf
config = tf.compat.v1.ConfigProto()
config.gpu_options.per_process_gpu_memory_fraction = 0.9
tf.compat.v1.Session(config=config)



neurons=[]

for arg in sys.argv:
    if arg[0]=="-":
        if arg[1]=="n":
            neurons.append(int(arg[2:]))
        elif arg[1]=="c":
            colIn=int(arg[2:])
        elif arg[1]=="C":
            colFi=int(arg[2:])
        elif arg[1]=="k":
            Knumber=int(arg[2:])
        elif arg[1]=="t":
            TestRate=float(arg[2:])
        elif arg[1]=="b":
            batch_size=int(arg[2:])
        elif arg[1]=="e":
            nb_epoch=int(arg[2:])
        elif arg[1]=="l":
            LearningRate=float(arg[2:])
        elif arg[1]=="m":
            Momentum=float(arg[2:])
        elif arg[1]=="f":
            fileCSV=arg[2:]
        elif arg[1]=="v":
            Verbose=int(arg[2:])
        elif arg[1]=="o":
            fileOutput=arg[2:]

            
            
rateColumn=(colIn, colFi)


"""
Descrição:

--------     Exemplo de uso:

python3 NN.py -f"B_2009_8854 A_1x.csv" -n8 -n5 -c4 -C61 -k10 -t0.1 -b50 -e200 -l0.1 -m0.0 -v0 -oOutput.csv

--------     Detalhes:

-f    arquivo com as características que serão usadas como input para a NN. ATENÇÃO: a última coluna sempre deve ser os labels.

-n    núrmero de neurônios utilizados em cada camada oculta. Por exemplo, -n2 -n3 irá construir uma camada oculta com dois               neurônios seguida por outra camada oculta com 3 neurônios.

-c    indíce da coluna inicial das características. Consideramos que os indíces inicial com valor 0.

-C    indíce da coluna final das características, a qual também será inclusa. Exemplo, -C23 refere-se a coluna 24 e está fará parte       do input da NN.

-k    Valor para k-foulds

-t    Taxa de amostras (valor float) que serão separadas para serem usadas na fase de teste. Exemplo, 0.1 significa 10% de todas as       amostras.

-b    Quantidade de elementos do batch. Quantidade de amostras usadas na fase de treino para ser apresentada a NN e depois de serem       apresentadas a NN terá os pesos ajustados.

-e    Quantidade de épocas que a NN será treinada. Em outras palavras, quantidade de vezes que todo o conjunto de dados será             apresentado para a NN na fase de treinamento.

-l    Taxa de aprendizado (valor float). Magnitude no ajuste dos pesos. Valores altos representam alta alteração nos pesos.

-m    Taxa de momentum (valor float). Uma taxa de estabilização na atualização de pesos.

-v    Verbose. Se 1, exibe informações da etapa de treinamento. Se 0, não exibe informações de treinamento.

-o    Nome do arquivo csv que conterá os resultados.
"""


# fileCSV="B_2009_8854 A_1x.csv"

# neurons=[8, 5]

# rateColumn=(4, 61) # usa LBP
# rateColumn=(4, 65) # usa LBP + co-ocorrencia

# Knumber=10

# TestRate=0.1

# batch_size=50

# nb_epoch=200

# LearningRate=0.1

# Momentum=0.0





features=pd.read_csv(fileCSV)


np_features= features.to_numpy()

labels=np_features[:,(len(np_features[0])-1)]

dataset=np_features[:,rateColumn[0]:(rateColumn[1]+1)]


del np_features, features


output=[]

mean=int()
for k in range(0,Knumber):
    rowTest=random.sample(range(0,(len(dataset)-1)), int(np.around(len(dataset)*TestRate, 0)))




    rowTrain=[]
    for row in range(0, len(dataset)):
        if not (row in rowTest):
            rowTrain.append(row)

    dataset_test=dataset[rowTest]
    dataset_train=dataset[rowTrain]

    labels_test=labels[rowTest]
    labels_train=labels[rowTrain]



    dataset_train=np.asarray(dataset_train)
    dataset_train=np.float32(dataset_train)
    dataset_train=dataset_train.astype('float32')
    dataset_train=dataset_train.reshape(dataset_train.shape[0], len(dataset_train[0]))


    dataset_test=np.asarray(dataset_test)
    dataset_test=np.float32(dataset_test)
    dataset_test=dataset_test.astype('float32')
    dataset_test=dataset_test.reshape(dataset_test.shape[0], len(dataset_test[0]))



    encoder = LabelEncoder()
    encoder.fit(labels_train)
    labels_train = encoder.transform(labels_train)

    encoder = LabelEncoder()
    encoder.fit(labels_test)
    labels_test = encoder.transform(labels_test)






    model=Sequential()
    model.add(tf.keras.Input(shape=(len(dataset[0]), )))
    for neuron in neurons:
        model.add(Dense(neuron, activation='relu'))
    model.add(Dense(1, activation='sigmoid'))

    opt = SGD(lr=LearningRate, momentum=Momentum)

    model.compile(loss="binary_crossentropy", optimizer=opt, metrics=['accuracy'])

    modelFit=model.fit(dataset_train, labels_train, batch_size=batch_size, epochs=nb_epoch, verbose=Verbose)


    # print(len(dataset_train))
    # print(len(labels_train))

    # print(len(dataset_test))
    # print(len(labels_test))

    # sys.exit()

    results = model.evaluate(dataset_test, labels_test, batch_size=batch_size, verbose=0)
#     print("------------------------------------------------------------")
#     print("------------------------------------------------------------")
#     print("--------------------Avaliação-------------------------------")
#     print("------------------------------------------------------------")
    print("------------------------------------------------------------")
    print("k: ", (k+1), " --- Test loss, Test acc:", results)
    mean+=results[1]/(Knumber+1)
    output.append(results)
    
print("Acurácia média: ", mean)

df_output = pd.DataFrame(output, columns= ['Loss', 'Accuracy'])

df_output.to_csv (fileOutput, index = False, header=True)