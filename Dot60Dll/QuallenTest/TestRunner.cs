using System;
using System.Threading;
using System.Collections.Generic;
using Consola.Test;
using System.Reflection;
using QuallenTest;

namespace TestQualle
{
    public class QuallenTest : Test
    {
        private TimeSpan        timeout;
        private MainWindow      testobject;
        private volatile char   testaxis;
        private volatile int    positionX;
        private volatile int    positionY;
        private int             measuredX;
        private int             measuredY;

        private Dictionary<string,MethodInfo> privateApi;

        private bool ReflectPrivateMethod( string method )
        {
            MethodInfo funcptr = testobject.GetType().GetMethod( method, BindingFlags.NonPublic | BindingFlags.Instance );
            if( funcptr != null ) { privateApi.Add(method, funcptr); return true; }
            return false;
        }

        private void CallPrivat( string methodname, params object[] parmeters )
        {
            testobject.Lock();
            privateApi[methodname].Invoke(testobject, parmeters);
            testobject.Unlock();
        }

        public QuallenTest( MainWindow qualle, TestResults flags ) : base(flags)
        {
            privateApi = new Dictionary<string, MethodInfo>();
            positionX = 0;
            positionY = 0;
            measuredX = 0;
            measuredY = 0;
            timeout = TimeSpan.FromSeconds(2);
            testobject = qualle;

            ReflectPrivateMethod("controllerReConfiguration");


            AddTestCase("Object is Moving", QuallenTest_ObjectMovesByOwnIntention);
            AddTestCase("Switching to Sinus mode", QuallenTest_SwitchingControlModeToSinus);
            AddTestCase("Switching to Pulse mode", QuallenTest_SwitchingControlModeToPulse);
            AddTestCase("Switching to PingPong mode", QuallenTest_SwitchingControlModeToPingPong);
            AddTestCase("Switching to Delegates", QuallenTest_SwitchingControlModeToDelegate);

            AddTestCase("Ending the Test", QuallenTest_EndTestrun);
        }

        protected override void OnStartUp()
        {
            TimesStamps = true;
        }

        private void QuallenTest_EndTestrun()
        {
            StepInfo("Ending Testrun");
            testobject.beending = true;
        }

        private void QuallenTest_SwitchingControlModeToSinus()
        {
            Thread.Sleep(10000);
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            CallPrivat("controllerReConfiguration", "Sinus");
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Change ControlMode to '{0}'", newMode);
            QuallenTest_ObjectMovesByOwnIntention();
            Thread.Sleep(5000);
        }

        private void QuallenTest_SwitchingControlModeToPulse()
        {
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            CallPrivat("controllerReConfiguration", "Pulse");
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Change ControlMode to '{0}'", newMode);
            QuallenTest_ObjectMovesByOwnIntention();
            Thread.Sleep(5000);
        }

        private void QuallenTest_SwitchingControlModeToDelegate()
        {
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            measuredX = testobject.centerX;
            measuredY = testobject.centerY;
            StepInfo("Object is located at X:{0}, Y:{1}", measuredX, measuredY);

            positionX = measuredX - 30;
            positionY = measuredY - 30;
            ConTrol.Mouse(ConTrol.Move.Absolute, positionX, positionY);
            StepInfo("Mouse Pointer placed to X:{0}, Y:{1}", positionX, positionY);
            Thread.Sleep(250);

            CallPrivat("controllerReConfiguration", "Follow");
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Changed ControlMode to '{0}'", newMode);
            positionX -= 200;
            positionY -= 100;
            ConTrol.Mouse(ConTrol.Move.Absolute, positionX, positionY);
            StepInfo("Mouse Pointer placed at X:{0}, Y:{1}", positionX, positionY);

            QuallenTest_ObjectMovesByOwnIntention();

            Thread.Sleep(10000);

            measuredX = testobject.centerX;
            measuredY = testobject.centerY;

            int dist = Math.Abs( positionX - measuredX );
            CheckStep(dist < 200, "Object distance to mouse X {0} pixels", dist);
            dist = Math.Abs(positionY - measuredY);
            CheckStep(dist < 100, "Object distance to mouse Y {0} pixels", dist);
        }

        private void QuallenTest_SwitchingControlModeToPingPong()
        {
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            CallPrivat("controllerReConfiguration", "PingPong");
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Changed ControlMode to '{0}'", newMode);
            QuallenTest_ObjectMovesByOwnIntention();
            Thread.Sleep(5000);
        }

        private void QuallenTest_ObjectMovesByOwnIntention()
        {
            // prepare
            System.Windows.Point point = new System.Windows.Point( testobject.centerX, testobject.centerY );
            positionX = measuredX = (int)point.X;
            positionY = measuredY = (int)point.Y;
            DateTime lastmeasure = DateTime.Now;

            // measure
            testobject.LocationChanged += Testobject_LocationChanged;
            do {
                Thread.Sleep(200);
            } while( ( measuredX == positionX || measuredY == positionY ) && ( ( DateTime.Now - lastmeasure ) < timeout ) );
            testobject.LocationChanged -= Testobject_LocationChanged;

            // results
            int movement = positionX - measuredX;
            int distance = Math.Abs( movement );
            CheckStep(distance > 0, "Object has moved {0} pixels {1}", distance, movement == 0 ? string.Empty : movement < 0 ? "LEFT" : "RIGHT");

            movement = positionY - measuredY;
            distance = Math.Abs(movement);
            CheckStep(distance > 0, "Object has moved {0} pixels {1}", distance, movement == 0 ? string.Empty : movement < 0 ? "UP" : "DOWN");
        }

        private void Testobject_LocationChanged( object sender, EventArgs e )
        {
            MainWindow  obj = sender as MainWindow;
            positionX = obj.centerX;
            positionY = obj.centerY;
        }
    }
}
