using Configsys;
using System;
using System.Windows.Forms;

namespace InHomeSwitching.Window
{
    public partial class Window : Form
    {
        private Streamer streamer;

        // Initialize currentip and quality (shouldn't affect config) [JiNama]
        private string currentip = "Enter the IP of your Switch";
        private int currentqualityvalue = 25;
        private Boolean failed = true;

        public Window()
        {
            streamer = new Streamer();
            InitializeComponent();
            LoadConfig();

            // Creates the banner effect
            inhomeswitching_picture.Parent = under;

        }

        private void Window_Shown(object sender, EventArgs e)
        {
            while (true)
            {
                //connect_status.Text = streamer.running.ToString();
                if (streamer.running)
                {
                    connect_status.Text = $"Connected to {streamer.ip} with a bitrate of {streamer.quality}M";
                    toggle_button.Text = "Disconnect";
                    quality_bar.Enabled = false;
                    ip_box.Enabled = false;
                    save_button.Enabled = false;
                    
                    // Change color when "connected"
                    toggle_button.BackColor = System.Drawing.Color.FromArgb(255, 100, 100);
                }
                else
                {
                    connect_status.Text = "Disconnected";
                    toggle_button.Text = "Connect";
                    quality_bar.Enabled = true;
                    ip_box.Enabled = true;
                    save_button.Enabled = true;

                    // Change color when "disconnected"
                    toggle_button.BackColor = System.Drawing.Color.FromArgb(128, 255, 128);
                }

                Application.DoEvents();
            }
        }


        private void Toggle_button_Click(object sender, EventArgs e)
        {

            if (ip_box.Text != string.Empty)
            {
                if (streamer.started)
                    streamer.Stop();
                else if (failed == false)
                {
                    streamer.Start(currentip, currentqualityvalue);
                }
                else
                {
                    MessageBox.Show("Edit your settings.", "Failed");
                }
            }
            else
            {
                MessageBox.Show("Edit your settings.", "Failed");
            }


        }

        private void Window_FormClosed(object sender, FormClosedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void LoadConfig()
        {
            config config = new config();
            if (config.IP != null){
                ip_box.Text = config.IP;
                currentip = config.IP;
            }
            quality_bar.Value = config.Quality;
            currentqualityvalue = config.Quality;
            failed = false;
        }

        private void SaveConfig()
        {
            config config = new config()
            {
                IP = ip_box.Text,
                Quality = quality_bar.Value
            };
            config.Save();
        }

        // When in focus, remove watermark [JiNama]
        private void ip_box_GotFocus(object sender, EventArgs e)
        {
            if (ip_box.Text == "Enter the IP of your Switch") {
                ip_box.Text = "";
            }
            ip_box.ForeColor = System.Drawing.Color.Black;
        }

        // With my code, settings only apply once saved, not on connect button clicked [JiNama]
        private void save_button_Click(object sender, EventArgs e)
        {
            currentip = ip_box.Text;
            currentqualityvalue = quality_bar.Value;
            if (currentip != "Enter the IP of your Switch" && currentip != string.Empty) {
                MessageBox.Show("Saved settings successfully.", "Success");
                failed = false;
                SaveConfig();
            }
            else
            {
                MessageBox.Show("Settings failed to save. Try adjusting your settings.", "Failed");
                failed = true;
            }
        }
    }
}
