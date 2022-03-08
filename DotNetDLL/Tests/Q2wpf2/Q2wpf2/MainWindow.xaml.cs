//#define DINGENS_IS_ENABLED
#define USE_QUALLENSERVER

using System;
using System.Media;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows;
using Win32Imports;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Stepflow;
using System.Xml;
using System.Windows.Data;

namespace Qualle
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
	{
        [DllImport("user32.dll")]
        private static extern RETURN_CODE FindWindow(string className, string windowText);
#if DEBUG
        private Consola.StdStreams std = new Consola.StdStreams(Consola.CreationFlags.NewConsole);
#endif
        System.Windows.Automation.Peers.HostedWindowWrapper d;

		private System.Windows.Threading.DispatcherTimer dTimer1;
		private App App;
        private static Point Zero = new Point(0,0);
        private ControlledPoint movement;
        private ControlledPoint location;
        private ControlledPoint mousen;
        private Vector          mousenOffset;
        private bool            mouseFollow = false;
        private bool            mousenAvoid = false;

        private bool Xturn, Yturn;

        private Point screenScale;
        private Point usedScreenSize;
        private Point windowSize;

        public class Blubberer {
            private MediaElement media;
            private System.IO.Stream audio;
            public Blubberer( MediaElement wpfelement ) {
                media = wpfelement;
                media.MediaEnded += Media_Ended;
                media.Loaded += (object sender, RoutedEventArgs e) => {
                    (sender as MediaElement).Play();
                };
                media.IsEnabled = false;
                audio = null;
            }
            public void Blup() {
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
                Consola.StdStream.Out.WriteLine("blub!");
#endif
            }
            public void LoadFromRawData( byte[] rawfiledata )
            {
                audio = new MemoryStream( rawfiledata );
                if( media.IsEnabled ) {
                    media.Stop();
                    media.IsEnabled = false;
                } Blup();
            }
            public void LoadFromRawData( UnmanagedMemoryStream rawfiledata )
            {
                audio = rawfiledata;
                if( media.IsEnabled ) {
                    media.Stop();
                    media.IsEnabled = false;
                } Blup();
            }
            public void LoadFromLocation( Uri location )
            {
                audio = null;
                if( !media.IsEnabled ) {
                    media.IsEnabled = true;
                } media.Source = location;
            }
            private void Media_Ended(object sender, RoutedEventArgs e)
            {
                media.LoadedBehavior = MediaState.Pause;
                media.Position = new TimeSpan(0);
            }
        };

        public Blubberer bluber;

        public Point Size {
            get { windowSize.X = Width;
                  windowSize.Y = Height;
                  return windowSize; }
        } 
        public Int32Rect TheOtherSize {
            get { return new Int32Rect( (int)(screenScale.X * usedScreenSize.X * location.X),
                                        (int)(screenScale.Y * usedScreenSize.Y * location.Y),
                                        (int)(screenScale.X * Width),
                                        (int)(screenScale.Y * Height)); }
        }

		public Point position
        {
			get {
                return new Point( (usedScreenSize.X * location.X) - (Width / 2) ,
                                  (usedScreenSize.Y * location.Y) - (Height /2) );
            }
			private set {
                Point loc = new Point( (value.X + (Width / 2)) / usedScreenSize.X,
                                       (value.Y + (Height / 2)) / usedScreenSize.Y );
                location.VAL = loc;
                loc = position;
                Left = loc.X;
                Top = loc.Y;
            }
		}

        public Point Pivot {
            get{ return (Point)((Vector)Size/2); }
        }

        public Point Center {
            get { return (Vector)position + Pivot; }
        }

		private Random rand = new Random(System.DateTime.Now.Millisecond);
		private volatile int currentFrame=0;
		private volatile int counter=0;
		enum DIRECTION {LEFT=0,RIGHT=12,EXPLODING=24}
		private volatile DIRECTION direction = DIRECTION.LEFT;
        private BitmapImage[] bitmaps = new BitmapImage[36];
        private volatile int interval1;
        private volatile int interval2;
        private volatile bool showView;
      
        public MainWindow()
		{
            this.WindowStartupLocation = WindowStartupLocation.CenterScreen;
            uint dtList = FindWindow("Desktop", "Desktop");
            d = new System.Windows.Automation.Peers.HostedWindowWrapper(new IntPtr(dtList));
			InitializeComponent();
            interval1 = 250000;
            interval2 = 150 + (rand.Next(30));
			App = Qualle.App.Current as App;

            init( App.host, App.port, App.mode, App.maus, App.zoom );
#if DEBUG
            AllowsTransparency = false;
            WindowStyle = System.Windows.WindowStyle.ToolWindow;
            showView = false;
#else
            showView = true;
#endif
            showValuesView( !showView );
#if DINGENS_IS_ENABLED
            rdb_scaledFullScreen.IsChecked = false;
#else
            rdb_scaledFullScreen.IsEnabled = true;
#endif
            rdb_scaledScreen.IsChecked = true;
            chk_scaling.IsChecked = true;
            chk_mousen.IsChecked = false;
        }

        private DispatcherTimer startNewTimerThread(EventHandler action, int erval)
        {
            DispatcherTimer newTimer = new DispatcherTimer( new TimeSpan(erval),
                                                            DispatcherPriority.Render,
                                                            action, this.Dispatcher );
            newTimer.IsEnabled = true;
            newTimer.Start();
            return newTimer;
        }

        [Flags]
        enum ScalingFlags
        {
            Scaled = 1,
            Fullscreen = 2,
            Screen = 4,
            RealDisplayPixels = 8,
        }
        private void showValuesView( bool show )
        {
            if ( showView == show )
                return;
            Visibility state = show 
          ? Visibility.Visible 
          : Visibility.Collapsed;
            lbl_ix.Visibility = state;
            lbl_yps.Visibility = state;
            lbl_W.Visibility = state;
            lbl_H.Visibility = state;
            lbl_min.Visibility = state;
            lbl_max.Visibility = state;
            lbl_ORH.Visibility = state;
            lbl_ORV.Visibility = state;
            showView = show;
        }

        private void updateValueLabels()
        {
            lbl_ORH.Content = movement.X;
            lbl_min.Content = movement.Y;
            Point mov = movement.MOV;
            lbl_ORV.Content = mov.X;
            lbl_max.Content = mov.Y;
            lbl_ix.Content = Left;
            lbl_yps.Content = Top;

            if( mousen.ControllerActive ) {
                lbl_W.Content = mousen.X;
                lbl_H.Content = mousen.Y;
            } else {
                lbl_W.Content = location.X;
                lbl_H.Content = location.Y;
            }
        }

        private void screenReConfiguration( ScalingFlags scaling )
        {
            usedScreenSize = new Point();
            screenScale = SystemMetrics.SCREENSCALE;
            if ( scaling.HasFlag( ScalingFlags.Scaled ) ) {
                if ( scaling.HasFlag( ScalingFlags.Screen ) ) {
                    usedScreenSize = SystemMetrics.SCALEDSCREEN;
                } else if ( scaling.HasFlag( ScalingFlags.Fullscreen ) ) {
                    usedScreenSize = SystemMetrics.SCALEDFULLSCREEN;
                }
                screenScale.X = 1.0 / screenScale.X;
                screenScale.Y = 1.0 / screenScale.Y;
            } else {
                if ( scaling.HasFlag( ScalingFlags.Screen ) ) {
                    usedScreenSize = SystemMetrics.SCREEN;
                } else if ( scaling.HasFlag( ScalingFlags.Fullscreen ) ) {
                    usedScreenSize = SystemMetrics.FULLSCREEN;
                }
            } position = new Point( usedScreenSize.X / 2 - Pivot.X,
                                    usedScreenSize.Y / 2 - Pivot.Y );
        }

        private void controllerReConfiguration( string behavior )
        {
            if (Mouse.Captured == this)
                Mouse.Capture(null);

            if( mouseFollow = (behavior=="Follow") ) {
                this.CaptureMouse();
                mousen.ControllerActive = mousenAvoid = false;
            } 
            movement.ControllerActive = false;
            switch ( behavior )
            {
                case "Sinus":
                    movement.Mode = ControlMode.Sinus;
                    movement.MOV = new Point((8.0 / (usedScreenSize.X * 180.0)), (4.5 / (usedScreenSize.Y * 180.0)));
                    movement.MIN = new Point(-8 / usedScreenSize.X, -4.5 / usedScreenSize.Y);
                    movement.MAX = new Point(8 / usedScreenSize.X, 4.5 / usedScreenSize.Y);
                    break;
                case "PingPong":
                    movement.Mode = ControlMode.PingPong;
                    movement.MOV = new Point((16.0 / (usedScreenSize.X * 180.0)), (9.0 / (usedScreenSize.Y * 180.0)));
                    movement.MIN = new Point(-8 / usedScreenSize.X, -4.5 / usedScreenSize.Y);
                    movement.MAX = new Point( 8 / usedScreenSize.X, 4.5 / usedScreenSize.Y);
                    break;
                case "Pulse":
                    movement.Mode = ControlMode.Pulse;
                    movement.MOV = new Point((16.0 / (usedScreenSize.X * 180.0)), (9.0 / (usedScreenSize.Y * 180.0)));
                    movement.MIN = new Point(-8 / usedScreenSize.X, -4.5 / usedScreenSize.Y);
                    movement.MAX = new Point(8 / usedScreenSize.X, 4.5 / usedScreenSize.Y);
                    break;
                case "Follow": unsafe {
                    movement.UseDelegateFunction( iXfollow, YpsFollow );
                    movement.MOV = new Point((12.0 / (usedScreenSize.X * 1000.0)), (12.0 / (usedScreenSize.Y * 1000.0)));
                    movement.MIN = new Point(-6.0f / usedScreenSize.X, -6.0f / usedScreenSize.Y);
                    movement.MAX = new Point(6.0f / usedScreenSize.X, 6.0f / usedScreenSize.Y);
                  } break;
            } movement.SetReadOnly( true );
            movement.ControllerActive = true;
            if( mousenAvoid )
                MouseEnter += window_MouseEnter;
#if DEBUG
            std.Out.WriteLine(string.Format("switched movment behavior to mode: '{0}'",behavior));
#endif
        }

        private void setMousingEnabled(bool shouldBeMousable)
        {
            if ( ( mousenAvoid = ( shouldBeMousable && (!mouseFollow) ) ) ) {
#if DEBUG
                std.Out.WriteLine("Mousen Enabled!");
#endif
                this.MouseEnter += window_MouseEnter;
                this.MouseMove += window_MouseMove;
                this.MouseLeave += window_MouseLeave;
            } else {
#if DEBUG
                std.Out.WriteLine("Mousen Disabled!");
#endif
                this.MouseMove -= window_MouseMove;
                this.MouseEnter -= window_MouseEnter;
                this.MouseLeave -= window_MouseLeave;
            } mousen.ControllerActive = mousenAvoid;
        }

        private volatile string imagesourcelocation=null;
        private bool reloadImageSources()
        {
            if (imagesourcelocation != null) {
                for(int i=0;i<36;i++)
                    bitmaps[i] = new BitmapImage( new Uri(imagesourcelocation+"/images/Q2_" + i.ToString() + ".png") );
                bluber.LoadFromLocation( new Uri(imagesourcelocation + "/audio/blub.wav") );
                imagesourcelocation = null;
                return true;
            } else return false;
        }

        private async void setImageLocation(string imagelocation)
        {
            imagesourcelocation = imagelocation;

            dTimer1.Stop();
            await Dispatcher.InvokeAsync<bool>( reloadImageSources );
            dTimer1 = startNewTimerThread( dTimer1_Tick, interval1 );
        }

        private string QuallenserverAddress = null;
		public void init( string quallenServerHost, uint quallenServerPort, 
                          ControlMode movementMode, ControlMode mousenMode,
                          bool virtualScreenScale )
		{
            bool clamb = true;
            if( quallenServerHost == "" )
                QuallenserverAddress = "pack://application:,,,";
            else {
                QuallenserverAddress = "http://"+quallenServerHost+":"
                                     + quallenServerPort.ToString();
                XmlReader config = XmlReader.Create(new Uri(QuallenserverAddress+"/Config.xml").ToString());
                while( config.Read() ) {
                    if( config.IsStartElement("qualle") ) {
                        config = config.ReadSubtree();
                        while( config.Read() )
                            if( config.IsStartElement("config") ) {
                                Enum.TryParse<ControlMode>(config.GetAttribute("mode").ToString(), out movementMode);
                                bool.TryParse(config.GetAttribute("clamb").ToString(), out clamb);
                                switch(config.GetAttribute("maus").ToString()) {
                                    case "Follow": mousenMode = ControlMode.Delegate; break;
                                    case "Avoid": mousenMode = ControlMode.Element; break;
                                } break;
                            }
                    } break;     
                }
            }
            RETURN_CODE.AddErrorBase<MMSYSERR>(ERROR_BASES.MMSYSERR);
            RETURN_CODE.ActivLogging = true;
#if DEBUG
            RETURN_CODE.SetLogWriter( std.Err.WriteLine );
#endif
            RETURN_CODE result = RESULT.OK;
            int maximumTouches = SystemMetrics.MAXIMUMTOUCHES;
            usedScreenSize = SystemMetrics.SCALEDFULLSCREEN;
            screenScale = SystemMetrics.SCREENSCALE;
            screenScale.X = 1.0 / screenScale.X;
            screenScale.Y = 1.0 / screenScale.Y;
            screenSettings = virtualScreenScale 
                           ? ScalingFlags.Scaled|ScalingFlags.Fullscreen
                           : ScalingFlags.RealDisplayPixels|ScalingFlags.Fullscreen;

            lbl_W.Content = (uint)usedScreenSize.X;
            lbl_H.Content = (uint)usedScreenSize.Y;
            lbl_ix.Content = (uint)(usedScreenSize.X * screenScale.X);
            lbl_yps.Content = (uint)(usedScreenSize.Y * screenScale.Y);

            for (int i = 0; i < 36; i++) {
                bitmaps[i] = new BitmapImage(new Uri(QuallenserverAddress + "/images/Q2_" + i.ToString() + ".png"));
            } view.Source = bitmaps[0];
            Xturn = Yturn = false;

            movement = new ControlledPoint( movementMode );
            movement.MOV = new Point((16.0 / (usedScreenSize.X * 180.0)), (9.0 / (usedScreenSize.Y * 180.0)));
            movement.MIN = new Point(-8 / usedScreenSize.X, -4.5 / usedScreenSize.Y);
            movement.MAX = new Point(8 / usedScreenSize.X, 4.5 / usedScreenSize.Y);
            movement.VAL = Zero;
            movement.SetReadOnly( true );
            movement.ControllerActive = true;

            location = new ControlledPoint( ControlMode.Clamp );
            location.MIN = new Point(0.002,0.002);
            location.MAX = new Point(0.998,0.998);
            location.VAL = new Point(0.5, 0.5);
            location.MOV = new Point(1.0/usedScreenSize.X,1.0/usedScreenSize.Y);         
            location.ControllerActive = clamb;
            location.SetCheckAtSet();
           
            bluber = new Blubberer( blub );
            if( QuallenserverAddress.StartsWith( "pack" ) ) {
                bluber.LoadFromRawData( Properties.Resources.blub );
            } else {
                bluber.LoadFromLocation( new Uri(QuallenserverAddress+"/audios/blub.wav") );
            }

            cbx_resourceLocations.Content = QuallenserverAddress;
            dTimer1 = startNewTimerThread( dTimer1_Tick, interval1 );

            mousen = new ControlledPoint( Zero );
            mousen.SetUp( Zero, Size, Zero, (Point)((Vector)Size/2), ControlMode.Element );
            mousen.SetCheckAtSet();
            mousen.ControllerActive = false;

#if DEBUG
            std.Out.WriteLine( string.Format("\nStart up parameters: host={3},port={4},maus={1},mode={0},zoom={2}\n",
                               new string[] { movementMode.ToString(), mousenMode == ControlMode.Element ? "Avoid"
                                            : mousenMode == ControlMode.Delegate ? "Follow"
                                            : mousenMode.ToString(), virtualScreenScale.ToString(),
                                              quallenServerHost, quallenServerPort.ToString() } ) );
#endif
            setMousingEnabled( mousenMode == ControlMode.Element );
            controllerReConfiguration( mousenMode == ControlMode.Delegate 
                                     ? "Follow" : movementMode.ToString() );
        }

        private bool ganz = false;
		void dTimer1_Tick(object sender, EventArgs e)
		{
            ControlledPoint.ActiveState state = movement.Check();

            if ( ganz = !ganz )
            {
                if( ++currentFrame == 12 ) {
                    if( direction == DIRECTION.EXPLODING )
                        App.Current.Shutdown( 0 );
                    if( beending )
                        direction = DIRECTION.EXPLODING;
                    currentFrame = 0;
#if DEBUG
                    std.Out.WriteLine("X: " + lbl_ix.Content + ", Y: " + lbl_yps.Content);
#endif
                }
            }
            if (direction != DIRECTION.EXPLODING)
            {
                direction = ( movement.X + mousenOffset.X ) > 0 ? DIRECTION.RIGHT : DIRECTION.LEFT;

                if ( mousen.ControllerActive )
                { // if mousen active, prepare offsets from distance to the 
                 // mouse pointer to be added to the movement controllers
                    Point One = Pivot;
                    Vector mouseAvoiden = One - mousen;
                    Vector avoidenedDirection = new Vector( 
                        mouseAvoiden.X > 0 ? 1 : -1, 
                        mouseAvoiden.Y > 0 ? 1 : -1 
                    );
                    mouseAvoiden.X = Math.Abs(mouseAvoiden.X);
                    mouseAvoiden.Y = Math.Abs(mouseAvoiden.Y);
                    mousenOffset = One - (Point)mouseAvoiden;
                    mousenOffset.X *= (avoidenedDirection.X / 7500.0);
                    mousenOffset.Y *= (avoidenedDirection.Y / 7500.0);
                }
                else if (mousenOffset.X != 0 || mousenOffset.Y != 0)
                {
                    mousenOffset *= 0.95;
                }

                if ( ++counter >= interval2 ) {
                        counter = 0;

                        // check for turn point (means: that point where movement changes it's direction due to controller has oscillated across zero
                        // point) and decide by random if next oscillation cycle should go on by cycling same direction or to the opposide direction.
                        bool t = (movement.X + mousenOffset.X) < 0;
                        if ( t != Xturn ) {
                            Xturn = t;
#if DEBUG
                            double mov = movement['x'].MOV;
                            bool up = mov>0;
                            mov = Math.Abs(mov) * (rand.Next(0, 2) == 0 ? 1: -1);
                            if (mov > 0 != up)
                                std.Out.WriteLine("Movement 'X' changed direction!");
                            movement['x'].MOV = mov;
#else
                            movement['x'].MOV = Math.Abs(movement['x'].MOV) * (rand.Next(0, 2) == 0 ? 1 : -1);
#endif
                        }
                        t = (movement.Y + mousenOffset.Y) < 0;
                        if ( t != Yturn ) {
                            Yturn = t;
#if DEBUG
                            double mov = movement['y'].MOV;
                            bool up = mov > 0;
                            mov = Math.Abs(mov) * (rand.Next(0, 2) == 0 ? 1 : -1);
                            if (mov > 0 != up)
                                std.Out.WriteLine("Movement 'Y' changed direction!");
                            movement['y'].MOV = mov;
#else
                            movement['y'].MOV = Math.Abs(movement['y'].MOV) * (rand.Next(0, 2) == 0 ? 1 : -1);
#endif
                        }
                        movement['x'].MOV *= (movement.X < 0 ? 1.01 : 0.99);
                        movement['y'].MOV *= (movement.Y > 0 ? 1.01 : 0.99);
                }

                // assign this frame movements + maybe mousen offsets to the window's actual x/y location
                Left = (usedScreenSize.X * (location.X += movement.X + mousenOffset.X)) - (Width / 2);
                Top = (usedScreenSize.Y * (location.Y += movement.Y + mousenOffset.Y)) - (Height /2);

                if ( ganz ) {
                    if ( showView ) {
                        updateValueLabels();
#if DEBUG
                    } else {
                        lbl_ix.Content = (int)Left;
                        lbl_yps.Content = (int)Top;
                        Int32Rect tr = TheOtherSize;
                        lbl_W.Content = tr.X;
                        lbl_H.Content = tr.Y;
#endif
                    }
                }
            }
            view.Source = bitmaps[currentFrame + (int)direction];
            movement.ControllerActive = state;
		}

        ///
        ///////////////////////////////////////////////////////////////////////////////////////////////
        ///

        private void Image_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
            bluber.Blup();
            ContextMenu.IsOpen = true;
            mousen.ControllerActive = false;
            mousen.MOV = Zero;
		}

        private void schnell_Click(object sender, RoutedEventArgs e)
        {
            ContextMenu.IsOpen = false;

            dTimer1.Stop();
            interval2 = interval2 >= 30 ? interval2 - 10 : 20;
            dTimer1 = startNewTimerThread(dTimer1_Tick, interval1 -= 25000);            
        }

        private void lahmsam_Click(object sender, RoutedEventArgs e)
        {
            ContextMenu.IsOpen = false;

            dTimer1.Stop();
            interval2 = interval2 <= 190 ? interval2 + 10 : 200;
            dTimer1 = startNewTimerThread(dTimer1_Tick, interval1 += 25000);
        }

        private volatile bool beending = false;

        private void chk_scaling_Checked( object sender, RoutedEventArgs e )
        {
            if (((MenuItem)sender).IsChecked) {
                if(rdb_scaledScreen!=null)
                    rdb_scaledScreen.Visibility = Visibility.Visible;
                if(rdb_scaledScreen!=null)
                    rdb_scaledFullScreen.Visibility = Visibility.Visible;
            } else {
                if ( rdb_scaledScreen != null )
                    rdb_scaledScreen.Visibility = Visibility.Collapsed;
                if ( rdb_scaledScreen != null )
                    rdb_scaledFullScreen.Visibility = Visibility.Collapsed;
            } rdb_scaledScreen_Checked(sender, e);
        }

        private void chk_mousen_Clicked(object sender, RoutedEventArgs e) 
        {
            setMousingEnabled( ( sender as MenuItem ).IsChecked );
        }

        private void chk_showValues_Clicked( object sender, RoutedEventArgs e )
        {
            showValuesView( (sender as MenuItem).IsChecked );
        }

        private void btn_reloadImages_Click( object sender, RoutedEventArgs e )
        {
            setImageLocation( cbx_resourceLocations.Content as string );
        }

        private void window_MouseEnter(object sender, MouseEventArgs e)
        {
#if DEBUG
            std.Out.WriteLine("Trying to avoid Mousing!");
#endif
            mousen.VAL = e.GetPosition((Window)sender);
            mousen.MOV = Zero;
            mousen.ControllerActive = true;
            this.MouseEnter -= window_MouseEnter;
            this.MouseMove += window_MouseMove;
            this.MouseLeave += window_MouseLeave;
        }

        private void window_MouseLeave(object sender, MouseEventArgs e)
        {
            this.MouseLeave -= window_MouseLeave;
            if( mouseFollow )
                mousenAvoid = false;
            if( mousenAvoid ) {
#if DEBUG
                std.Out.WriteLine("Successfully avoided Mousing!");
#endif
                MouseEnter += window_MouseEnter; 
            }
            MouseMove -= window_MouseMove;
            mousen.ControllerActive = false;
            mousen.VAL = Pivot;
            mousen.MOV = Zero;
        }

        private void window_MouseMove(object sender, MouseEventArgs e)
        {
            if( mousen.ControllerActive || mouseFollow )
            { // check 'Wo Is Die Maus?'
                mousen.VAL = e.GetPosition((Window)sender);
            }
        }

        private double iXfollow(ref double val, ref double min, ref double max, ref double mov)
        {
            double dif = (
                Mouse.GetPosition(this).X
                - (this.Width / 2)
            );
            if ( dif > 0 ) {
                if ( dif > 20 )
                    val = max;
                else if ( dif > 5 )
                    val = max / 10;
                else if ( dif > 2 )
                    val = max / 100;
                else
                    val = 0;
            } else if( dif < 0 ) {
                if ( dif < -20 )
                    val = min;
                else if ( dif < -5 )
                    val = min / 10;
                else if ( dif < -2 )
                    val = min / 100;
                else
                    val = 0;
            } else val = 0;
            return val;
        }

        private double YpsFollow(ref double val, ref double min, ref double max, ref double mov)
        {
            double dif = (
                Mouse.GetPosition(this).Y 
                - (this.Height / 2)
            );
            if ( dif > 0 ) {
                if ( dif > 20 )
                    val = max;
                else if ( dif > 5 )
                    val = max / 10;
                else if ( dif > 2 )
                    val = max / 100;
                else
                    val = 0;
            } else if( dif < 0 ) {
                if ( dif < -20 )
                    val = min;
                else if ( dif < -5 )
                    val = min / 10;
                else if ( dif < -2 )
                    val = min / 100;
                else
                    val = 0;
            } else val = 0;
            return val;
        }

        private void ControllerSelectButton_Click(object sender, RoutedEventArgs e)
        {
            controllerReConfiguration( (sender as MenuItem).Name );
            ContextMenu.IsOpen = false;
        }

        private ScalingFlags screenSettings = 0;
        private void rdb_scaledScreen_Checked(object sender, RoutedEventArgs e)
        {
            MenuItem button = (sender as MenuItem);
            switch ( button.Uid )
            {
                case "ScreenScaling":
                    if ( button.IsChecked )
                        screenSettings |= ScalingFlags.Scaled;
                    else
                        screenSettings ^= ScalingFlags.Scaled;
                    ContextMenu.IsOpen = false;
                    break;
                case "Screen":
                    rdb_scaledFullScreen.IsChecked = !button.IsChecked;
                    if ( button.IsChecked ) {
                        screenSettings ^= ScalingFlags.Screen;
                        screenSettings |= ScalingFlags.Fullscreen;
                    } else {
                        screenSettings |= ScalingFlags.Scaled;
                        screenSettings ^= ScalingFlags.Fullscreen;
                    } break;
                case "Fullscreen":
                    rdb_scaledScreen.IsChecked = !button.IsChecked;
                    if ( button.IsChecked ) {
                        screenSettings |= ScalingFlags.Screen;
                        screenSettings ^= ScalingFlags.Fullscreen;
                    } else {
                        screenSettings ^= ScalingFlags.Scaled;
                        screenSettings |= ScalingFlags.Fullscreen;
                    }
                    break;
            } screenReConfiguration( screenSettings );
        }

        private void garnich_Click(object sender, RoutedEventArgs e)
        {
            ContextMenu.IsOpen = false;
            beending = true;
        }
	}
}
