#define USE_WITH_WPF

using Stepflow;
using System.Windows;
using DPOINT = System.Drawing.Point;
using WPOINT = System.Windows.Point;
#if   USE_WITH_WPF
using Point = System.Windows.Point;
#else
using Point = System.Drawing.Point;
#endif

namespace QuallenTest
{
    public class ControlledPoint 
    {
        public struct ActiveState {
            public bool activeX;
            public bool activeY;
            public ActiveState(bool x,bool y) {
                activeX = x;  activeY = y;
            }
            public static implicit operator ActiveState(bool cast) {
                return new ActiveState(cast, cast);
            }
            public static implicit operator bool(ActiveState cast) {
                return cast.activeX | cast.activeY;
            }
        };

        private Controlled.Float64 x, y;

        public double X
        {
            get { return x; }
            set { x.VAL = value; }
        }
        public double Y
        {
            get { return y.VAL; }
            set { y.VAL = value; }
        }

        public Point VAL
        {
            get { return new Point(x, y); }
            set {
                x.VAL = value.X;
                y.VAL = value.Y;
            }
        }

        public Point MIN
        {
            get { return new Point(x.MIN, y.MIN); }
            set { x.MIN = value.X; y.MIN = value.Y; }
        }
        public Point MAX
        {
            get { return new Point(x.MAX, y.MAX); }
            set { x.MAX = value.X; y.MAX = value.Y; }
        }
        public Point MOV
        {
            get { return new Point(x.MOV, y.MOV); }
            set { x.MOV = value.X; y.MOV = value.Y; }
        }

        public ControlMode Mode
        {
            get { return x.Mode; }
            set { x.Mode = value; y.Mode = value; }
        }

        public ActiveState ControllerActive
        {
            get { return new ActiveState( x.Active, y.Active ); }
            set { double get;
                if ( !value.activeX ) {
                    get = x.VAL;
                } x.Active = value.activeX;
                if ( !value.activeY ) {
                    get = y.VAL;
                } y.Active = value.activeY;
            }
        }
        public void SetReadOnly(bool shoulOnlyBeReadable)
        {
            x.SetReadOnly( shoulOnlyBeReadable );
            y.SetReadOnly( shoulOnlyBeReadable );
        }
        public void SetCheckAtGet()
        {
            x.SetCheckAtGet();
            y.SetCheckAtGet();
        }
        public void SetCheckAtSet()
        {
            x.SetCheckAtSet();
            y.SetCheckAtSet();
        }
        public void SetUp(Point min, Point max, Point mov, Point val, ControlMode mod)
        {
            x.SetUp( min.X, max.X, mov.X, val.X, mod );
            y.SetUp( min.Y, max.Y, mov.Y, val.Y, mod );
        }
        public Controlled.Float64 this[char index]
        {
            get { return index=='x'? x : y; }
            set {
                if (index=='y')
                    y.VAL = value;
                else
                    x.VAL = value;
            }
        }

        public ControlledPoint()
        {
            x = new Controlled.Float64(ControlMode.Sinus);
            y = new Controlled.Float64(ControlMode.Sinus);

            x.MAX = 1;
            x.MIN = -1;
            x.MOV = 0.001;
            x.VAL = 0;

            y.MAX = 1;
            y.MIN = -1;
            y.MOV = 0.001;
            y.VAL = 0;
            x.SetCheckAtGet();
            y.SetCheckAtGet();
        }
        public ControlledPoint( ControlledPoint copy )
        {
            x = copy['x'].Refer() as Controlled.Float64;
            y = copy['y'].Refer() as Controlled.Float64;
            x.VAL = copy.X;
            y.VAL = copy.Y;
        }
        public ControlledPoint( ref double wrap_x, ref double wrap_y )
        {
            x = new Controlled.Float64( ref wrap_x );
            y = new Controlled.Float64( ref wrap_y );
            x.Mode = ControlMode.Sinus;
            y.Mode = ControlMode.Sinus;
            x.SetCheckAtGet();
            y.SetCheckAtGet();
            x.Active = y.Active = true;
        }

        public ControlledPoint( ref Point wrap )
        {
            unsafe{fixed(void* p = &wrap) {
                x = new Controlled.Float64( ref ((double*)p)[0] );
                y = new Controlled.Float64( ref ((double*)p)[1] );
            }}
            x.Mode = ControlMode.Sinus;
            y.Mode = ControlMode.Sinus;
            x.SetCheckAtGet();
            y.SetCheckAtGet();
            x.Active = y.Active = true;
        }

        public ControlledPoint( ControlMode mode )
        {
            x = new Controlled.Float64(mode);
            y = new Controlled.Float64(mode);
            x.Active = y.Active = true;
        }

        public ControlledPoint(Point copy) : this()
        {
            x.VAL = copy.X;
            y.VAL = copy.Y;
            x.Active = y.Active = true;
        }

        public Controlled.Float64 cloneControllerX()
        {
            return x.Clone(); 
        }

        public Controlled.Float64 cloneControllerY()
        {
            return y.Clone();
        }

        public ActiveState Check()
        {
            return new ActiveState( x.Check(), y.Check() );
        }

        public static implicit operator Point( ControlledPoint cast )
        {
            return cast.VAL;
        }
        public static Point operator +( ControlledPoint This, Point That )
        {
            Point p = This.VAL;
            p.X += That.X;
            p.Y += That.Y;
            return p;
        }
        public static Vector operator -( ControlledPoint This, Point That )
        {
            return new Vector(This.x.VAL - That.X, This.y.VAL - That.Y);
        }
        public static Vector operator *( ControlledPoint This, double That )
        {
            return new Vector(This.x.VAL * That, This.y.VAL * That);
        }

        public static Point operator *( ControlledPoint This, Point That ) {
            Point p = This.VAL;
            p.X *= That.X;
            p.Y *= That.Y;
            return p;
        }

        public void UseDelegateFunction( Controlled.Float64.UserModeDelegate Delegate )
        {
            x.AttachedDelegate = Delegate;
            y.AttachedDelegate = Delegate;
        }

        public void UseDelegateFunction( Controlled.Float64.UserModeDelegate xDelegate,
                                         Controlled.Float64.UserModeDelegate yDelegate )
        {
            x.AttachedDelegate = xDelegate;
            y.AttachedDelegate = yDelegate;
        }
    }
}
