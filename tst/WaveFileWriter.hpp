/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileWriter.hpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFileWriter_hpp_
#define _WaveFileWriter_hpp_

#include "WaveFormat.hpp"

namespace stepflow
{
    namespace Tests
    {
        class WaveFileWriter
        {
        private:
            FILE* f;
            bool isOpen;
            i32 position;
            i32 fixedSize;
            WaveHeader whdr;
            WaveFileWriter(void);
            void* attachedBuffer;
            bool ownbuffer;

        public:
            union audioFrame{
            f32         F32;
            i32         I32;
            s16         S16[2];
            i8          i8[4];
            }           Frame;
                        WaveFileWriter(const char* fileName,i32 dataSize = 0);
                        WaveFileWriter(const char* fileName,i32 frequency,i16 bitDepth, i16 channels, i32 dataSize = 0,void* dataBuffer=nullptr);
                        WaveFileWriter(Audio& dataChunk, const char* fileName);
            virtual    ~WaveFileWriter(void);
            i32         Save(void);
            i32         Write(void* data, i32 framesCount);
            i32         Write(byte);
            i32         Write(s16);
            i32         Write(i32);
            i32         Write(f32);
            i32         WriteFrame(void);
            i32         WriteFrame(s16);
            i32         WriteFrame(f32);
            i32         Close(void);
            i32         Open(const char* fileName, i32 dataSize = 0);
        };

}}//end of stepflow::Tests
#endif
