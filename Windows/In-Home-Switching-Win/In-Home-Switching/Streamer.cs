using ScpDriverInterface;
using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace InHomeSwitching
{
    internal class Streamer
    {
        // Properties
        public bool IsRunning { get; private set; }
        public string IPAddress { get; private set; }

        public event EventHandler<EventArgs> OnStreamerError = delegate { };

        public struct InputPkg
        {
            public ulong HeldKeys;
            public short LJoyX;
            public short LJoyY;
            public short RJoyX;
            public short RJoyY;
        }

        public enum InputKeys : ulong
        {
            A = 1,
            B = 1 << 1,
            X = 1 << 2,
            Y = 1 << 3,
            LS = 1 << 4,
            RS = 1 << 5,
            L = 1 << 6,
            R = 1 << 7,
            ZL = 1 << 8,
            ZR = 1 << 9,
            Plus = 1 << 10,
            Minus = 1 << 11,
            Left = 1 << 12,
            Up = 1 << 13,
            Right = 1 << 14,
            Down = 1 << 15
        }

        // Members
        private const string FFMPEGArgs = "-y -f rawvideo -pixel_format rgb32 -framerate 300 -video_size {0}x{1} -i pipe: -f h264 -vf scale=1280x720 -preset ultrafast -tune zerolatency -pix_fmt yuv420p -profile:v baseline -x264-params \"nal-hrd=cbr\" -b:v {2}M -minrate {2}M -maxrate {2}M -bufsize 2M tcp://{3}:2222 ";
        private static DesktopDuplicator desktopDuplicator;

        private int resX;
        private int resY;
        private int quality;

        private Thread renderThread;
        private Thread inputThread;

        private TcpClient client;
        private NetworkStream clientStream;

        private Process ffmpegProc;
        private ScpBus scp;
        private X360Controller ctrl = new X360Controller();

        public Streamer(TcpClient switchClient, int streamQuality)
        {
            client = switchClient;
            quality = streamQuality;
            IPAddress = ((IPEndPoint)client.Client.RemoteEndPoint).Address.ToString();

            scp = new ScpBus();
            scp.PlugIn(1);
        }

        public void Start()
        {
            if (IsRunning)
            {
                Stop();
            }

            IsRunning = true;

            var inputRef = new ThreadStart(InputLoop);
            inputThread = new Thread(inputRef);
            inputThread.Name = "Input";
            inputThread.Start();

            var renderRef = new ThreadStart(RenderLoop);
            renderThread = new Thread(renderRef);
            renderThread.Name = "Render";
            renderThread.Start();
        }

        public void Stop()
        {
            IsRunning = false;

            try { client.Close(); }
            catch { }

            try { ffmpegProc.Kill(); }
            catch { }

            if (inputThread != null)
            {
                inputThread.Join();
                inputThread = null;
            }

            if (renderThread != null)
            {
                renderThread.Join();
                renderThread = null;
            }

            resX = 0;
            resY = 0;
        }

        private void StopDueToError(Exception e)
        {
            if (IsRunning)
            {
                new Task(Stop).Start();

                // Todo: Do something with the actual exception (display message to user?)
                OnStreamerError(this, new EventArgs());
            }
        }

        private void StartFFMPEGStreaming()
        {
            DesktopFrame frame = null;

            while (IsRunning)
            {
                try
                {
                    frame = desktopDuplicator.GetLatestFrame();
                    break;
                }
                catch
                {
                    desktopDuplicator = new DesktopDuplicator(0);
                }
            }

            if (frame != null)
            {
                resX = frame.DesktopImage.Width;
                resY = frame.DesktopImage.Height;

                if (ffmpegProc != null)
                {
                    try { ffmpegProc.Kill(); }
                    catch { };
                }

                ffmpegProc = new Process()
                {
                    StartInfo = new ProcessStartInfo()
                    {
                        FileName = "ffmpeg.exe",
                        Arguments = string.Format(FFMPEGArgs, resX, resY, quality, IPAddress),
                        UseShellExecute = false,
                        RedirectStandardInput = true,
                        RedirectStandardOutput = true,
                        CreateNoWindow = true
                    }
                };

                ffmpegProc.Start();
            }
        }

        private unsafe void RenderLoop()
        {
            DesktopFrame frame = null;

            while (IsRunning)
            {
                try
                {
                    frame = desktopDuplicator.GetLatestFrame();
                }
                catch
                {
                    desktopDuplicator = new DesktopDuplicator(0);
                    continue;
                }

                if (frame == null) continue;

                if (resX != frame.DesktopImage.Width || resY != frame.DesktopImage.Height)
                {
                    StartFFMPEGStreaming();
                    continue;
                }

                var bData = frame.DesktopImage.LockBits(
                    new Rectangle(0, 0, frame.DesktopImage.Width, frame.DesktopImage.Height),
                    ImageLockMode.ReadWrite, frame.DesktopImage.PixelFormat);

                using (var ms = new UnmanagedMemoryStream(
                    (byte*)bData.Scan0.ToPointer(),
                    bData.Width * bData.Height * 4))
                {
                    try
                    {
                        ffmpegProc.StandardInput.BaseStream.Flush();
                        ms.CopyTo(ffmpegProc.StandardInput.BaseStream);
                    }
                    catch(Exception e)
                    {
                        StopDueToError(e);
                    }

                    frame.DesktopImage.UnlockBits(bData);
                }
            }
        }

        private void InputLoop()
        {
            while (IsRunning)
            {
                try
                {
                    clientStream = client.GetStream();
                    clientStream.ReadTimeout = 2000;
                }
                catch (Exception e)
                {
                    StopDueToError(e);
                }

                while (IsRunning)
                {
                    InputPkg pkg;

                    try
                    {
                        pkg = ReadPkg();
                    }
                    catch (Exception e)
                    {
                        StopDueToError(e);
                        break;
                    }

                    void map(InputKeys inkey, X360Buttons outkey)
                    {
                        if ((pkg.HeldKeys & (ulong)inkey) > 0)
                            ctrl.Buttons |= outkey;
                        else ctrl.Buttons &= ~outkey;
                    }

                    map(InputKeys.A, X360Buttons.B);
                    map(InputKeys.B, X360Buttons.A);
                    map(InputKeys.X, X360Buttons.Y);
                    map(InputKeys.Y, X360Buttons.X);

                    map(InputKeys.L, X360Buttons.LeftBumper);
                    map(InputKeys.R, X360Buttons.RightBumper);

                    map(InputKeys.LS, X360Buttons.LeftStick);
                    map(InputKeys.RS, X360Buttons.RightStick);

                    map(InputKeys.Plus, X360Buttons.Start);
                    map(InputKeys.Minus, X360Buttons.Back);

                    map(InputKeys.Up, X360Buttons.Up);
                    map(InputKeys.Down, X360Buttons.Down);
                    map(InputKeys.Left, X360Buttons.Left);
                    map(InputKeys.Right, X360Buttons.Right);

                    if ((pkg.HeldKeys & (ulong)InputKeys.ZL) > 0)
                        ctrl.LeftTrigger = byte.MaxValue;
                    else ctrl.LeftTrigger = byte.MinValue;

                    if ((pkg.HeldKeys & (ulong)InputKeys.ZR) > 0)
                        ctrl.RightTrigger = byte.MaxValue;
                    else ctrl.RightTrigger = byte.MinValue;

                    ctrl.LeftStickX = pkg.LJoyX;
                    ctrl.LeftStickY = pkg.LJoyY;

                    ctrl.RightStickX = pkg.RJoyX;
                    ctrl.RightStickY = pkg.RJoyY;

                    scp.Report(1, ctrl.GetReport());
                }
            }
        }

        private InputPkg ReadPkg()
        {
            var buf = new byte[0x10];
            var pkg = new InputPkg();

            clientStream.Read(buf, 0, 0x10);

            pkg.HeldKeys = BitConverter.ToUInt64(buf, 0);
            pkg.LJoyX = BitConverter.ToInt16(buf, 8);
            pkg.LJoyY = BitConverter.ToInt16(buf, 10);
            pkg.RJoyX = BitConverter.ToInt16(buf, 12);
            pkg.RJoyY = BitConverter.ToInt16(buf, 14);

            return pkg;
        }
    }
}