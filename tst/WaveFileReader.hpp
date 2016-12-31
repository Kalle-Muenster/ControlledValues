/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveFileReader.hpp                         ||
||     Author:    unknown                                    ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _WaveFileReader_hpp_
#define _WaveFileReader_hpp_

#include "WaveFormat.hpp"

namespace stepflow
{
    namespace Tests
    {
        class WaveFileReader
        {
        private:
            FILE* f;
            i32 position;
            bool canRead;
            bool advance(i32);
            WaveHeader whdr;
            void* attachedBuffer;
            WaveFileReader(void);

        public:
            union audioFrame
            {
                f32 F32[2];
                i32 I32[2];
                s16 S16[4];
                i8 i8[8];
            }   Frame;
            WaveFileReader(const char* fileName);
            WaveFileReader(const char* fileName,void* buffer);
            virtual    ~WaveFileReader(void);
            i32         NextFrame(void);
            i32         GetLength(void);
            WAVEFORMATEX GetHeader(void);
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
}}//end of stepflow::Tests
#endif
