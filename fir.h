#ifndef FIR__H__
#define FIR__H__
//create filter datatype
//declare public functions

#define MAX_ORDER 24

typedef struct{
    int order;
    float b[MAX_ORDER + 1]; //needs to be dynamic
    double inputs[MAX_ORDER + 1]; //needs to be dynamic
}FIRfilter;

void initFilter(int order, float b[], FIRfilter *filter);
double filterSignal(double sample, FIRfilter *filter);
#endif