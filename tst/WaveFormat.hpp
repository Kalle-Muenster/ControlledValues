/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFormat.hpp                             ||
||     Author:    Kalle                                      ||
||     Generated: 24.09.2016                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFormat_hpp_
#define _WaveFormat_hpp_
#include "Test.h"

#define asWAVEFORMATEX(audiofmt) (*(WAVEFORMATEX*)&audiofmt)
#define asPCMWAVEFORMAT(audiofmt) (*(PCMWAVEFORMAT*)&audiofmt)

namespace stepflow{
    namespace Tests{
        


        enum CONSTS : i32
        {
            ChunkID = 1179011410,       // Contains the letters "RIFF" in ASCII form //0x52494646,
            ChunkSize = 36,             // size of the entire file in bytes minus 8
            FileFormat = 1163280727,    // Contains the letters "WAVE" (0x57415645 big-endian form).
            Subchunk1ID = 544501094,    // Contains the letters "fmt " (0x666d7420 big-endian form).
            Subchunk1Size = 16,         // 16 for PCM. This is the size of the rest of the Subchunk which follows this number.
            Subchunk2ID = 1635017060,   // Contains the letters "data" (0x64617461 big-endian form).
            HeaderSize = 44,            // Header-size.
            PCMs16 = 1,                 // Values other than 1 indicate some form of compression.
            PCMf32 = 3                  // (or even uncompressed float PCM, which would be 3)
        };

        typedef struct AudioFormat
        {
            i16 Tag;           //2
            i16 NumChannels;   //4
            i32 SampleRate;    //8
            i32 ByteRate;      //12
            i16 BlockAlign;    //14
            i16 BitsPerSample; //16
        } AudioFormat;

        typedef struct WaveHeader
        {
            const i32   ChunkID;        //1-4
            i32         ChunkSize;      //5-8
            const i32   FileFormat;     //9-12
            const i32   Subchunk1ID;    //13-16
            i32         Subchunk1Size;  //17-20
            AudioFormat AudioFormat;    //21-36
            i32         Subchunk2ID;    //37-40
            i32         DataSize;       //41-44
            WaveHeader& operator = (WaveHeader& that)
                        { return *(WaveHeader*)memcpy(this, &that, 44); }
        } WaveHeader;
        
        

        s32         SetWaveDataSize(WaveHeader*, s32);
        WaveHeader* CreateWaveHeader( i32 samplingFrequency, 
                                      i16 bitsPerSample,
                                      i16 numberOfChannels, 
                                      i32 sizeOfWaveData, 
                                      WaveHeader* pHeader);
    
        struct Audio
        {
            PCMWAVEFORMAT   format;
            i32             cbSize;
            void*           data;
            i32             frameCount;
            bool            own;

                            Audio(void);
                            Audio(Audio& copy);
                            Audio(WaveHeader hdr, void* = NULL);
                            Audio(PCMWAVEFORMAT fmt, i32 size);
                            Audio( i32 sRate, i32 bitDepth, 
                                   i32 channels, i32 dataSize );
                           ~Audio(void);
            void            createBuffer(i32 size);
            void*           detachBuffer(void);
            Audio&          outscope(void);
            Audio&          operator=(Audio&);
        };

        class WaveFileWriter
        {
        private:
                        WaveFileWriter(void);
            FILE*       f;
            bool        isOpen;
            i32         position;
            i32         fixedSize;
            WaveHeader  whdr;
            bool        ownbuffer;
            void*       attachedBuffer;

        public: 
                        WaveFileWriter(const char* fileName,i32 dataSize = 0);
                        WaveFileWriter(const char* fileName,Audio& dataChunk);
                        WaveFileWriter( const char* fileName, i32 frequency,
                                        i16 bitDepth, i16 channels, i32 dataSize=0,
                                        void* dataBuffer = nullptr );                 
            virtual    ~WaveFileWriter(void);
            union audioFrame{
                   f32 F32;
                   i32 I32;
                   s16 S16[2];
                   i8  I8[4];
                  }     Frame;
            i32         Write(void* data, i32 framesCount);
            i32         Write(byte);
            i32         Write(s16);
            i32         Write(i32);
            i32         Write(f32);
            i32         WriteFrame(void);
            i32         WriteFrame(s16);
            i32         WriteFrame(f32);
            i32         Save(void);
            i32         Open(const char* fileName, i32 dataSize = 0);
            i32         Close(void);
        };

        class WaveFileReader
        {
        private:
                        WaveFileReader(void);
            FILE*       f;
            i32         position;
            bool        canRead;
            bool        advance(i32);
            WaveHeader  whdr;
            void*       attachedBuffer;

        public:
                        WaveFileReader(const char* fileName);
                        WaveFileReader(const char* fileName,void* buffer);
            virtual    ~WaveFileReader(void);
            union audioFrame{
                  f32 F32[2];
                  i32 I32[2];
                  s16 S16[4];
                  i8   I8[8];
                  }     Frame;
            i32         NextFrame(void);
            i32         GetLength(void);
            void        Seek(i32 position);
            i32         Read(void* buffer, i32 samplesCount);
            Audio       Read(i32=NULL);
            byte        ReadByte(void);
            s16         ReadShort(void);
            i32         ReadUnInt(void);
            f32         ReadFloat();
            audioFrame& ReadFrame(void);
            i32         Close(void);
            i32         Open(const char* fileName);
        };
    }//end of Tests
}
#endif
