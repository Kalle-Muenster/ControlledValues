using System;
using System.Collections.Generic;
using Stepflow;

namespace Tests
{
    public class ElementController : Controlled.Float
    {
        private float LAST;

        public bool Cycled;
        public bool ForceUnsigned;
        public float peak;

        private float calcFunc(ref float value,ref float min, ref float max, ref float mov)
        {
            if (ForceUnsigned && value < 0)
                value = -value;

            mov = value - LAST;
            if (Cycled)
                mov = mov <= -((max - min) - 1)
                           ? 
                 mov + max : mov >= ((max-min) - 1)
                                 ?
                         mov-max : mov;

            LAST = value < min ? Cycled ? max - (min - value) : min
                 : value > max ? Cycled ? min + (value - max) : max
                 : value;

            peak = (mov > 0) ? LAST : peak;
            return value = LAST;
            
        }

        public ElementController() : base()
        {
            this.UseDelegateFunction(calcFunc);
        }
        public ElementController(ref float val) : base(ref val)
        {
            this.UseDelegateFunction(calcFunc);
        }

    }

    class Program
    {
  

        static void Main(string[] args)
        {
            
            Console.Write("set int value: ");
      
            string input = Console.ReadLine();
            float i = 0;
            float.TryParse(input, out i);

         //   Controlled.Float f = new Controlled.Float(ref i);

            ElementController c = new ElementController(ref i);
            

        //    c.Mode = Controlled.MODE.ControlElement;
            c.MIN = 0;
            c.MAX = 100;
            
            c.SetCheckAtGet(true);
            c.Cycled = false;
            c.ForceUnsigned = true;
            c.active = true;



            float temp = c;
            Console.WriteLine(c.Mode);
            Console.WriteLine(temp);
            Console.WriteLine(i);
            Console.WriteLine(c);
            Console.WriteLine("");

            Console.ReadLine();
        }
    }
}
