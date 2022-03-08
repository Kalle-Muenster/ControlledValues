#if DEBUG
 #define LOG_ALL
#else
 #define RELEASE
#endif
/*
#if RELEASE
 #define THROW_EXCEPTIONS
#endif
*/
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Win32Imports
{
    public enum CALLBACK : uint
    {
        NULL = 0x00000000,    /* no callback */
        WINDOW = 0x00010000,    /* dwCallback is a HWND */
        TASK = 0x00020000,    /* dwCallback is a HTASK */
        FUNCTION = 0x00030000,    /* dwCallback is a FARPROC */
        THREAD = TASK,          /* thread ID replaces 16 bit task */
        EVENT = 0x00050000,    /* dwCallback is an EVENT Handle */
        TYPEMASK = 0x00070000,    /* callback type mask */
    }

    public enum ERROR_BASES
    {
        RESULT_OK = 0,
        MMSYSERR = 1,
        WAVERR = 32,
        MIDIERR = 64,
        TIMERR = 96,
        JOYERR = 160,

        MCIERR = 256,
        MIXERR = 1024,
        MCI_STRING_OFFSET = 512,
        MCI_VD_OFFSET = 1024,
        MCI_CD_OFFSET = 1088,
        MCI_WAVE_OFFSET = 1152,
        MCI_SEQ_OFFSET = 1216,
    }

    public enum MMSYSERR : int
    {
        NOERROR = 0,
        ERROR = ERROR_BASES.MMSYSERR,
        BADDEVICEID,
        NOTENABLED,
        ALLOCATED,
        INVALHANDLE,
        NODRIVER,
        NOMEM,
        NOTSUPPORTED,
        BADERRNUM,
        INVALFLAG,
        INVALPARAM,
        HANDLEBUSY,
        INVALIDALIAS,
        BADDB,
        KEYNOTFOUND,
        READERROR,
        WRITEERROR,
        DELETEERROR,
        VALNOTFOUND,
        NODRIVERCB,
        MOREDATA
    }

    public enum RESULT : byte
    {
        OK = 0x00,
        INFO = 0x01,
        WARNING = 0x02,
        ERROR = 0x03
    }

    [ StructLayout( LayoutKind.Explicit, Size = 8, Pack = 1) ]
    public struct RETURN_CODE
    {
        [FieldOffset(0)]
        public RESULT result;

        [FieldOffset(0)]
        public UInt32 u32;

        [FieldOffset(1)]
        public Int32 i32;

        [FieldOffset(0)]
        public UInt64 u64;

        private static SortedDictionary<uint, Enum> EnumValueLists;
        public delegate RETURN_CODE ErrorHandler( RETURN_CODE error );
        public delegate void ResultLogger( string logentry );

        public static ErrorHandler OnError;
        private static bool logAll = false;
        private static bool logErrors = false;
        private static ResultLogger logwriter;

        public static bool ActivLogging {
            get { return logAll || logErrors; }
            set { logErrors = value;
                logAll = value ?
                        logAll : false; }
        }
        public static bool LogAnyResult {
            get { return logAll; }
            set { logAll = value; }
        }
        private static void activeCheck( RETURN_CODE This ) {
            if ( ActivLogging ) {      
                if ( logAll )
                    This.log();
                else
                    This.logError();
            }
        }
        private static void AddErrorEnum( Type enumType, uint errorBase ) {
            if ( EnumValueLists.ContainsKey(errorBase) )
                return;
            Enum[] values = new Enum[Enum.GetValues(enumType).Length];
            Enum.GetValues(enumType).CopyTo(values, 0);
            int startIndex = Enum.IsDefined(enumType, Enum.ToObject(enumType, 0)) ? 1 : 0;
            EnumValueLists.Add(errorBase, values[startIndex]);
        }

        public static void SetLogWriter( ResultLogger loggerfunction ) {
            logwriter = loggerfunction;
        }
        public static void SetErrorHandler( ErrorHandler errorFunction ) {
            OnError = errorFunction;
        }

        public ERROR_BASES ERROR_BASE {
            get { uint get = 0;
                foreach ( uint key in EnumValueLists.Keys ) {
                    if ( key > this.u32 )
                        return (ERROR_BASES)get;
                    get = key;
                } return 0;
            }
        }

        public Enum ERROR_VALUE {
            get { return Enum.ToObject(EnumValueLists[(uint)ERROR_BASE].GetType(), u32) as Enum; }
        }

        public RETURN_CODE( RETURN_CODE copy ) : this() {
            u64 = copy.u64;
            activeCheck(this);
        }
        public RETURN_CODE( UInt32 init ) : this() {
            u32 = init;
            activeCheck(this);
        }
        public RETURN_CODE( Boolean init ) : this() {
            result = init ? RESULT.OK : RESULT.ERROR;
            activeCheck(this);
        }
        public RETURN_CODE( RESULT init ) : this() {
            result = init;
            activeCheck(this);
        }
        public RETURN_CODE( Int32 init ) : this() {
            result = RESULT.OK;
            i32 = init;
            activeCheck(this);
        }
        public RETURN_CODE( UInt64 init ) : this() {
            u64 = init;
            activeCheck(this);
        }

        public static implicit operator RESULT( RETURN_CODE cast ) {
            return cast.result;
        }
        public static implicit operator bool( RETURN_CODE cast ) {
            return cast.result == RESULT.OK;
        }
        public static implicit operator UInt32( RETURN_CODE cast ) {
            return cast.u32;
        }
        public static implicit operator string( RETURN_CODE cast ) {
            return cast.ToString();
        }
        public static implicit operator Enum( RETURN_CODE cast ) {
            return cast.ERROR_VALUE;
        }

        public static implicit operator RETURN_CODE( UInt32 cast ) {
            return new RETURN_CODE(cast);
        }
        public static implicit operator RETURN_CODE( Int32 cast ) {
            return new RETURN_CODE(cast);
        }
        public static implicit operator RETURN_CODE( UInt64 cast ) {
            return new RETURN_CODE(cast);
        }
        public static implicit operator RETURN_CODE( Boolean cast ) {
            return new RETURN_CODE(cast);
        }
        public static implicit operator RETURN_CODE( RESULT cast ) {
            return new RETURN_CODE(cast);
        }


        public override string ToString() {
            if ( result == RESULT.OK )
                return "RESULT.OK";
            else
                return ERROR_BASE.ToString() + "." + ERROR_VALUE.ToString();
        }

        public RETURN_CODE CheckForError() {
            if ( OnError != null )
                return OnError(this);
            else
                return logError();
        }

        public RETURN_CODE logError() {
            if ( result != RESULT.OK ) {
#if THROW_EXCEPTIONS
                throw new Exception(
#else
                logwriter(
#endif
                string.Format( 
                    "{1} - {0}", this.u32.ToString(),
                this.ToString() ) );
            } return this;
        }

        public RETURN_CODE logError( string someText ) {
            if ( result != RESULT.OK ) {
#if THROW_EXCEPTIONS
                throw new Exception(
#else
                logwriter(
#endif
                string.Format("{2}: {0} - {1}",
                    this.u32.ToString(), this.ToString(),
                     someText) );
            } return this;
        }

        public RETURN_CODE log() {
            logwriter( string.Format("{0} - {1}",
                       this.u32.ToString(),
                       this.ToString() ) );
            return this;
        }

        public RETURN_CODE log( string someTextToLog ) {
            logwriter( string.Format( "{2}: {0} - {1}",
                       this.u32.ToString(), this.ToString(),
                       someTextToLog ) );
            return this;
        }

        public eT To<eT>() where eT : struct {
            eT value;
            Enum.TryParse<eT>(this.u32.ToString(), out value);
            return value;
        }

        public static void AddErrorBase<eT>( ERROR_BASES errorBase ) {
            AddErrorEnum(typeof(eT), (uint)errorBase);
        }
        public static void AddErrorBase<eT>( uint errorBase ) {
            AddErrorEnum(typeof(eT), errorBase);
        }
        static RETURN_CODE() {
            EnumValueLists = new SortedDictionary<uint, Enum>();
            AddErrorBase<RESULT>((uint)0);
            AddErrorBase<MMSYSERR>(ERROR_BASES.MMSYSERR);
            logwriter = Console.WriteLine;
#if LOG_ALL
            logAll = true;
#else
            logAll = false;
#endif
            logErrors = true;

        }
    }
}

