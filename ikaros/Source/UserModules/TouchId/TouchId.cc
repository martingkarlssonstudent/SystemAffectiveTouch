#include <fstream>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

#include "TouchId.h"
#include <math.h>
#include <sstream>
#include <string>
#include <list>

using namespace ikaros;

Module *
TouchId::Create(Parameter * p)
{
	return new TouchId(p);
}

TouchId::TouchId(Parameter * p): Module(p)
{
    CheckParameters();
}

void TouchId::Init()
{
	Bind(touchId_matrix_NoRows, "touchId_matrix_NoRows");
	Bind(touchIdBuffer_matrix_NoRows, "touchIdBuffer_matrix_NoRows");
	Bind(touchCertaintyCutoff, "touchCertaintyCutoff");
	Bind(touchSignalNormFactor, "touchSignalNormFactor");
	Bind(touchIdStageFactor, "touchIdStageFactor");
	Bind(touchIdEnvFactor, "touchIdEnvFactor");
	Bind(touchIdLabelId, "touchIdLabelId");

	touchSignalInput_array = GetInputArray("TOUCHSIGNAL_INPUT");
	touchSignalInput_NoCols = GetInputSize("TOUCHSIGNAL_INPUT");

	touchActiveOutput_array = GetOutputArray("TOUCHACTIVE_OUTPUT");

	touchCertainty_matrix = GetOutputMatrix("TOUCHCERTAINTY_OUTPUT");
	touchCertainty_NoRows = GetOutputSizeX("TOUCHCERTAINTY_OUTPUT");

	touchIdOutput_array = GetOutputArray("TOUCHID_OUTPUT");

	touchTimeOutput_array = GetOutputArray("TOUCHTIME_OUTPUT");
	noTouchesIdOutput_array = GetOutputArray("NOTOUCHESID_OUTPUT");

	touchIdBuffer_matrix = create_matrix(touchSignalInput_NoCols, touchIdBuffer_matrix_NoRows); //Creates a new internal matrix

	for (int i=0; i<touchIdBuffer_matrix_NoRows; i++)
	{
		for (int j=0; j<touchSignalInput_NoCols; j++)
		{
			touchIdBuffer_matrix[i][j] = 0;
		}
	}

	predictOn = true;
	train = true;

	std::string touchIdFileStr;
	std::string touchIdLabelFileStr;
	touchIdFileStr.append("/home/martin/ikaros/Source/UserModules/TouchAnalysis/Examples/touchId_");
	std::ostringstream touchIdStageFactorStrS;
	touchIdStageFactorStrS << touchIdStageFactor;
	touchIdFileStr.append(touchIdStageFactorStrS.str());
	touchIdFileStr.append(".csv");
	touchIdLabelFileStr.append("/home/martin/ikaros/Source/UserModules/TouchAnalysis/Examples/touchIdLabel_");
	touchIdLabelFileStr.append(touchIdStageFactorStrS.str());
	touchIdLabelFileStr.append(".csv");
  touchIdCSVfile.open(touchIdFileStr, std::ofstream::out | std::ofstream::app);
  touchIdLabelCSVfile.open(touchIdLabelFileStr,std::ofstream::out | std::ofstream::app);

}

TouchId::~TouchId()
{
// Destroy data structures that is allocated in Init.
	destroy_matrix(touchIdBuffer_matrix);
	touchIdCSVfile.close();
	touchIdLabelCSVfile.close();
}

void
TouchId::Command(std::string s, float x, float y, std::string value)
{
    if(s == "predict")
        Predict();
}

void
TouchId::Predict()
{
		if (predictOn)
		{
			predictOn =false;
		}
		else
		{
			predictOn = true;
		}
    printf("Predict\n");
}

static inline int naive_bayesian(float * buffer, float * means, float * variances, int length,int max_movements,float threshold)
{
  float buffer_mean = mean(buffer,length);
  float out_prop = 0;
  float cat = -1;

  for(int i=0; i < max_movements; i++){
    float m = means[i];
    float v = variances[i];
    float prob = 1 / sqrt(2 * pi * v) * exp(-sqr(buffer_mean-m) / (2 * v));

    if(prob > out_prop){
      out_prop = prob;
      cat = i;
    }
  }
  printf("out_prop: %f \n",out_prop);
  if(out_prop < threshold){
    cat = -1;
  }
  return cat;
}

void TouchId::Tick()
{
//	int touchSignal = 0;
  touchActive = false;
	touchActiveOutput_array[0] = 0;
	sumTouchBufferSignal = 0;
	changeTouchBufferSignal = 0;
//	float * changeTouchBufferSignal_array;
//	float meanChangeTouchBufferSignal = 0;

//	changeTouchBufferSignal = create_array(12);
	cout << "touchIdBuffer_matrix_row: " << touchIdBuffer_matrix_row << endl;
	for (int i=0; i<touchSignalInput_NoCols; i++)
	{
		touchIdBuffer_matrix[touchIdBuffer_matrix_row][i] = touchSignalInput_array[i]/touchSignalNormFactor;
	}
	touchIdBuffer_matrix_row++;
	if (touchIdBuffer_matrix_row == touchIdBuffer_matrix_NoRows)
	{
		touchIdBuffer_matrix_row = 0;
	}

	for (int i=0; i<touchIdBuffer_matrix_NoRows; i++)
	{
/*		int iCompare;
		if (i == 0)
		{
			iCompare = 9;
		}
		else
		{
			iCompare = i-1;
		} */
		for (int j=0; j<touchSignalInput_NoCols; j++)
		{
			sumTouchBufferSignal += touchIdBuffer_matrix[i][j];
//			changeTouchBufferSignal[j] += touchIdBuffer_matrix[i][j] - touchIdBuffer_matrix[iCompare][j];
		}
	}
	changeTouchBufferSignal = sumTouchBufferSignal - sumTouchBufferSignalPrev;
	sumTouchBufferSignalPrev = sumTouchBufferSignal;

//  int c = naive_bayesian(buffer_angle,mean_array,variance_array,input_length,max_movements,baysian_threshold);
	cout << "sumTouchBufferSignal: " << sumTouchBufferSignal << endl;
//	meanChangeTouchBufferSignal = mean(changeTouchBufferSignal,12);
//	cout << "meanChangeTouchBufferSignal: " << meanChangeTouchBufferSignal << endl;
	cout << "changeTouchBufferSignal: " << changeTouchBufferSignal << endl;
//	std::cout << '\n';

	if (noTicks > 0)
	{
		touchActive = true;
	}
	else
	{
	//		for (int i=0; i<touchSignalInput_NoCols; i++)
	//		{
	//			touchSignal = touchSignalInput_array[i];
	//		if (touchSignal > 0.5)
		if (sumTouchBufferSignal > touchIdStageFactor/touchSignalNormFactor)
		{
			touchActive = true;
			cout << "Touch active" << endl;
	//			break;
		}
	//		}
	}

	touchEnv_Previous = touchEnv_Current;
	if (!touchActive)
	{
			touchEnv_Current = none;
	}
	else if (abs(changeTouchBufferSignal)<touchIdEnvFactor*touchIdStageFactor/touchSignalNormFactor)
	{
			touchEnv_Current = sustain;
	}
	else if (changeTouchBufferSignal>0)
	{
			touchEnv_Current = attack;
	}
	else if (changeTouchBufferSignal<0)
	{
			touchEnv_Current = release;
	}

	if (touchActive)
	{
		if (noTicks == 0)
		{
			touchId_matrix_NoRows = touchId_matrix_NoRows;
	  	touchId_matrix_row = 0;
			touchId_matrix = create_matrix(touchSignalInput_NoCols,touchId_matrix_NoRows); //Creates a new internal matrix
			while (touchId_matrix_row < touchId_matrix_NoRows)
			{
				for (int j=0; j<touchSignalInput_NoCols; j++)
				{
			      touchId_matrix[touchId_matrix_row][j] = 0;
				}
				touchId_matrix_row++;
			}
			touchId_matrix_row = 0;
		}

		noTicks++;

		touchTimeOutput_array[0] = noTicks*0.035;

	  for (int i=0; i<touchSignalInput_NoCols; i++)
		{
	      touchId_matrix[touchId_matrix_row][i] = touchSignalInput_array[i]/touchSignalNormFactor;
		}
		touchId_matrix_row++;

		cout << "touchEnv_Current: " << touchEnv_Current << endl;
		cout << "touchEnv_Previous: " << touchEnv_Previous << endl;

		int touchId_matrix_row_tmp = 0;
		if (sumTouchBufferSignal < touchIdStageFactor/(2*touchSignalNormFactor))
		{
			touchId_matrix_row = touchId_matrix_NoRows;
		}
		else if (touchEnv_Current != touchEnv_Previous &&
		touchEnv_Previous != none &&
		touchEnv_Current != none)
		{
			touchId_matrix_row_tmp = touchId_matrix_NoRows;
		}

		cout << "touchId_matrix_row: " << touchId_matrix_row << endl;
		cout << "touchId_matrix_row_tmp: " << touchId_matrix_row_tmp << endl;

		if (touchId_matrix_row == touchId_matrix_NoRows ||
			  (touchId_matrix_row_tmp == touchId_matrix_NoRows &&
				predictOn))
		{
			cout << "touchId_matrix";
		  std::cout << '\n';
			int touchIdOutputCol = 0;
			for (int i=0; i<touchId_matrix_NoRows; i++)
			{
				for (int j=0; j<touchSignalInput_NoCols; j++)
				{
					cout << touchId_matrix[i][j] << " ";
					touchIdOutput_array[touchIdOutputCol] = touchId_matrix[i][j];
					touchIdOutputCol++;
				}
			}
		  std::cout << '\n';

			if (touchId_matrix_row == touchId_matrix_NoRows)
			{
				if (train)
				{
					cout << "CSV out" << endl;
					int touchId_array_NoCols = touchSignalInput_NoCols*touchId_matrix_NoRows;
					for (int i=0; i<touchId_array_NoCols; i++)
					{
						if (i==touchId_array_NoCols-1)
						{
					  	touchIdCSVfile << touchIdOutput_array[i];
						}
						else
						{
					  	touchIdCSVfile << touchIdOutput_array[i] << ", ";
						}
					}
					touchIdCSVfile << endl;

					touchIdLabelCSVfile << touchIdLabelId;
					touchIdLabelCSVfile << endl;
					noTouchesId++;
					noTouchesIdOutput_array[0] = noTouchesId;
				}

				noTicks = 0;
				touchId_matrix_row = 0;
				destroy_matrix(touchId_matrix);
			}

			if (predictOn)
			{
				touchActiveOutput_array[0] = 1;
			}
		}
	}
}

void TouchId::CheckParameters()
{
}

static InitClass init("TouchId", &TouchId::Create, "Source/UserModules/TouchId/");
