﻿using System;
using System.Windows;
using Stepflow;

using Mode = Stepflow.ControlMode;


namespace QuallenTest
{
    public partial class App : Application
	{
        internal protected uint   port = 0;
        internal protected string host = "";
        internal protected bool   zoom = true;
        internal protected Mode   maus = Mode.None;
        internal protected Mode   mode = Mode.PingPong;

        private struct CmdArg
        {
            public bool   valid;
            public string param;
            public string value;
            public CmdArg(string raw)
            {
                if( valid = ( raw.StartsWith("--") && raw.Contains('=') ) ) {
                    string[] split = raw.Split('=');
                    param = split[0].Replace("--", "");
                    value = split[1];
                } else {
                    param = null;
                    value = null;
                }
            }
        }

        private void Application_Startup( object sender, StartupEventArgs e )
        {
            string[] args = e.Args;
            if ( args.Length > 0 ) {
                for( int p = 0; p < args.Length; ++p ) {
                    CmdArg argum = new CmdArg(args[p]);
                    if ( argum.valid ) {
                        switch( argum.param ) {
                        case "host": host = argum.value;
                              port = port > 0 
                                   ? port : 5678;
                              break;
                        case "port": uint.TryParse( argum.value, out port );
                              host = host == ""
                                   ? Environment.GetEnvironmentVariable( "COMPUTERNAME" )
                                   : host;
                              break;
                        case "mode": if( !Mode.TryParse( argum.value, true, out mode ) )
                                        mode = Mode.PingPong;
                              break;
                        case "zoom": bool.TryParse( argum.value, out zoom );
                              break;
                        case "maus": if (argum.value.ToLower() == "follow") {
                                    maus = Mode.Delegate;
                                } else if (argum.value.ToLower() == "avoid") {
                                    maus = Mode.Element;
                            } break;
                        }
                    }
                }
            }
        }
    }
}
