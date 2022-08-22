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



        public QuallenTest( MainWindow qualle, TestResults flags ) : base(flags)
        {
            positionX = 0;
            positionY = 0;
            measuredX = 0;
            measuredY = 0;
            timeout = TimeSpan.FromSeconds(2);
            testobject = qualle;

            privateApi = new Dictionary<string,MethodInfo>();
            privateApi.Add("controllerReConfiguration",qualle.GetType().GetMethod("controllerReConfiguration", BindingFlags.NonPublic|BindingFlags.Instance));

            AddTestCase("Object is Moving", QuallenTest_ObjectMovesByOwnIntention);
            AddTestCase("Switching to Sinus mode", QuallenTest_SwitchingControlModeToSinus);
            AddTestCase("Switching to Pulse mode", QuallenTest_SwitchingControlModeToPulse);
            AddTestCase("Switching to PingPong mode", QuallenTest_SwitchingControlModeToPingPong);
            AddTestCase("Switching to Delegates", QuallenTest_SwitchingControlModeToDelegate);
            
            AddTestCase("Ending the Test", QuallenTest_EndTestrun);
        }

        private void QuallenTest_EndTestrun()
        {
            InfoStep("Ending Testrun");
            testobject.beending = true;
        }

        private void QuallenTest_SwitchingControlModeToSinus()
        {
            Thread.Sleep(10000);
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            testobject.Lock();
            privateApi["controllerReConfiguration"].Invoke(testobject, new object[] { "Sinus" });
            testobject.Unlock();
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Change ControlMode to '{0}'", newMode);
            QuallenTest_ObjectMovesByOwnIntention();
            Thread.Sleep(5000);
        }

        private void QuallenTest_SwitchingControlModeToPulse()
        {
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            testobject.Lock();
            privateApi["controllerReConfiguration"].Invoke(testobject, new object[] { "Pulse" });
            testobject.Unlock();
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
            InfoStep( "Object is located at X:{0}, Y:{1}", measuredX, measuredY);

            positionX = measuredX - 50;
            positionY = measuredY - 50;
            ConTrol.Mouse( ConTrol.Move.Absolute, positionX, positionY );
            InfoStep("Mouse Pointer placed at X:{0}, Y:{1}", positionX, positionY);
            Thread.Sleep(250);

            testobject.Lock();
            privateApi["controllerReConfiguration"].Invoke(testobject, new object[] { "Follow" });
            testobject.Unlock();
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Change ControlMode to '{0}'", newMode);
            positionX -= 100;
            positionY -= 50;
            ConTrol.Mouse( ConTrol.Move.Absolute, positionX, positionY );
            InfoStep("Mouse Pointer moved to X:{0}, Y:{1}", positionX, positionY );

            QuallenTest_ObjectMovesByOwnIntention();

            Thread.Sleep(10000);

            measuredX = testobject.centerX;
            measuredY = testobject.centerY;
            int dist = Math.Abs(positionX - measuredX);
            CheckStep(dist < 200, "Object distance to mouse X {0} pixels", dist);
            dist = Math.Abs(positionY - measuredY);
            CheckStep(dist < 200, "Object distance to mouse Y {0} pixels", dist);
        }

        private void QuallenTest_SwitchingControlModeToPingPong()
        {
            Stepflow.ControlMode lastMode = testobject.movement.Mode;
            testobject.Lock();
            privateApi["controllerReConfiguration"].Invoke(testobject, new object[] { "PingPong" });
            testobject.Unlock();
            Stepflow.ControlMode newMode = testobject.movement.Mode;
            CheckStep(newMode != lastMode, "Change ControlMode to '{0}'", newMode);
            QuallenTest_ObjectMovesByOwnIntention();
            Thread.Sleep(5000);
        }

        private void QuallenTest_ObjectMovesByOwnIntention()
        {
            // prepare
            System.Windows.Point point = new System.Windows.Point( testobject.centerX, testobject.centerY );
            testobject.LocationChanged += Testobject_LocationChanged;
            positionX = measuredX = (int)point.X;
            positionY = measuredY = (int)point.Y;
            DateTime lastmeasure = DateTime.Now;

            // measure
            while( ( measuredX == positionX || measuredY == positionY ) && ( ( DateTime.Now - lastmeasure ) < timeout ) ) ;
                Thread.Sleep(100);
            testobject.LocationChanged -= Testobject_LocationChanged;

            // results
            int movement = positionX - measuredX;
            int distance = Math.Abs(movement);
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
