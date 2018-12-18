using System;
using System.Collections.Concurrent;
using System.ComponentModel;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace InHomeSwitching
{
    internal class Server
    {
        // Properties
        public bool IsRunning { get; private set; }
        public int Port { get; private set; }
        public int Quality { get; private set; }
        public int ClientCount
        {
            get
            {
                return streamers.Count;
            }
        }

        // Events
        public event EventHandler<EventArgs> OnClientUpdate = delegate { };
        public event EventHandler<EventArgs> OnServerError = delegate { };

        // Members
        private Thread listenThread;
        private TcpListener listener;
        private ConcurrentDictionary<string, Streamer> streamers = new ConcurrentDictionary<string, Streamer>();

        public void Start(int port, int quality)
        {
            if (IsRunning)
            {
                Stop();
            }

            IsRunning = true;

            Port = port;
            Quality = quality;

            var listenThreadLoop = new ThreadStart(ListenLoop);
            listenThread = new Thread(listenThreadLoop);
            listenThread.Name = "Listener";
            listenThread.Start();
        }

        public void Stop()
        {
            IsRunning = false;

            foreach (var streamer in streamers)
            {
                streamer.Value.Stop();
            }
            streamers.Clear();

            if (listener != null)
            {
                listener.Stop();
            }
            
            if (listenThread != null)
            {
                listenThread.Join();
                listenThread = null;
            }
        }

        private void StopDueToError(Exception e)
        {
            if (IsRunning)
            {
                new Task(Stop).Start();

                // Todo: Do something with the actual exception (display message to user?)
                RaiseEventOnUIThread(OnServerError, this, new EventArgs());
            }
        }

        private void ListenLoop()
        {
            try
            {
                listener = new TcpListener(IPAddress.Any, Port);
                listener.Start();

                while (IsRunning)
                {
                    TcpClient client = listener.AcceptTcpClient();
                    Streamer switchStreamer = new Streamer(client, Quality);

                    if (streamers.ContainsKey(switchStreamer.IPAddress))
                    {
                        streamers[switchStreamer.IPAddress].Stop();
                        streamers[switchStreamer.IPAddress].OnStreamerError -= SwitchStreamer_OnStreamerError;
                    }

                    switchStreamer.OnStreamerError += SwitchStreamer_OnStreamerError;
                    streamers[switchStreamer.IPAddress] = switchStreamer;
                    switchStreamer.Start();

                    RaiseEventOnUIThread(OnClientUpdate, this, new EventArgs());
                }
            }
            catch (Exception e)
            {
                StopDueToError(e);
            }

            listener.Stop();
        }

        private void SwitchStreamer_OnStreamerError(object sender, EventArgs e)
        {
            Streamer streamer;
            if (streamers.TryRemove(((Streamer)sender).IPAddress, out streamer))
            {
                streamer.OnStreamerError -= SwitchStreamer_OnStreamerError;
            }

            RaiseEventOnUIThread(OnClientUpdate, this, new EventArgs());
        }

        private void RaiseEventOnUIThread(MulticastDelegate theEvent, params object[] args)
        {
            MulticastDelegate threadSafeMulticastDelegate = theEvent;
            if (threadSafeMulticastDelegate != null)
            {
                foreach (Delegate d in threadSafeMulticastDelegate.GetInvocationList())
                {
                    var synchronizeInvoke = d.Target as ISynchronizeInvoke;
                    if ((synchronizeInvoke != null) && synchronizeInvoke.InvokeRequired)
                    {
                        synchronizeInvoke.BeginInvoke(d, args);
                    }
                    else
                    {
                        d.DynamicInvoke(args);
                    }
                }
            }
        }
    }
}
