using Configsys;
using System;
using System.Windows.Forms;

namespace InHomeSwitching
{
    public partial class Window : Form
    {
        private Server server;

        public Window()
        {
            InitializeComponent();

            // Update the window title with the product and version number
            Text = $"{Application.ProductName} v{Application.ProductVersion}";

            // Create a new streaming server that will listen for connections and stream video to clients
            server = new Server();
            server.OnClientUpdate += Server_OnClientUpdate;
            server.OnServerError += Server_OnServerError;
            UpdateUI();

            LoadConfig();
        }

        private void Window_FormClosed(object sender, FormClosedEventArgs e)
        {
            SaveConfig();
            Environment.Exit(0);
        }

        private void Toggle_button_Click(object sender, EventArgs e)
        {
            if (server.IsRunning)
            {
                server.Stop();
            }
            else
            {
                server.Start(2223, quality_bar.Value);
            }

            UpdateUI();
        }

        private void Server_OnClientUpdate(object sender, EventArgs e)
        {
            UpdateUI();
        }

        private void Server_OnServerError(object sender, EventArgs e)
        {
            UpdateUI();
        }

        private void UpdateUI()
        {
            if (server.IsRunning)
            {
                connect_status.Text = (server.ClientCount > 0 ? "Streaming to client..." : "Waiting for connection...");
                toggle_button.Text = "Stop";
                quality_bar.Enabled = false;
            }
            else
            {
                connect_status.Text = "Server Stopped";
                toggle_button.Text = "Start Server";
                quality_bar.Enabled = true;
            }
        }

        private void LoadConfig()
        {
            Config config = new Config();
            quality_bar.Value = config.Quality;
        }

        private void SaveConfig()
        {
            Config config = new Config()
            {
                Quality = quality_bar.Value
            };
            config.Save();
        }
    }
}
