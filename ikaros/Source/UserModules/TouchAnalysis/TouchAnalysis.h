#ifndef TouchAnalysis_H
#define TouchAnalysis_H

#include "IKAROS.h"
#include <stdio.h>

class TouchAnalysis: public Module
{
public:
    TouchAnalysis(Parameter * p);
    virtual ~TouchAnalysis();
    static Module * Create(Parameter * p);

    void 		Init();
    void 		Tick();
    void    CheckParameters(void);

    float *     touchActive;

    int         noTouches;
    int         noEmotions;

    float **    touchCertainty_matrix;
    int         touchCertainty_NoRows;

    float **    touchEmotion_matrix;
    float **    touchEmotionCutOff_matrix;

    float **    emotionFactor_matrix;
    float **    emotionScale_matrix;
    float **    emotionHistoryScale_matrix;

    float **    certaintyEmotionCurrent_matrix;
    float **    certaintyEmotionHistory_matrix;
    float **    certaintyEmotionTotal_matrix;

    float **    emotionRGB_matrix;

    int         noRGB;

    float *     emotionPupilSize_array;

    float *     epi_eye_r_array;
    float *     epi_eye_g_array;
    float *     epi_eye_b_array;

    float      crgb_r;
    float      crgb_g;
    float      crgb_b;

// Set scale of emotions
		double emotionScale;
		double emotionHistoryScale;

    float * certaintyEmotionTotal_output_array;
    float *     epi_pupilsize_array;

    int noEmotionsCutoff;

    int noTicks;
    int noTicksEmotion;
    int noTicksEmotionHistory;
    float timeEmotionPeak;
    int noTicksEmotionPeak;

    float emotionHistoryFactor;

private:

};

#endif
