using System;
using System.Collections.Generic;
using System.Windows.Threading;
using System.Threading.Tasks;
using Consola;
using Consola.Test;
using System.Windows;
using Mode = Stepflow.ControlMode;

namespace TestQualle
{
    public partial class App : Application
	{
        internal protected uint   port = 0;
        internal protected string host = "";
        internal protected bool   zoom = true;
        internal protected Mode   maus = Mode.None;
        internal protected Mode   mode = Mode.PingPong;

        internal protected TestResults test = TestResults.NONE;
        internal protected Test       task;
        internal protected MainWindow qualle;

        private Test testrunfunction()
        {
            return new TestQualle.QuallenTest(qualle, test).Run();
        }

        private void testrunner()
        {
            task = testrunfunction();
        }

        public void ApplicationStarted( MainWindow self )
        {
            if( test != TestResults.NONE ) {
                qualle = self;
                System.Threading.ThreadStart starter = new System.Threading.ThreadStart(testrunner);
                System.Threading.Thread thread = new System.Threading.Thread(starter);
                thread.SetApartmentState(System.Threading.ApartmentState.STA);
                thread.Start();
            }
        }

        private struct CmdArg
        {
            public bool        Valid;
            public string      Param;
            private string     value;
            public TestResults Tests;
            public string      Value {
                get {
                    return Tests == TestResults.NONE
                         ? value : Tests.ToString();
                }
            }

            private void Invalidate()
            {
                Param = null;
                value = null;
                Valid = false;
                Tests = TestResults.NONE;
            }

            public CmdArg( string raw )
            {
                Tests = TestResults.NONE;
                Param = null;
                value = null;

                if( Valid = raw.StartsWith("--") ) {
                    if( raw.Contains('=') ) {
                        string[] split = raw.Split('=');
                        Param = split[0].Replace("--", "");
                        if( split.Length > 1 )
                            value = split[1];
                        else Invalidate();
                    } else {
                        switch( raw ) {
                            case "--testrun": Tests = TestResults.TextOutput; Param = "test"; break;
                            case "--verbose": Tests = TestResults.Verbose; Param = "test"; break;
                            case "--xmllogs": Tests = TestResults.XmlOutput; Param = "test"; break;
                            default: Invalidate(); break;
                        }
                    }
                } else {
                    Invalidate();
                }
            }
        }

        private void Application_Startup( object sender, StartupEventArgs e )
        {
            test = TestResults.TextOutput;
            bool isTestrun = false;
            string[] args = e.Args;
            if( args.Length > 0 ) {
                for( int p = 0; p < args.Length; ++p ) {
                    CmdArg argum = new CmdArg( args[p] );
                    if( argum.Valid ) {
                        switch( argum.Param ) {
                            case "host":
                            host = argum.Value;
                            port = port > 0
                                 ? port : 5678;
                            break;
                            case "port":
                            uint.TryParse(argum.Value, out port);
                            host = host == ""
                                 ? Environment.GetEnvironmentVariable("COMPUTERNAME")
                                 : host;
                            break;
                            case "mode":
                            if( !Mode.TryParse(argum.Value, true, out mode) )
                                mode = Mode.PingPong;
                            break;
                            case "zoom":
                            bool.TryParse(argum.Value, out zoom);
                            break;
                            case "test":
                            isTestrun = true;
                            test |= argum.Tests;
                            break;
                            case "maus":
                            if( argum.Value.ToLower() == "follow" ) {
                                maus = Mode.Delegate;
                            } else if( argum.Value.ToLower() == "avoid" ) {
                                maus = Mode.Element;
                            }
                            break;
                        }
                    }
                }
            }

            CreationFlags flags = CreationFlags.TryConsole;
            if( !isTestrun ) {
                test = TestResults.NONE;
            } else {
                flags |= CreationFlags.CreateLog | CreationFlags.NoInputLog;
            }
            StdStream.Init(flags);
        }

        private void Application_Exit( object sender, ExitEventArgs e )
        {
            if( task is null ) return;
            switch( task.Results ) {
                case TestResults.PASS: { StdStream.Out.WriteLine("    ...all tests PASSED"); } break;
                case TestResults.FAIL: { StdStream.Err.WriteLine("    ...{0} tests FAILED", task.getFailures()); } break;
                case TestResults.NONE: {
                        string[] exceptions = task.getErrors();
                        StdStream.Err.WriteLine("    --- test CRASHED ---");
                        foreach( string msg in exceptions ) StdStream.Err.WriteLine(msg);
                        StdStream.Err.WriteLine("    ...{0} ERRORS", exceptions.Length);
                    }
                    break;
            }
        }
    }
}
