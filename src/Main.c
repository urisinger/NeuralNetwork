﻿#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "Layer.h"

int ChangeEndianness(int value) {
    int result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    return result;
}

int main()
{

    srand(time(NULL));

    FILE* imageTrainFiles = fopen("../../data/train-images.idx3-ubyte", "rb");
    FILE* imageTrainlabels = fopen("../../data/train-labels.idx1-ubyte", "rb");

    if (imageTrainFiles == NULL || imageTrainlabels == NULL) {
        perror("File Not Found");
        return -1;
    }

    int magic_number;
    fread(&magic_number, sizeof(int), 1, imageTrainFiles);

    if (ChangeEndianness(magic_number) != 2051) {
        printf("Invalid magic number : %d\n", ChangeEndianness(magic_number));
        return -1;
    }

    //create samples and labels
    Matrix* images = NewMat(10000, 784);
    Matrix* labels = NewMat(10000, 10);

    GetSample(images, imageTrainFiles, 16);  //read images

    GetLabel(labels, imageTrainlabels, 8);      //read labels

    //create network
    Layer* HeadLayer = NewNetwork(NewVec(784) ,128, relu, reluder);

    NewTailLayer(HeadLayer, 32 , relu,reluder);

    NewTailLayer(HeadLayer, 10, softmax, softmaxder);

    //train the network
    clock_t start = clock();
    LearnGroup(HeadLayer, images, labels, 10, 0.1);
    printf("%f", (double)(clock() - start)/CLOCKS_PER_SEC);

    //free samples
    FreeMat(images);
    FreeMat(labels);

    FILE* imageTestFiles = fopen("../../data/t10k-images.idx3-ubyte", "rb");    //get test samples
    FILE* imageTestlabels = fopen("../../data/t10k-labels.idx1-ubyte", "rb");   //get test labels


    images = NewMat(10000, 784);
    labels = NewMat(10000, 10);

    GetSample(images, imageTestFiles, 16);  //read images

    GetLabel(labels, imageTestlabels, 8);   //read labels

    double errsum = 0;
    int accuracy = 0;
    for (int i = 0; i < 10000; i++) {
        HeadLayer->input->vals = images->vals[i];
        Vector* output = Forward(HeadLayer);
        double max = 0;
        int maxindex;
        for(int k = 0; k < 10; k++){
            if(output->vals[k] > max){
                max = output->vals[k];
                maxindex = k;
            }
        }
        for(int k = 0; k < 10; k++){
            if(labels->vals[i][k] > 0.99){
                accuracy += (k==maxindex);
                break;
            }
        }
        for (int j = 0; j < 10; j++) {
            errsum -= labels->vals[i][j] * log(output->vals[j] + 1e-9)/log(10);
        }

    }

    printf("\n error is : %f\n", errsum/10000);
    printf("the accuracy is: %f",accuracy/10000.0);

    Vector* err = NewVec(10);

    FreeInputs(HeadLayer);

    //test samples
    int index;
    while (getchar()) {
        index = rand()%10000;
        system("clear");
        HeadLayer->input = NewVec(784);
        HeadLayer->input->vals = images->vals[index];

        for (int i = 0; i < 28; ++i) {
            for (int j = 0; j < 28; ++j) {
                if (images->vals[index][i * 28 + j])
                    printf("%1.3f|", images->vals[index][i * 28 + j]);
                else
                    printf("     |");
            }
            printf("\n");
        }

        Vector* output = ForwardNoWaste(HeadLayer,HeadLayer->input);
        double max = -1;
        int maxnum;
        for (int i = 0; i < 10; i++) {
            if (max < output->vals[i]) {
                max = output->vals[i];
                maxnum = i;
            }
            err->vals[i] = output->vals[i] - labels->vals[index][i];
        }

        printf("\n\n");
        PrintVec(output);
        printf("\n\n");
        PrintVec(err);
        printf("\n%d", maxnum);
    }

    FreeNetwork(HeadLayer);

    return -1;
}
