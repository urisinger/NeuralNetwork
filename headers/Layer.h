#pragma once
#include "LinearAlgebra.h"


//layer functions
void sigmoid(Vector* x);

void sigmoidder(Vector* x);

void tanh2(Vector* x);

void tanhder(Vector* x);

void relu(Vector* x);

void reluder(Vector* x);

void softmax(Vector* x);

void softmaxder(Vector* x);



typedef struct Layer {
    int size;
    struct Layer* NextLayer;
    struct Layer* LastLayer;
    Matrix* Weights;
    Vector* Biases;
    Vector* input;
    Vector* PreActivateOut;
    Activation ActivationLayer;
    Activation ActivationDervtive;
}Layer;



Layer* NewNetwork(Vector* input, int size, Activation ActivationLayer, Activation ActivationDervtive);
void NewLayer(Layer* LastLayer, int size, Activation ActivationLayer, Activation ActivationDervtive);
Layer* FindTail(Layer* Head);
void NewTailLayer(Layer* Head, int size, Activation ActivationLayer, Activation ActivationDervtive);

Vector* Forward(Layer* layer);
Vector* ForwardNoWaste(Layer* layer,Vector* input);
void BackPropogate(Layer* layer, Vector* error_grad, double learnrate);
void LearnGroup(Layer* head, Matrix* Sample, Matrix* Labels, int epochs, double learnrate);

void FreeNetwork(Layer* layer);
void FreeInputs(Layer* layer);

void PrintNetwork(Layer* layer);