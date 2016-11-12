/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFormat.cpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 24.09.2016                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#include "WaveFormat.hpp"
#include "../inc/StepLists.h"

using namespace stepflow;

static const Tests::WaveHeader
WaveHeaderInit = {
    Tests::CONSTS::ChunkID, Tests::CONSTS::ChunkSize, Tests::CONSTS::FileFormat,
    Tests::CONSTS::Subchunk1ID, Tests::CONSTS::Subchunk1Size, { Tests::CONSTS::PCMs16,
    2, 44100, 176400, 4, 16 }, Tests::CONSTS::Subchunk2ID, 0
};

Tests::WaveHeader
Tests::getInitializedWaveheader(void)
{ return *(Tests::WaveHeader*)&WaveHeaderInit; }

Tests::WaveHeader*
calculateValues(Tests::WaveHeader* whdr, Tests::s32 dataSize = -1)
{
    if( whdr->AudioFormat.BitsPerSample == 32 )
        whdr->AudioFormat.Tag = Tests::CONSTS::PCMf32;

    whdr->AudioFormat.BlockAlign = (Tests::i16)((whdr->AudioFormat.BitsPerSample / 8)*whdr->AudioFormat.NumChannels);
    whdr->AudioFormat.ByteRate = whdr->AudioFormat.SampleRate * whdr->AudioFormat.BlockAlign;
    whdr->DataSize = dataSize < 0 ? whdr->DataSize : (Tests::i32)dataSize;
    whdr->ChunkSize = whdr->DataSize + Tests::CONSTS::ChunkSize;
    return whdr;
}

Tests::s32
Tests::SetWaveDataSize(WaveHeader* whdr, s32 setTo = -1)
{
    if(setTo) {
        calculateValues(whdr, setTo - CONSTS::HeaderSize);
    } return whdr->ChunkSize - 8;
}

Tests::WaveHeader*
Tests::CreateWaveHeader(i32 samplingFrequency, i16 bitsPerSample,
                        i16 numberOfChannels, i32 sizeOfWaveData,
                        WaveHeader* whdr) {
     whdr->AudioFormat.NumChannels = numberOfChannels;
     whdr->AudioFormat.SampleRate = samplingFrequency;
     whdr->AudioFormat.BitsPerSample = bitsPerSample;
     return calculateValues(whdr, sizeOfWaveData);
 }



Tests::Audio::Audio(void)
{
    data = NULL;
}

Tests::Audio::Audio(Tests::Audio &copy)
{
    this->data = copy.data;
    this->cbSize = copy.cbSize;
    this->format = copy.format;
    this->frameCount = copy.frameCount;
    this->own = true;
}

Tests::Audio&
Tests::Audio::operator=(Tests::Audio& other)
{
    this->data = other.data;
    this->cbSize = other.cbSize;
    this->format = other.format;
    this->frameCount = other.frameCount;
    this->own = true;
    other.own = false;
    return other;
}

//Tests::Audio
//Tests::Audio::operator*(void)
//{
//  this->own = false;
//  return this[0];
//}

void
Tests::Audio::createBuffer(i32 size)
{
    data = new byte[cbSize=size];
    frameCount = cbSize / format.wf.nBlockAlign;
    own = true;
}

Tests::Audio::Audio(PCMWAVEFORMAT fmt, i32 sizeBytes)
     : Audio()
{
    own = false;
    format = fmt;
    createBuffer(sizeBytes);
}

Tests::Audio::Audio(WaveHeader hdr,void* bufferToWrap)
     : Audio()
{
    format = asPCMWAVEFORMAT(hdr.AudioFormat);
    createBuffer( hdr.Subchunk2ID==CONSTS::Subchunk2ID
                ? hdr.DataSize
                : hdr.Subchunk2ID );
}

Tests::Audio::Audio(i32 sRate, i32 bitDepth, i32 channels, i32 frameCount)
     : Audio()
{
    format = asPCMWAVEFORMAT(CreateWaveHeader(sRate, bitDepth, channels)->AudioFormat);
    createBuffer(frameCount*format.wf.nBlockAlign);
}

Tests::Audio::~Audio(void)
{
    if(own&&data)
        delete data;
}

void*
Tests::Audio::detachBuffer(void)
{
    void* tmp = data;
    data = NULL;
    return tmp;
}

Tests::Audio&
Tests::Audio::outscope(void)
{
    own = false;
    return this[0];
}



