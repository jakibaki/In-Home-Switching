using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Drawing.Imaging;

namespace InHomeSwitching.Window
{
    public partial class Window : Form
    {

        private Streamer streamer;

        public Window()
        {
            streamer = new Streamer();
            InitializeComponent();
        }

        private void Window_Shown(object sender, EventArgs e)
        {
            while (true)
            {
                //connect_status.Text = streamer.running.ToString();
                if (streamer.running)
                {
                    connect_status.Text = String.Format("Connected to {0} with bitrate {1}M", streamer.ip, streamer.quality);
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


        private void toggle_button_Click(object sender, EventArgs e)
        {
            if (streamer.started)
            {
                streamer.Stop();
            }
            else
            {
                if (ip_box.Text != "")
                {
                    streamer.Start(ip_box.Text, quality_bar.Value);
                }
                else
                {
                    MessageBox.Show("Please enter the IP-Address of your switch!");
                }
            }
        }

        private void Window_FormClosed(object sender, FormClosedEventArgs e)
        {
            System.Environment.Exit(1);
        }
    }
}
