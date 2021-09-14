#ifndef TouchId_H
#define TouchId_H

#include "IKAROS.h"
#include <stdio.h>

class TouchId: public Module
{
public:
    TouchId(Parameter * p);
    virtual ~TouchId();
    static Module * Create(Parameter * p);

    void 		Init();
    void 		Tick();
    void        Command(std::string s, float x, float y, std::string value);

    void        Predict();

    int noTicks;

    float *     touchSignalInput_array;
    int         touchSignalInput_NoCols;

    float **    touchId_matrix;
    int         touchId_matrix_NoRows;
    int         touchId_matrix_row;

    float **    touchIdBuffer_matrix;
	  int         touchIdBuffer_matrix_NoRows;
    int         touchIdBuffer_matrix_row;

    float **    touchCertainty_matrix;
    int         touchCertainty_NoRows;

    void        CheckParameters(void);

    float touchCertaintyCutoff;
    float touchSignalNormFactor;
    float touchIdStageFactor;
    float touchIdEnvFactor;

    float sumTouchBufferSignal;
  	float sumTouchBufferSignalPrev;
  	float changeTouchBufferSignal;

    bool touchActive;
    float * touchActiveOutput_array;

    float * touchTimeOutput_array;

    float * touchIdOutput_array;

    enum touchEnv {none, attack, sustain, release};

    touchEnv touchEnv_Previous;
    touchEnv touchEnv_Current;

    bool predictOn;
    int touchIdLabelId;

    int noTouchesId;
    float * noTouchesIdOutput_array;

    bool train = false;
	  ofstream touchIdCSVfile;
	  ofstream touchIdLabelCSVfile;

private:

};

#endif
