#ifndef MLExt_H
#define MLExt_H

#include "IKAROS.h"
#include <stdio.h>

class MLExt: public Module
{
public:
    MLExt(Parameter * p);
    virtual ~MLExt();
    static Module * Create(Parameter * p);

    void 		Init();
    void 		Tick();

    float *     touchIdInput_array;
    int         touchIdInput_NoCols;

    float **    touchLong_matrix;
    int         touchLong_NoCols;

    float **    touchCertainty_matrix;
    int         touchCertainty_NoRows;
    float       touchCertaintyCutoff;

    float *     touchActiveInput_array;
    float *     touchActiveOutput_array;

    int         noTouches;
    int         noEmotions;

    void        CheckParameters(void);

    int touchType;
    int noTouchTypes;
    int noTouchTypesOut;

    float * MLExtOutput_array;

private:
    int noTicks;
    int touchLong_matrix_row; // The row count, determining which row is going to be filled in touchLong_matrix
};

#endif
