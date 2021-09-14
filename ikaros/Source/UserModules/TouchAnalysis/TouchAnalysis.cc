//
#include <fstream>
#include <utility>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

#include "TouchAnalysis.h"
#include <math.h>
#include <sstream>
#include <string>
#include <list>

using namespace ikaros;

// std::poisson_distribution<int> distribution(4);
// std::default_random_engine generator;

Module *
TouchAnalysis::Create(Parameter * p)
{
	return new TouchAnalysis(p);
}

TouchAnalysis::TouchAnalysis(Parameter * p): Module(p)
{
    CheckParameters();
}

void TouchAnalysis::Init()
{
	touchActive = GetInputArray("TOUCHACTIVE_INPUT");

	touchCertainty_matrix = GetInputMatrix("TOUCHCERTAINTY_INPUT");
	touchCertainty_NoRows = GetInputSizeX("TOUCHCERTAINTY_INPUT");

	touchEmotion_matrix = GetInputMatrix("TOUCHEMOTION_INPUT");
	noTouches = GetInputSizeX("TOUCHEMOTION_INPUT");
	noEmotions = GetInputSizeY("TOUCHEMOTION_INPUT");

	touchEmotionCutOff_matrix = GetInputMatrix("TOUCHEMOTIONCUTOFF_INPUT");

	emotionFactor_matrix = GetInputMatrix("EMOTIONFACTOR_INPUT");
	certaintyEmotionTotal_output_array = GetOutputArray("CERTAINTYEMOTIONTOTAL_OUTPUT");

	emotionScale_matrix = create_matrix(noEmotions,1);
	emotionHistoryScale_matrix = create_matrix(noEmotions,1);
	certaintyEmotionCurrent_matrix = create_matrix(noEmotions,1);
	certaintyEmotionHistory_matrix = create_matrix(noEmotions,1);
	certaintyEmotionTotal_matrix = create_matrix(noEmotions,1);

	for (int i=0; i<noEmotions; i++)
	{
		emotionScale_matrix[0][i] = 0;
		certaintyEmotionCurrent_matrix[0][i] = 0;
		certaintyEmotionHistory_matrix[0][i] = 0;
		certaintyEmotionTotal_matrix[0][i] = 0;
	}

	emotionRGB_matrix = GetInputMatrix("EMOTIONRGB_INPUT");
	noRGB = GetInputSizeY("EMOTIONRGB_INPUT");

	emotionPupilSize_array = GetInputArray("EMOTIONPUPILSIZE_INPUT");

	crgb_r = 119.0/255.0;
	crgb_b = 119.0/255.0;
	crgb_g = 119.0/255.0;

	epi_eye_r_array = GetOutputArray("EPI_EYE_R");
	epi_eye_g_array = GetOutputArray("EPI_EYE_G");
	epi_eye_b_array = GetOutputArray("EPI_EYE_B");

	epi_pupilsize_array = GetOutputArray("EPI_PUPILSIZE");

	Bind(noEmotionsCutoff, "noEmotionsCutoff");
	Bind(timeEmotionPeak, "timeEmotionPeak");
	Bind(emotionHistoryFactor, "emotionHistoryFactor");
//	noTicksEmotionPeak = timeEmotionPeak/0.035;
}

TouchAnalysis::~TouchAnalysis()
{
	// Destroy data structures that is allocated in Init.
	destroy_matrix(emotionScale_matrix);
	destroy_matrix(emotionHistoryScale_matrix);
	destroy_matrix(certaintyEmotionCurrent_matrix);
	destroy_matrix(certaintyEmotionHistory_matrix);
	destroy_matrix(certaintyEmotionTotal_matrix);
}

void TouchAnalysis::Tick()
{
/*	if (noTicks == 0)
	{
		using param_t = std::poisson_distribution<int>::param_type;
		distribution.param(param_t{4});
	} */
//	cout << generator;
//	int number = distribution(noTicks);
/*	int number = distribution(generator);
	cout << "Number: " << number;
	std::cout << '\n';  */

	noTicks++;
	cout << "noTicks: " << noTicks << endl;

	if (touchActive[0] > 0 || noTicksEmotionHistory > 0)
	{
		noTicksEmotionPeak = timeEmotionPeak/0.035;		
// Emotion history
		noTicksEmotionHistory++;
		if (touchActive[0] > 0)
		{
			noTicksEmotionHistory += noTicksEmotion;
			noTicksEmotionHistory = noTicksEmotionHistory/2;
		}
		cout << "noTicksEmotionHistory: " << noTicksEmotionHistory << endl;

		double noTicksEmotionHistoryDoub = noTicksEmotionHistory;
		double emotionExponentHistory = (noTicksEmotionHistoryDoub-noTicksEmotionPeak)/noTicksEmotionPeak;

/*		if (noTicksEmotionHistory <= noTicksEmotionPeak)
		{
			emotionHistoryScale = exp(emotionExponentHistory);
		}
		else
		{
			emotionHistoryScale = exp(-emotionExponentHistory);
		}
		cout << "emotionHistoryScale: " << emotionHistoryScale << endl;
	}
	*/
	cout << "emotionHistoryScale_matrix" << endl;
	for (int i=0; i<noEmotions; i++)
	{
		if (noTicksEmotion <= noTicksEmotionPeak)
		{
			emotionHistoryScale_matrix[0][i] = emotionFactor_matrix[0][i]*exp(emotionExponentHistory);
		}
		else
		{
			emotionHistoryScale_matrix[0][i] = emotionFactor_matrix[0][i]*exp(-emotionExponentHistory);
		}
		cout << emotionHistoryScale_matrix[0][i] << " ";
	}
	cout << endl;
}
// Emotion current
	if (noTicksEmotion > 0  || touchActive[0] > 0)
	{
		cout << "noTicksEmotion: " << noTicksEmotion << endl;

		if (touchActive[0] > 0)
		{
			noTicksEmotion = 0;
		}

//		emotionScale = 0;

		for (int i=0; i<noEmotions; i++)
		{
				emotionScale_matrix[0][i] = 0;
		}

		noTicksEmotion++;
		double noTicksEmotionDoub = noTicksEmotion;
		double emotionExponent = (noTicksEmotionDoub-noTicksEmotionPeak)/noTicksEmotionPeak;

/*		if (noTicksEmotion <= noTicksEmotionPeak)
		{
			emotionScale = exp(emotionExponent);
		}
		else
		{
			emotionScale = exp(-emotionExponent);
		}
		cout << "emotionScale: " << emotionScale << endl;
	*/

		cout << "emotionScale_matrix" << endl;
		for (int i=0; i<noEmotions; i++)
		{
			if (noTicksEmotion <= noTicksEmotionPeak)
			{
				emotionScale_matrix[0][i] = emotionFactor_matrix[0][i]*exp(emotionExponent);
			}
			else
			{
				emotionScale_matrix[0][i] = emotionFactor_matrix[0][i]*exp(-emotionExponent);
			}
			cout << emotionScale_matrix[0][i] << " ";
		}
		cout << endl;
	}


// If a touch is active, provide repsonse.
	if (touchActive[0] > 0)
	{
		if (noTicksEmotionHistory > 0)
		{
			certaintyEmotionHistory_matrix = add(certaintyEmotionHistory_matrix, certaintyEmotionCurrent_matrix, certaintyEmotionHistory_matrix, noEmotions, 1);
		}
		cout << "certaintyEmotionHistory_matrix"<< '\n';
		for (int i=0; i<noEmotions; i++)
		{
				cout << certaintyEmotionHistory_matrix[0][i] << " ";
		}
		std::cout << '\n';

		cout << "noEmotionsCutoff: " << noEmotionsCutoff;
		std::cout << '\n';

		cout << "touchCertainty matrix";
		std::cout << '\n';

		cout << "touchCertainty_NoRows: " << touchCertainty_NoRows;
		std::cout << '\n';

		for (int i=0; i<touchCertainty_NoRows; i++)
		{
				cout << touchCertainty_matrix[0][i] << " ";
		}
		std::cout << '\n';

		cout << "touchEmotion_matrix"<< '\n';
		for (int i=0; i<noTouches; i++)
		{
			for (int j=0; j<noEmotions; j++)
			{
				cout << touchEmotion_matrix[j][i] << " ";
			  int noEmotionsAbove = 0;
				for (int k=0; k<noEmotions; k++)
				{
					if (j != k &&
						touchEmotion_matrix[j][i] < touchEmotionCutOff_matrix[k][i])
					{
						noEmotionsAbove++;
					}
				}
				if (noEmotionsAbove >= noEmotionsCutoff)
				{
					touchEmotion_matrix[j][i] = 0;
				}
			}
			std::cout << '\n';
		}
		std::cout << '\n';

		float ** touchEmotion_matrix_T = transpose(create_matrix(noEmotions, noTouches), touchEmotion_matrix, noEmotions, noTouches);

		cout << "touchEmotion_matrix_T" << '\n';
		for (int i=0; i<noTouches; i++)
		{
			for (int j=0; j<noEmotions; j++)
			{
				cout << touchEmotion_matrix_T[i][j] << " ";
			}
			std::cout << '\n';
		}
		std::cout << '\n';

		float ** certaintyEmotion_matrix = multiply(create_matrix(noEmotions, 1), touchCertainty_matrix, touchEmotion_matrix_T, noEmotions, 1, noTouches);

		cout << "certaintyEmotion_matrix"<< '\n';
		for (int i=0; i<noEmotions; i++)
		{
//				certaintyEmotion_matrix[0][i] = (certaintyEmotion_matrix[0][i]/certaintyEmotionScale);
				cout << certaintyEmotion_matrix[0][i] << " ";
		}
	  std::cout << '\n';

		cout << "emotionRGB_matrix" << '\n';
		for (int i=0; i<noRGB; i++)
		{
			for (int j=0; j<noEmotions; j++)
			{
				cout << emotionRGB_matrix[i][j] << " ";
			}
			std::cout << '\n';
		}
	  std::cout << '\n';

		for (int i=0; i<noEmotions; i++)
		{
				certaintyEmotionCurrent_matrix[0][i] = certaintyEmotion_matrix[0][i];
		}

		destroy_matrix(touchEmotion_matrix_T);
		destroy_matrix(certaintyEmotion_matrix);
	}

	float ** emotionRGB_matrix_T = transpose(create_matrix(noRGB, noEmotions), emotionRGB_matrix, noRGB, noEmotions);

	cout << "emotionRGB_matrix_T"<< '\n';
	for (int i=0; i<noEmotions; i++)
	{
		for (int j=0; j<noRGB; j++)
		{
			cout << emotionRGB_matrix_T[i][j] << " ";
		}
		std::cout << '\n';
	}
	std::cout << '\n';

// Total emotion scaled
	float certaintyEmotionCurrentScale = 0;
	for (int i=0; i<noEmotions; i++)
	{
			certaintyEmotionCurrentScale += certaintyEmotionCurrent_matrix[0][i];
	}
	cout << "certaintyEmotionCurrentScale "<< certaintyEmotionCurrentScale << '\n';
  std::cout << '\n';

	float ** certaintyEmotionCurrentScaled_matrix = create_matrix(noEmotions,1);
	if (certaintyEmotionCurrentScale > 0)
	{
		for (int i=0; i<noEmotions; i++)
		{
				certaintyEmotionCurrentScaled_matrix[0][i] = emotionScale_matrix[0][i]*certaintyEmotionCurrent_matrix[0][i]/certaintyEmotionCurrentScale;
//				certaintyEmotionTotal_output_array[i] = certaintyEmotionCurrentScaled_matrix[0][i];
		}
	}

	float certaintyEmotionHistoryScale = 0;
	for (int i=0; i<noEmotions; i++)
	{
			certaintyEmotionHistoryScale += certaintyEmotionHistory_matrix[0][i];
	}
	cout << "certaintyEmotionHistoryScale "<< certaintyEmotionHistoryScale << '\n';
  std::cout << '\n';

	float ** certaintyEmotionHistoryScaled_matrix = create_matrix(noEmotions,1);
	if (certaintyEmotionHistoryScale > 0)
	{
		for (int i=0; i<noEmotions; i++)
		{
				certaintyEmotionHistoryScaled_matrix[0][i] = emotionHistoryFactor*emotionHistoryScale_matrix[0][i]*certaintyEmotionHistory_matrix[0][i]/certaintyEmotionHistoryScale;
		}
	}

	certaintyEmotionTotal_matrix = add(certaintyEmotionTotal_matrix, certaintyEmotionCurrentScaled_matrix, certaintyEmotionHistoryScaled_matrix, noEmotions, 1);

	float ** certaintyEmotionTotalScaled_matrix = create_matrix(noEmotions,1);
	for (int i=0; i<noEmotions; i++)
	{
			certaintyEmotionTotalScaled_matrix[0][i] = certaintyEmotionTotal_matrix[0][i]/(1+emotionHistoryFactor);
			certaintyEmotionTotal_output_array[i] = certaintyEmotionTotalScaled_matrix[0][i];
	}

	float certaintyEmotionTotalScale = 0;
	for (int i=0; i<noEmotions; i++)
	{
			certaintyEmotionTotalScale += certaintyEmotionTotalScaled_matrix[0][i];
	}
	cout << "certaintyEmotionTotalScale "<< certaintyEmotionTotalScale << '\n';
  std::cout << '\n';

/*	if (certaintyEmotionTotalScale > 1)
	{
			certaintyEmotionTotalScale = 1;
	}
*/

/*		if (certaintyEmotionTotalScale > 0)
	{
		for (int i=0; i<noEmotions; i++)
		{
				certaintyEmotionTotal_matrix[0][i] = certaintyEmotionTotal_matrix[0][i]/certaintyEmotionTotalScale;
		}
	}


	float certaintyEmotionTotalScale2 = 0;
	for (int i=0; i<noEmotions; i++)
	{
			certaintyEmotionTotalScale2 += certaintyEmotionTotal_matrix[0][i];
	}
	cout << "certaintyEmotionTotalScale2 "<< certaintyEmotionTotalScale2 << '\n';
	std::cout << '\n';
*/
// RGB
	float ** certaintyRGB_matrix = multiply(create_matrix(noRGB, 1), certaintyEmotionTotalScaled_matrix, emotionRGB_matrix_T, noRGB, 1, noEmotions);
	cout << "certaintyRGB_matrix"<< '\n';
	for (int j=0; j<noRGB; j++)
	{
		cout << certaintyRGB_matrix[0][j] << " ";
	}
	std::cout << '\n';

	crgb_r = (1-certaintyEmotionTotalScale)*119/255 + certaintyRGB_matrix[0][0]/255;
	crgb_g = (1-certaintyEmotionTotalScale)*119/255 + certaintyRGB_matrix[0][1]/255;
	crgb_b = (1-certaintyEmotionTotalScale)*119/255 + certaintyRGB_matrix[0][2]/255;

	/*if (crgb_r > 1)
	{
		crgb_r = 1;
	}
	if (crgb_g > 1)
	{
		crgb_g = 1;
	}
	if (crgb_b > 1)
	{
		crgb_b = 1;
	}
	if (crgb_r < 0)
	{
		crgb_r = 0;
	}
	if (crgb_g < 0)
	{
		crgb_g = 0;
	}
	if (crgb_b < 0)
	{
		crgb_b = 0;
	} */

	destroy_matrix(emotionRGB_matrix_T);
	destroy_matrix(certaintyRGB_matrix);
	destroy_matrix(certaintyEmotionCurrentScaled_matrix);
	destroy_matrix(certaintyEmotionHistoryScaled_matrix);
	destroy_matrix(certaintyEmotionTotalScaled_matrix);

	std::cout << "CR, CG, CB: " << crgb_r << ", " << crgb_g <<  ", " << crgb_b;
	std::cout << '\n';

	epi_eye_r_array[0] = crgb_r;
	epi_eye_g_array[0] = crgb_g;
	epi_eye_b_array[0] = crgb_g;
//	epi_eye_r_array[0] = emotionScale*(crgb_r + (emotionHistoryScale*epi_eye_r_array[0])) + (1-(emotionScale*(1+emotionHistoryScale))*epi_eye_r_array[0]);
//	epi_eye_b_array[0] = emotionScale*(crgb_g + (emotionHistoryScale*epi_eye_g_array[0])) + (1-(emotionScale*(1+emotionHistoryScale))*epi_eye_g_array[0]);
//	epi_eye_g_array[0] = emotionScale*(crgb_b + (emotionHistoryScale*epi_eye_b_array[0])) + (1-(emotionScale*(1+emotionHistoryScale))*epi_eye_b_array[0]);

	std::cout << "R, G, B: " << epi_eye_r_array[0] << ", " << epi_eye_b_array[0] <<  ", " << epi_eye_g_array[0];
	std::cout << '\n';

/*	std::cout << '\n';
	cout << "emotionPupilSize_array"<< '\n';
	for (int i=0; i<noEmotions; i++)
	{
		cout << emotionPupilSize_array[i] << " ";
	}
	std::cout << '\n';

	float * certaintyPupilsize_array = multiply(create_array(1), certaintyEmotion_matrix, emotionPupilSize_array, noEmotions, 1);

	std::cout << '\n';
	cout << "certaintyPupilsize_array"<< '\n';
	cout << certaintyPupilsize_array[0]<< '\n';

	epi_pupilsize_array[0] = certaintyPupilsize_array[0]/20;
	destroy_array(certaintyPupilsize_array);

*/
	epi_pupilsize_array[0] = 0.5;

//	}
}

void TouchAnalysis::CheckParameters()
{
}

static InitClass init("TouchAnalysis", &TouchAnalysis::Create, "Source/UserModules/TouchAnalysis/");
