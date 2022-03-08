//css_ref linkage\ControlledValues.dll
//css_ref linkage\WaveFileHandling.dll
//css_co /unsafe
#define COMPILE

using System;
using System.Reflection;
using System.Collections.Generic;
//using System.Threading.Tasks;

using Stepflow.Audio;
using Stepflow.Audio.FrameTypes;
using Stepflow.Audio.FileIO;

namespace Stepflow
{
    namespace Audio
    {
        public class ToneGenerator
        {
            private delegate byte TheForm(byte form);
            private delegate float TheFreq(float frq);

            private enum FORM : uint
            {
                non = (uint)ControlMode.None,
                sin = (uint)ControlMode.Sinus,
                saw = (uint)ControlMode.Cycle,
                sdn = (uint)ControlMode.Cycle,
                pls = (uint)ControlMode.Capacitor,
                tri = (uint)ControlMode.PingPong
            };

            private static Controlled.Double[] osc;
            //private static Circular buffer;
            private static WaveFileWriter writer;
            private static Panorama[][] mixer;
            private static float[][] freqs;
            private static float[][] amps;

            private static TheFreq[] freqFunc;
            private static TheForm[] formFunc;

            private static System.IO.StreamReader tonscript = null;
            private static string ReadNextParameter()
            {
                if (tonscript != null)
                {
                    string nextArg = tonscript.ReadLine();
                    Console.WriteLine(nextArg);
                    return nextArg;
                }
                else
                    return Console.ReadLine();
            }
            private static float[][] readSeveralFloats(string input)
            {
                return readSeveralFloats(input, 4);
            }
            private static float[][] readSeveralFloats(string input, int max_count)
            {
                char level = '\0';
                if (max_count > 1)
                    level = input.Contains("~")
                    ? '~' : input.Contains("x")
                    ? 'x' : '\0';

                switch (level)
                {
                    case '~':
                        {
                            string[] inputs = input.Split(level);
                            if (max_count == 2)
                            {
                                float[] t = readSeveralFloats(inputs[0], max_count)[0];
                                t[1] = readSeveralFloats(inputs[1], max_count)[0][1];
                                return new float[2][] { t, t };
                            }
                            return new float[2][]{
                            readSeveralFloats(inputs[0],max_count)[0],
                            readSeveralFloats(inputs[1],max_count)[0]
                        };
                        }
                        break;
                    case 'x':
                        {
                            string[] inputs = input.Split(level);
                            float A = readSeveralFloats(inputs[0], max_count)[0][0];
                            float B = readSeveralFloats(inputs[1], max_count)[0][0];
                            return (max_count == 2)
                            ? new float[2][] { new float[] { A, A }, new float[] { B, B } }
                            : new float[2][] { new float[] { A, B }, new float[] { A, B } };
                        }
                        break;
                    case '\0':
                        {
                            float[] vals = new float[2];
                            if (!Single.TryParse(input.Replace('.', ','), out vals[0]))
                                vals[0] = 0.5f;
                            vals[1] = max_count > 2 ? 0.5f : vals[0];
                            return new float[2][] { vals, vals };
                        }
                }
                return null;
            }

            private static double correctFreqForFORM(double freq, FORM form)
            {
                if (form == FORM.pls || form == FORM.tri)
                    return (freq + freq);
                //    else if(form==FORM.sin)
                //        return freq/2.0;
                return freq;
            }

            public static void showHelpScreen()
            {
                Console.WriteLine("\nSynopsys: ToneGenerator [SampleRate *44100*] [Options]");
                Console.WriteLine("\nOptions:  (*'s = default)\n");
                Console.WriteLine("    --[16*|32|64]bit                  :    datatype of output file");
                Console.WriteLine("    --[mono|stereo|quadro|5.1*|7.1]   :    output file channel constelation");
                Console.WriteLine("    --[single|double*]                :    synthesis precision (internal)");
                Console.WriteLine("    --[tonescript=<filename>]         :    parse synthetization parameters from");
                Console.WriteLine("                                           file (.ton) instead of reading stdin.\n\n");
            }


#if COMPILE
            public static void generateFile(AudioFrameType fmt, string script) { // string[] args) {
                string[] args = new string[] {
                    "44100",
                    "--" +fmt.BitDepth.ToString()+"bit",
                    fmt.ChannelCount==1?"--mono"
                   :fmt.ChannelCount==2?"--stereo"
                   :fmt.ChannelCount==4?"--quadro"
                   :fmt.ChannelCount==6?"--5.1"
                   :fmt.ChannelCount==8?"--7.1":"",
                    "--tonescript="+script
                };
#else
            public static void Main(string[] args) {
#endif
                PcmFormat format;
                format.SampleRate = 44100;
                format.BitsPerSample = 16;
                format.NumChannels = 6;
                string InternalPrecision = "single";

                // get output format parameters from given command line parameters:
#if DEBUG
                Console.Write("arguments:");
#endif
                for (int i = 0; i < args.Length; i++)
                {
                    if ((args[i] == "/?") || (args[i] == "-h") || (args[i] == "/h"))
                    {
                        showHelpScreen(); return;
                    }
#if DEBUG
                    Console.WriteLine(" " + i.ToString() + ": " + args[i]);
#endif
                    if (args[i].StartsWith("--"))
                    {
                        string currentArg = args[i].Substring(2);

                        if (currentArg.EndsWith("bit"))
                        {
                            ushort.TryParse(currentArg.Substring(0, 2),
                                             out format.BitsPerSample);
                        }
                        else
                        if (currentArg.StartsWith("tonescript"))
                        {
                            System.IO.FileInfo f = new System.IO.FileInfo(currentArg.Split('=')[1]);
                            if (!f.Exists)
                            {
                                showHelpScreen(); return;
                            }
                            tonscript = f.OpenText();
                        }
                        else
                            switch (currentArg)
                            {
                                case "mono": format.NumChannels = 1; break;
                                case "stereo": format.NumChannels = 2; break;
                                case "quadro": format.NumChannels = 4; break;
                                case "5.1": format.NumChannels = 6; break;
                                case "7.1": format.NumChannels = 8; break;
                                case "help": showHelpScreen(); return;
                                case "single":
                                case "double":
                                    InternalPrecision = currentArg;
                                    break;
                            }
                    }
                    else
                    if (!UInt32.TryParse(args[i], out format.SampleRate))
                    {
                        format.SampleRate = 44100;
                    }
                }

                format.BlockAlign = (ushort)(format.NumChannels * (format.BitsPerSample >> 3));
                format.ByteRate = format.SampleRate * format.BlockAlign;
                format.Tag = (ushort)(format.BitsPerSample >= 32 ? 3 : 1);


                // all following parameters will be requested by stdin...
                // (or even read from file defined by --tonescript=filename option):

                Console.WriteLine("enter filename for storing the wave: ");

                string outputFileName = ReadNextParameter();

                if (!outputFileName.EndsWith(".wav"))
                    outputFileName += ".wav";

                uint intput;
                Console.WriteLine("enter length of tone in ms: ");
                while (!UInt32.TryParse(ReadNextParameter(), out intput))
                    Console.WriteLine("gib zahlen ein du blödmann!");
                uint frameCount = (uint)(((double)format.SampleRate / 1000.0) * intput);

                Console.Write("enter number of oscillators: ");
                while (!UInt32.TryParse(ReadNextParameter(), out intput))
                    Console.WriteLine("gib zahlen ein du blödmann!");

                mixer = new Panorama[intput][];
                for (int i = 0; i < intput; i++) {
                    mixer[i] = new Panorama[2];
                }
                freqs = new float[intput][];
                osc = new Controlled.Double[intput];
                freqFunc = new TheFreq[intput];
                formFunc = new TheForm[intput];

                for (int i = 0; i < intput; i++)
                {
                    FORM form;
                    Console.WriteLine(
                        string.Format("enter oscillator {0} waveform <sin|saw|pls|tri>: ", i.ToString())
                                       );
                    form = (FORM)FORM.Parse(typeof(FORM), ReadNextParameter(), false);
                      //  Console.WriteLine("gib ne form an blödmann!");

                    Console.Write("enter position in field of sound <LR[xFR]>[~<LR[xFR]>]: ");
                    float[][] pannos = readSeveralFloats(ReadNextParameter());
                    mixer[i][0] = new Panorama(pannos[0]);
                    mixer[i][1] = new Panorama(pannos[1]);
                    mixer[i][1] = ((mixer[i][1] - mixer[i][0]) / frameCount);

                    Console.WriteLine("enter <start[~end]> frequency: ");
                    freqs[i] = readSeveralFloats(ReadNextParameter(), 2)[0];
                    Console.WriteLine("OSC-{0}: frq start {1}, end {2}", i, freqs[i][0], freqs[i][1]);

                    float initialfrequency = freqs[i][0];
                    for (int c = 0; c < freqs[i].Length; c++)
                    {
                        freqs[i][c] = (float)(2.0d / ((double)format.SampleRate / (double)freqs[i][c]));
                    }
                    if (freqs[i].Length == 1)
                    {
                        freqs[i] = new float[2] { freqs[i][0], freqs[i][0] };
                    }

                    osc[i] = new Controlled.Double();
                    osc[i].SetUp(-1, 1, correctFreqForFORM(freqs[i][0], form), 0, (ControlMode)form);

                    osc[i].SetCheckAtGet(true);
                    osc[i].active = true;
                    if (form == FORM.pls)
                    {
                        Console.WriteLine("preloading Capacitor!");
                        int oneCycle = (int)(((double)format.SampleRate / (double)initialfrequency) + 0.5d);
                        Console.WriteLine(string.Format("   ..by {0} energy units of value: {1}!", oneCycle, freqs[i][0]));
                        bool up = true;
                        for (int precycle = 0; precycle < oneCycle; precycle++)
                            if (up)
                            {
                                if (osc[i].VAL > 0)
                                {
                                    Console.WriteLine(string.Format("Capacitor fully charged at {0} pre-cycles being applied", precycle + 1));
                                    up = false;
                                }
                            }
                            else
                            {
                                if (osc[i].VAL < 0)
                                {
                                    Console.WriteLine(string.Format("Capacitor flushed at {0} pre-cycles being applied", precycle + 1));
                                    up = true;
                                }
                            }
                    }
                    freqs[i][0] = (float)((correctFreqForFORM(freqs[i][1], form) - osc[i].MOV) / (double)frameCount);
                    freqs[i][0] = form == FORM.sin
                                ? (float)(freqs[i][0] / 3.0f)
                                : form == FORM.saw
                                ? freqs[i][0]
                                : freqs[i][0];
                    Console.WriteLine("Osc-{0}: {3}Hz {1} stepping:{2}", i, form, freqs[i][0], initialfrequency);
                }

                generateWaveFile(outputFileName, format, frameCount, osc, freqs, mixer);
            }

            public static void generateWaveFile(string filename, PcmFormat format, uint frameCount, Controlled.Double[] oscilators, float[][] freqs, Panorama[][] mixer )
            {
                writer = new WaveFileWriter(filename, format);
                AudioFrameType frameFactory = writer.GetFrameType();

                Console.WriteLine(frameFactory.CreateEmptyFrame().GetType().ToString());
                Console.WriteLine(string.Format("begin writing... {2}channels {0}kHz {1}bit", format.SampleRate,
                                                 format.BitsPerSample, format.NumChannels));
                IFrame currentFrame;
                IFrame currentOscillator;

                for (uint frame = 0; frame < frameCount; frame++)
                {
                    currentFrame = frameFactory.CreateEmptyFrame();
                    for (int o = 0; o < osc.Length; o++)
                    {
                        currentOscillator = frameFactory.CreateEmptyFrame();

                        switch (format.BitsPerSample)
                        {
                            case 16: currentOscillator.Mix((short)(osc[o].VAL * 32760), mixer[o][0]); break;
                            case 32: currentOscillator.Mix((float)osc[o].VAL, mixer[o][0]); break;
                            case 64: currentOscillator.Mix((double)osc[o].VAL, mixer[o][0]); break;
                        }

                        currentOscillator.Amp((float)(1.0f / osc.Length));
                        currentFrame.Mix(currentOscillator, 0.5f);

                        double nichWechoptimieren = osc[o].MOV;
                        osc[o].MOV = (nichWechoptimieren + freqs[o][0]);
                        mixer[o][0] += mixer[o][1];
                    }
                    writer.Write(currentFrame);
                }
                Console.WriteLine(
                  string.Format("...Done!\n{0}ms {1}-channel {2}kHz-{3}bit audio written to file: {4}",
                                 writer.getWrittenTime(), writer.Format.NumChannels, writer.Format.SampleRate,
                                 writer.Format.BitsPerSample, writer.FileInfo.Name)
                                     );
                writer.Close();
            }
        };
    }
}