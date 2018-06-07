//|---------------------------------------------------------------------------
//|   DESCRIPTION
//|  IR wave builder is a tool to assemble .wav audiofiles which contain the
//|  neededed data to emulate a IR remote with two 940nm LEDs driven on the
//|  audio jsck of any device. The IR assembly is built upon the raw IR 
//|  information modulation. The code assumes as an input a signal divided in 
//|  timeunits of equal length. This contains a known number of carrier 
//|  frequency periods. (16 in Philips RC6). The whole code then binary encoded
//|  as a list of high(1) and low(0) timeunits.
//|---------------------------------------------------------------------------

#include <iostream>
#include <cmath>
#include "sndfile.hh"

int main (int argc, const char * argv[])
{
    //---------------------------Configuration-------------------------------
    const int sampleRate=44100; // Audio sample rate
    const int channels=2;       // The number of audio channels to produce
    const int cfreq=36000;      // The infrared carrier frequency
    const int pperunit=16;      // The carrier frequency period per time unit
    const int length=58;        // Total length of a code (in time units)
    const int multiplier=10;    // The amount of times the code should be repeated
    //---------------------------Configuration END---------------------------

    std::cout << "Type in your IR command (1=high, 0=low) \n";
    char command[length];
    std::cin >> command;
    std::cout << "Type in the command name (xxx.wav) \n";
    char outfilename[20];
    std::cin >> outfilename;
    // Prepare sample buffer array
    int size = sampleRate*(float(1)/cfreq)*pperunit*length;
    std::cout << "Creating a wave file with a length of " << size << "\n";
    float sample[size];
    // Put carrier frequency in array
    std::cout << "Storing carrier frequency\n";
    for (int i=0; i<size; i++) //put carrier frequency on whole signal
    {
        sample[i]=sin(float(i)/size*M_PI*cfreq*(float(size)/float(sampleRate)));
        //float(i)/size = factor >=0 & <=1 , last number halfwave multiplicator if==1 -> sine runs only from 0-pi
    }
    // modify carrier frequency
    std::cout << "modifying timeunits\n";
    for(int c=0; c<length; c++)
    {
        if(command[c] == '0') // if no carrier frequency
        {
            int samplepos_end = floor((c+1)*sampleRate*(float(1)/cfreq)*pperunit);
            for(int samplepos = floor(c*sampleRate*(float(1)/cfreq)*pperunit); samplepos<samplepos_end; samplepos++)
            {
                sample[samplepos] = 0;
            }
        }
    }
    // Copy channel to second channels
    float chan_sample[size*channels];
    for(int i=0; i<size; i++) //process every sample
    {
        if (channels==1)
        {
            chan_sample[i]=sample[i];
        }
        if (channels==2)
        {
            chan_sample[i*2]=sample[i];
            chan_sample[i*2+1]=sample[i];
        }
    }
    // Multiply whole buffer
    float multipl_sample[size*channels*multiplier];
    int x=0;
    int q=0;
    for(int i=1; i<=multiplier; i++)
    {
        while(x<size*channels*i)
        {
            multipl_sample[x] = chan_sample[q];
            q++;
            x++;
        }
        q=0;
    }
    // Write File
    std::cout << "writing file with " << sizeof(sample) <<"\n";
    const int format=SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    //  const int format=SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SndfileHandle file;
    file = SndfileHandle(outfilename, SFM_WRITE, format, channels, sampleRate);
    file.write(multipl_sample, size*channels*multiplier);
    
    return 0;
}

