using System;
using System.Media;
using System.IO;
using System.Windows;
using System.Windows.Controls;


namespace TestQualle
{
    public class QuallenBlubberer
    {
            private MediaElement media;
            private SoundPlayer  sound;

            public QuallenBlubberer( MediaElement wpfelement )
            {
                media = wpfelement;
                media.MediaEnded += Media_Ended;
                media.Loaded += Media_Loade;
                media.IsEnabled = false;
                media.LoadedBehavior = MediaState.Manual;
                sound = null;
            }

            private void prepareStream( Stream data )
            {
                sound = new SoundPlayer(data);
                sound.LoadCompleted += (object sender, System.ComponentModel.AsyncCompletedEventArgs e) => {
                     (sender as SoundPlayer).Play();
                };
                if (media.IsEnabled) {
                    media.Stop();
                    media.IsEnabled = false;
                }
                sound.Load();
            }

            private void prepareMedia( Uri location )
            {
                if( sound != null) {
                    sound.Stop();
                    sound.Dispose();
                    sound = null;
                }
                if ( !media.IsEnabled ) {
                    media.IsEnabled = true;
                } media.Source = location;
                Blup();
            }

            public void Blup()
            {
                if( sound == null ) {
                    media.Play(); ;
            } else {
                    sound.Play();
                }
            #if DEBUG
                Consola.StdStream.Out.WriteLine("played audio signal");
            #endif
            }

            public void LoadFromRawData( byte[] rawfiledata )
            {
                prepareStream( new MemoryStream(rawfiledata) );
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromRawData( UnmanagedMemoryStream rawfiledata )
            {
                prepareStream( rawfiledata );
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromRawData( Stream rawfiledata )
            {
                prepareStream( rawfiledata );
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromLocation( Uri location )
            {
                prepareMedia( location );
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Location Blubberer");
            #endif
            }

            private void Media_Loade( object sender, RoutedEventArgs e )
            {
                
                ( sender as MediaElement ).Play();
            }
            private void Media_Ended( object sender, RoutedEventArgs e )
            {
                
                media.Position = new TimeSpan(0);
            }
        };
}
