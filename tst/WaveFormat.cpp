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
getInitializedWaveheader(void)
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
						i16 numberOfChannels, i32 sizeOfWaveData=0,
						WaveHeader* whdr = (WaveHeader*)&WaveHeaderInit) {
	 whdr->AudioFormat.NumChannels = numberOfChannels;
	 whdr->AudioFormat.SampleRate = samplingFrequency;
	 whdr->AudioFormat.BitsPerSample = bitsPerSample;
	 return calculateValues(whdr, sizeOfWaveData);
 }


Tests::WaveFileWriter::WaveFileWriter(void) 
	: whdr(WaveHeaderInit)
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

Tests::WaveFileWriter::WaveFileWriter(const char* fileName,Tests::Audio& dataChunk) 
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
//	if(attachedBuffer != nullptr)
//		delete attachedBuffer;
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
//	this->own = false;
//	return this[0];
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

