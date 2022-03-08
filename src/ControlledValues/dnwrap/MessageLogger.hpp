#ifndef _MessageLogger_hpp_
#define _MessageLogger_hpp_

namespace Stepflow 
{
    public ref class MessageLogger {
    public:
        delegate void LoggingDelegate( System::String^ stringContent );
        enum class Level : int {
            Garnich =  0,
            NurInfo =  1000,
            Wichtig = -1000,
            Schlimm =  2000,
        };
    private:
        static LoggingDelegate^ schlim = nullptr;
        static LoggingDelegate^ logger = nullptr;
        static MessageLogger^   single = nullptr;
        static Level             level = Level::Garnich;

    public:
        static void SetLogWriter( LoggingDelegate^ loggerFunction ) {
            logger = loggerFunction;
        }
        static void SetErrorWriter( LoggingDelegate^ errorFunction ) {
            schlim = errorFunction;
        }
        static void SetLoggingLevel( Level loglevel ) {
            if( loglevel == (Level::Schlimm|Level::Wichtig) ) logger = nullptr;
            level = loglevel;
        }
        static void LogAlleInfos() {
            SetLoggingLevel( Level::Schlimm );
        }
        static void LogNurSchlimmeFehler() {
            SetLoggingLevel( Level::Schlimm|Level::Wichtig );
        }
        static void logInfoWichtig( System::String^ formatString, ...array<System::Object^>^ argumentList ) {
            if( logger && level >= Level::NurInfo )
                logger( System::String::Format( formatString + "\n", argumentList ) );
            if( schlim && level != Level::Garnich )
                schlim( System::String::Format( formatString + "\n", argumentList ) );
        }
        static void logErrorSchlimm( System::String^ formatString, ...array<System::Object^>^ argumentList ) {
            if( schlim )
                schlim( System::String::Format( formatString + "\n", argumentList ) );
            else if ( logger )
                logger( System::String::Format( formatString + "\n", argumentList ) );
        }
        static MessageLogger^ Stream( System::Object^ output ) {
            if( logger )
                logInfoWichtig( output->ToString() );
            else if ( schlim && level == Level::NurInfo ) {
                logErrorSchlimm( output->ToString() );
            } if (!single) single = gcnew MessageLogger();
            return single;
        }
        static void logMessage( System::String^ formatString, ...array<System::Object^>^ arglist ) {
            if (logger)
                logInfoWichtig( formatString, arglist );
            else if (schlim && level == Level::NurInfo) {
                logErrorSchlimm( formatString, arglist );
            }
        }
        MessageLogger^ Out( System::Object^ output ) {
            if( logger )
                logger( output->ToString() );
            return this;
        }
    }; 
}

#endif
