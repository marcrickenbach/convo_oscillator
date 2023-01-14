/*
Author: Marc Rickenbach
Project: Digital Oscillator
Note: Work in progress - harmonics added to the fundamnetal frequency of sine wave oscillator. Experimenting with FFT of incoming signal. Stuck on algo implementation. 
*/


#include "daisy_seed.h"
#include "daisysp.h"
#include "math.h"
#include "daisy.h"
#include "stdlib.h"
#include "stdio.h"

using namespace daisysp;
using namespace daisy::seed;
using namespace daisy;

#define MAXFREQ 10000
#define N 64

static DaisySeed hw;

float sine_osc;
float saw_osc;
float sine_freq      = 440;
float sine_amp       = 0.5f;
float sine_phase     = 0.0f;
float sine_phase_acc = 0.0f;
float sineOut;
bool  endC = true;
bool  endR = true;
float sample_rate;


typedef float real;
typedef struct
{
    real Re;
    real Im;
} complex;

complex v[N]; 

float nPot[4] = {0, 0, 0, 0};


void setOscFreq()
{
    float freqKnob = hw.adc.GetFloat(0);
    // int voct = hw.adc.Get(0);
    // float adcRead = adcLUT[voct];
    float oscFreq  = (freqKnob * MAXFREQ); //* powf(2, adcRead);
    oscFreq        = (oscFreq < 22) ? 22 : oscFreq;
    sine_phase_acc = (TWOPI_F * oscFreq) * (1 / sample_rate);
}

void updateParameters()
{
    nPot[0] = hw.adc.GetFloat(1);
    if(nPot[0] <= 0)
        nPot[0] = 0.01f;
    if(nPot[0] >= 1)
        nPot[0] = 0.9f;
    nPot[1] = hw.adc.GetFloat(2);
    if(nPot[1] <= 0)
        nPot[1] = 0.01f;
    if(nPot[1] >= 1)
        nPot[1] = 0.9f;
}

void processInput(complex *v, int size, complex *tmp, float *inputSig)
{
    
    // divide signal into two half-length signals using the even- and odd-numbered points;

    complex z, w, *sigOdd, *sigEven;
    sigEven = tmp;
    sigOdd = tmp + size / 2; 

    for(int i = 0; i < size / 2; i++)
    {
        sigEven[i] = v[2 * i];
        sigOdd[i] = v[2 * i + 1]; 
    }

    processInput(sigEven, size / 2, v, inputSig);
    processInput(sigOdd, size / 2, v, inputSig);

    //   merge the two half-length transforms, using xsubk^even + e^-jkPI/M * Xsubk^odd

    for(int k = 0; k < size / 2; k++)
    {
        w.Re    = inputSig[k];
        w.Im    = inputSig[k]; 
        z.Re    = w.Re * sigOdd[k].Re - w.Im * sigOdd[k].Im;
        z.Im    = w.Re * sigOdd[k].Im + w.Im * sigOdd[k].Re;
        v[k].Re = sigEven[k].Re + z.Re;
        v[k].Im = sigEven[k].Im + z.Im;
        v[k + size / 2].Re = sigEven[k].Re - z.Re;
        v[k + size / 2].Im = sigEven[k].Im - z.Im;
    }
}

void callback(AudioHandle::InputBuffer  in,
              AudioHandle::OutputBuffer out,
              size_t                    size)
{
    float nSig[4] = {0, 0, 0, 0};
    float x_out, y_out = 0;
    float inputData[N]; 
    complex scratch[N]; 

    for (int i = 0; i < N; i++) {
        inputData[i] = in[0][i];
        inputData[i] = 0.0f; 
    }

    processInput(v, N, scratch, inputData);

    // CALCULATE ONE SAMPLE OF THE SINE WAVE
    sine_osc = sinf(sine_phase);
    // saw_osc = -1.0f * (((sine_phase * (1.0f / TWOPI_F) * 2.0f)) - 1.0f);

    // call function to check folding parameters, update values accordingly
    updateParameters();

    // nSig[0] = (tanh((nPot[0]*5.0f) * sine_osc)/tanh((nPot[0]*5.0f)));
    nSig[0] = sine_osc;
    nSig[1] = (sinf(sine_phase * 2.0f) / 2.0f) * (nPot[0] * 1.5);
    nSig[2] = (sinf(sine_phase * 3.0f) / 3.0f) * (nPot[1] * 1.8);
    // nSig[2] = saw_osc * nPot[1];

    // nSig[3] = (sinf(sine_phase * 8.0f) / 4.0f ) * nPot[3];

    x_out = (nSig[0] + nSig[1] + nSig[2] + nSig[3]) / 2.6f;
    // We'll want to figure out some stereo effect here
    y_out = x_out;

    // PHASE ACCUMULATOR, INCREASE IN SETOSCFREQ FUNCTION
    sine_phase += sine_phase_acc;

    if(sine_phase > TWOPI_F)
    {
        sine_phase -= TWOPI_F;
        endC = true;
    }
    else
    {
        endC = false;
    }

    // endR = (sine_phase - sine_phase_acc < PI_F && sine_phase >= PI_F);

    for(size_t i = 0; i < size; i++)
    {
        out[0][i] = x_out;
        out[1][i] = y_out;
    }
}

void config_hw()
{
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(1);
    sample_rate = hw.AudioSampleRate();
}

void config_ADC()
{
    const int        adc_channels = 3;
    AdcChannelConfig adc_config[adc_channels];
    adc_config[0].InitSingle(A0); // hw.adc.Get(0) Frequency
    adc_config[1].InitSingle(A1); // hw.adc.Get(1) N0
    adc_config[2].InitSingle(A2); // hw.adc.Get(2) N1
    hw.adc.Init(adc_config, adc_channels);
    hw.adc.Start();
}
int prev_freq = 0; 
int main(void)
{
    config_hw();
    config_ADC();
    hw.StartAudio(callback);

    // hw.StartLog(true);
    

    while(1)
    {
        setOscFreq();
    }
}
