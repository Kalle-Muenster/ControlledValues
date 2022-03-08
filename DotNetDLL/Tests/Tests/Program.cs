using System;
using System.Collections.Generic;
using Stepflow;
using System.IO;
using Stepflow.Audio;

namespace Tests
{



    public unsafe class Program
    {
    //    public static TestConsole entry;

        public static void Main(string[] args)
        {
    //        new Q2wpf2.App().console = new Q2wpf2
            Console.Write("set float value: ");
      
            string input = Console.ReadLine();
            float i = 0;
            float.TryParse(input, out i);

            Controlled.Float c = new Controlled.Float(ControlMode.Element);
            c.SetTarget(ref i);
     
            //    c.Mode = Controlled.MODE.Moderate;
            c.MIN = 0;
            c.MAX = 100;
            
            c.SetCheckAtGet(true);
            *(bool*)c.GetPin((int)PIN.CYCLED).ToPointer() = true;
            *(bool*)c.GetPin((int)PIN.UNSIGNED).ToPointer() = true;
            c.active = true;


            Controlled.Float sample = new Controlled.Float();

            sample.Mode = ControlMode.Capacitor;

            sample.MIN = -1;
            sample.MAX = 1;
            sample.VAL = 0;
            sample.MOV = (1f / (44100f / 441f));
            sample.active = true;

            Console.WriteLine(sample.Mode);
            Console.WriteLine(sample.ToString());

            BinaryWriter writer = new BinaryWriter(new FileInfo("testOutputAudioFloat_16bitMono.raw").Create());
            for (int s = 0; i < 44100; i++)
                writer.Write((short)(short.MaxValue* sample.VAL));
            writer.Flush();
            writer.Close();

            float temp = c;
            Console.WriteLine(c.Mode);
            Console.WriteLine(c.ToString());
            Console.WriteLine(temp);
            c.VAL = 7;
            Console.WriteLine(i);
            Console.WriteLine(c);
            Console.WriteLine("");

            Console.ReadLine();

            Console.WriteLine("Testing by ToneGeneration");

            ToneGenerator.generateFile(Stepflow.Audio.FrameTypes.Au32bit6ch.type, "testtone.ton");

        }
    }
}
