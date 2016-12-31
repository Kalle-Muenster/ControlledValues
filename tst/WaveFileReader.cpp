/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileReader.cpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include "WaveFileReader.hpp"

using namespace stepflow;

Tests::WaveFileReader::WaveFileReader(const char* fileName)
    : whdr(getInitializedWaveheader())
{
    attachedBuffer = NULL;
    fopen_s(&f, fileName, "rb");
    canRead = true;
    fread_s(&whdr, 44, 44, 1, f);
    whdr.Subchunk2ID = whdr.DataSize;
    position = 0;
}

Tests::WaveFileReader::WaveFileReader(const char* fileName, void* buffer)
     : WaveFileReader(fileName)
{
    attachedBuffer = buffer;
}

Tests::WaveFileReader::~WaveFileReader(void)
{
    if(f)
    {
        fflush(f);
        fclose(f);
        f = nullptr;
    }
}


Tests::i32
Tests::WaveFileReader::Read(void* buffer, i32 samplesCount)
{
    return position+=fread_s (
        buffer, samplesCount*whdr.AudioFormat.BlockAlign,
        whdr.AudioFormat.BlockAlign, samplesCount, f
    );
}

Tests::Audio
Tests::WaveFileReader::Read(i32 frameCount)
{
    whdr.Subchunk2ID = frameCount ? frameCount * whdr.AudioFormat.BlockAlign
                                  : whdr.DataSize;

    frameCount = whdr.Subchunk2ID / whdr.AudioFormat.BlockAlign;

    Audio buffer(whdr,attachedBuffer);

    advance( fread_s( buffer.data, buffer.cbSize,
                         buffer.format.wf.nBlockAlign,
                         frameCount, f ) );

    return buffer.outscope();
}

byte
Tests::WaveFileReader::ReadByte(void)
{
    return advance(1) ? fgetc(f) : 0;
}

Tests::s16
Tests::WaveFileReader::ReadShort(void)
{
    return advance(2) ? fgetwc(f) : 0;
}

Tests::i32
Tests::WaveFileReader::ReadUnInt(void)
{
    if(advance(4)) {
        Frame.S16[0] = fgetwc(f);
        Frame.S16[1] = fgetwc(f);
    } return Frame.I32[0];
}

Tests::f32
Tests::WaveFileReader::ReadFloat(void)
{
    if(advance(4)) {
        Frame.S16[0] = fgetwc(f);
        Frame.S16[1] = fgetwc(f);
    } return Frame.F32[0];
}

Tests::i32
Tests::WaveFileReader::NextFrame(void)
{
    if(advance(whdr.AudioFormat.BlockAlign))
        for(int i = 0; i < whdr.AudioFormat.BlockAlign / 2; i++)
            Frame.S16[i] = fgetwc(f);
    return position;
}

Tests::WaveFileReader::audioFrame&
Tests::WaveFileReader::ReadFrame(void)
{
    NextFrame();
    return Frame;
}

bool
Tests::WaveFileReader::advance(i32 pos)
{
    position = min(position+pos, whdr.DataSize);
    if(!(canRead = position < whdr.DataSize))
        Frame.I32[0] = Frame.I32[1] = 0;
    return canRead;
}

Tests::i32
Tests::WaveFileReader::GetLength(void)
{
    return whdr.DataSize / whdr.AudioFormat.BlockAlign;
}

WAVEFORMATEX
Tests::WaveFileReader::GetHeader(void)
{
    return asWAVEFORMATEX(whdr.AudioFormat);
}

void
Tests::WaveFileReader::Seek(i32 seekPos)
{
    if(seekPos<whdr.DataSize)
        fseek(f, (position=seekPos) + 44, SEEK_SET);
}

Tests::i32
Tests::WaveFileReader::Open(const char* fileName)
{
    if(f)
        Close();
    fopen_s(&f, fileName, "rb");
    fread_s(&whdr, 44, 44, 1, f);
    whdr.Subchunk2ID = whdr.DataSize;
    position = 0;
    canRead = true;
    return whdr.DataSize;
}

Tests::i32
Tests::WaveFileReader::Close(void)
{
    if(f)
    {
        fflush(f);
        fclose(f);
        whdr.DataSize =
        canRead =
        f = NULL;
    }
    return position;
}


