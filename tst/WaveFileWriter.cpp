/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileWriter.cpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "WaveFileWriter.hpp"

using namespace stepflow;

Tests::WaveFileWriter::WaveFileWriter(void)
    : whdr(getInitializedWaveheader())
{
    isOpen = false;
    fixedSize = 0;
    position = -1;
    f = nullptr;
    attachedBuffer = nullptr;
}


Tests::WaveFileWriter::WaveFileWriter( const char* fileName,
    i32 frequency, i16 bitDepth, i16 channels, i32 dataSize,void* buffer)
    : WaveFileWriter()
{
    fixedSize = dataSize==0?-1:dataSize;
    fopen_s(&f, fileName, "wb");
    CreateWaveHeader(frequency, bitDepth, channels, dataSize,&whdr);
    fwrite(&whdr, 44, 1, f);
    position = 0;
    attachedBuffer = buffer;
}

Tests::WaveFileWriter::WaveFileWriter(const char* fileName,
    i32 dataSize )
    : WaveFileWriter(fileName,44100,16,2,dataSize)
{}

Tests::WaveFileWriter::WaveFileWriter(Tests::Audio& dataChunk, const char* fileName)
    : Tests::WaveFileWriter::WaveFileWriter(fileName,
                                            dataChunk.format.wf.nSamplesPerSec,
                                            dataChunk.format.wBitsPerSample,
                                            dataChunk.format.wf.nChannels,
                                            dataChunk.cbSize) {
    attachedBuffer = dataChunk.data;
}

Tests::i32
Tests::WaveFileWriter::Write(void* data, i32 frameCount)
{
    i32 bytewrite = frameCount*whdr.AudioFormat.BlockAlign;
    if((position + bytewrite) > fixedSize)
        frameCount = (bytewrite = (fixedSize - position)) / whdr.AudioFormat.BlockAlign;
    fwrite(data, whdr.AudioFormat.BlockAlign, frameCount, f);
    if(fixedSize == (position += bytewrite))
        Close();
    return position;
}

Tests::i32
Tests::WaveFileWriter::Save(void)
{
    if(attachedBuffer)
        return Write(attachedBuffer,
        fixedSize / whdr.AudioFormat.BlockAlign);
    else
        return 0;
}

Tests::i32
Tests::WaveFileWriter::Write(s16 data)
{
    return (position += fputwc(data, f)) >= fixedSize ?
                                              Close() : position;
}
Tests::i32
Tests::WaveFileWriter::WriteFrame(void)
{
    fputwc(Frame.S16[0], f);
    fputwc(Frame.S16[1], f);
    return (position += 4) >= fixedSize ? Close() : position;
}
Tests::i32
Tests::WaveFileWriter::Write(i32 data)
{
    Frame.I32=data;
    return WriteFrame();
}
Tests::i32
Tests::WaveFileWriter::Write(float data)
{
    Frame.F32=data;
    return WriteFrame();
}
Tests::i32
Tests::WaveFileWriter::WriteFrame(s16 sample)
{
    for(int i = 0; i < whdr.AudioFormat.NumChannels; i++) {
        fputwc(sample, f);
        position += 2;
    }return position>=fixedSize ? Close() : position;
}
Tests::i32
Tests::WaveFileWriter::WriteFrame(f32 sample)
{
    for(int i = 0; i < whdr.AudioFormat.NumChannels; i++) {
        Frame.F32 = sample;
        fputwc(Frame.S16[0], f);
        fputwc(Frame.S16[1], f);
        position += 4;
    }return position>=fixedSize ? Close():position;
}

Tests::i32
Tests::WaveFileWriter::Open(const char* fileName, i32 dataSize)
{
    i32 lastDataSize = 0;
    if(isOpen)
        lastDataSize = Close();
    fopen_s(&f, fileName, "wb");
    whdr = *CreateWaveHeader( whdr.AudioFormat.SampleRate,
                              whdr.AudioFormat.BitsPerSample,
                              whdr.AudioFormat.NumChannels,
                              dataSize );
    fwrite(&whdr, 44, 1, f);
    position = 0;
    fixedSize = dataSize;
    isOpen = true;
    return lastDataSize;
}

Tests::i32
Tests::WaveFileWriter::Close(void)
{
    i32 writtenData = 0;
    if(f)
    {
        fflush(f);
        if(fixedSize == EMPTY)
        {
            writtenData = ftell(f) - CONSTS::HeaderSize;
            fseek(f, 40, SEEK_SET);
            fwrite(&writtenData, 4, 1, f);
        }
        else
        {
            writtenData = position = fixedSize - (writtenData = ftell(f) - CONSTS::HeaderSize);
            if(position) {
                void* dummy = (whdr.AudioFormat.BitsPerSample > 16) ?
                                         (void*)new f32[2]{0.0,0.0} : (void*)new s16[2]{0,0};
                while(position -= fwrite(&dummy, whdr.AudioFormat.BlockAlign, 1, f));
                delete dummy;
            }
        }
        fflush(f);
        fclose(f);
        f = NULL;
    }
    return writtenData;
}


Tests::WaveFileWriter::~WaveFileWriter(void)
{
    if(isOpen)
    {
        fflush(f);
        fclose(f);
        isOpen = false;
    }
    if(f != nullptr)
    {
        f = nullptr;
    }
//  if(attachedBuffer != nullptr)
//      delete attachedBuffer;
}

