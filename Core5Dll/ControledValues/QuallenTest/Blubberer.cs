using System;
using System.Media;
using System.IO;
using System.Windows;
using System.Windows.Controls;


namespace QuallenTest
{
    public class QuallenBlubberer
    {
            private MediaElement media;
            private Stream       audio;
            public QuallenBlubberer( MediaElement wpfelement )
            {
                media = wpfelement;
                media.MediaEnded += Media_Ended;
                media.Loaded += (object sender, RoutedEventArgs e) => {
                    (sender as MediaElement).Play();
                };
                media.IsEnabled = false;
                audio = null;
            }
            public void Blup()
            {
                if( audio == null ) {
                    media.LoadedBehavior = MediaState.Play;
                } else {
                    audio.Seek( 0, SeekOrigin.Begin );
                    SoundPlayer oneshot = new SoundPlayer( audio );
                    oneshot.LoadCompleted += ( object sender, System.ComponentModel.AsyncCompletedEventArgs e )=> {
                        (sender as SoundPlayer).Play();
                    }; oneshot.Load();
                }
            #if DEBUG
                Consola.StdStream.Out.WriteLine("played audio signal");
            #endif
            }
            public void LoadFromRawData( byte[] rawfiledata )
            {
                audio = new MemoryStream( rawfiledata );
                if( media.IsEnabled ) {
                    media.Stop();
                    media.IsEnabled = false;
                } Blup();
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromRawData( UnmanagedMemoryStream rawfiledata )
            {
                audio = rawfiledata;
                if( media.IsEnabled ) {
                    media.Stop();
                    media.IsEnabled = false;
                } Blup();
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromRawData( Stream rawfiledata )
            {
                audio = rawfiledata;
                if( media.IsEnabled ) {
                    media.Stop();
                    media.IsEnabled = false;
                } Blup();
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Stream Blubberer");
            #endif
            }
            public void LoadFromLocation( Uri location )
            {
                audio = null;
                if( !media.IsEnabled ) {
                    media.IsEnabled = true;
                } media.Source = location;
            #if DEBUG
                Consola.StdStream.Out.WriteLine("Set Up as Location Blubberer");
            #endif
            }
            private void Media_Ended( object sender, RoutedEventArgs e )
            {
                media.LoadedBehavior = MediaState.Pause;
                media.Position = new TimeSpan(0);
            }
        };
}
