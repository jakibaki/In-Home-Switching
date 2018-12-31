using Configsys;
using System;
using System.Windows.Forms;

namespace InHomeSwitching.Window
{
    public partial class Window : Form
    {
        private Streamer streamer;

        public Window()
        {
            streamer = new Streamer();
            InitializeComponent();
            LoadConfig();
        }

        private void Window_Shown(object sender, EventArgs e)
        {
            while (true)
            {
                //connect_status.Text = streamer.running.ToString();
                if (streamer.running)
                {
                    connect_status.Text = $"Connected to {streamer.ip} with bitrate {streamer.quality}M";
                    toggle_button.Text = "Disconnect";
                    quality_bar.Enabled = false;

                }
                else
                {
                    connect_status.Text = "Disconnected";
                    toggle_button.Text = "Connect";
                    quality_bar.Enabled = true;
                }

                Application.DoEvents();
            }
        }


        private void Toggle_button_Click(object sender, EventArgs e)
        {
            if (streamer.started)
                streamer.Stop();
            else
            {
                if (ip_box.Text != string.Empty)
                {
                    streamer.Start(ip_box.Text, quality_bar.Value);
                    SaveConfig();
                }
                else
                    MessageBox.Show("Please enter the IP-Address of your switch!");
            }
        }

        private void Window_FormClosed(object sender, FormClosedEventArgs e)
        {
            SaveConfig();
            Environment.Exit(0);
        }

        private void LoadConfig()
        {
            Config config = new Config();
            ip_box.Text = config.IP;
            quality_bar.Value = config.Quality;
        }

        private void SaveConfig()
        {
            Config config = new Config()
            {
                IP = ip_box.Text,
                Quality = quality_bar.Value
            };
            config.Save();
        }

    }
}
