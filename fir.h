#ifndef FIR__H__
#define FIR__H__

#define MAX_ORDER 24

typedef struct{
    int order;
    float b[MAX_ORDER + 1];
    float inputs[MAX_ORDER + 1];
}FIRfilter;

void initFilter(int order, float b[], FIRfilter *filter);
float filterSignal(float sample, FIRfilter *filter);
void resetFilter(int order, FIRfilter *filter);
#endif