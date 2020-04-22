#include "fir.h"

void initFilter(int order, float b[], FIRfilter *filterp){
    filterp->order = order;
    int i;
    for(i=0; i < (filterp->order + 1); i++){
        filterp->b[i] = b[i];
    }
    for(i=0; i < (filterp->order + 1); i++){
        filterp->inputs[i] = 0;
    } 
    return;
}

void updateInputs(float sample, FIRfilter *filterp){
    int i;
    for(i=(filterp->order); i > 0; i--){
        filterp->inputs[i] = filterp->inputs[i-1];
    }
    filterp->inputs[0] = sample;
}

float filterSignal(float sample, FIRfilter *filterp){
    updateInputs(sample, filterp);
    float result = 0;
    int i;
    for(i=0; i < (filterp->order + 1); i++){
        result += filterp->b[i] * filterp->inputs[i];
    }
    return result;
}

void resetFilter(int order, FIRfilter *filterp){
  int i;
    for(i=0; i < (filterp->order + 1); i++){
        filterp->inputs[i] = 0;
    }
}
