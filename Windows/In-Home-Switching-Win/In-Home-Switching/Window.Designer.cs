namespace InHomeSwitching.Window
{
    partial class Window
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Window));
            this.ip_box = new System.Windows.Forms.TextBox();
            this.quality_bar = new System.Windows.Forms.TrackBar();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.connect_status = new System.Windows.Forms.ToolStripStatusLabel();
            this.toggle_button = new System.Windows.Forms.Button();
            this.inhomeswitching_picture = new System.Windows.Forms.PictureBox();
            this.under = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.save_button = new System.Windows.Forms.Button();
            this.fiftym_label = new System.Windows.Forms.Label();
            this.onem_label = new System.Windows.Forms.Label();
            this.settings_label = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.quality_bar)).BeginInit();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.inhomeswitching_picture)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.under)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ip_box
            // 
            this.ip_box.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ip_box.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ip_box.ForeColor = System.Drawing.Color.Gray;
            this.ip_box.Location = new System.Drawing.Point(12, 91);
            this.ip_box.MaxLength = 20;
            this.ip_box.Name = "ip_box";
            this.ip_box.Size = new System.Drawing.Size(304, 26);
            this.ip_box.TabIndex = 3;
            this.ip_box.TabStop = false;
            this.ip_box.Text = "Enter the IP of your Switch";
            this.ip_box.Enter += new System.EventHandler(this.ip_box_GotFocus);
            // 
            // quality_bar
            // 
            this.quality_bar.Cursor = System.Windows.Forms.Cursors.Default;
            this.quality_bar.Location = new System.Drawing.Point(9, 40);
            this.quality_bar.Maximum = 50;
            this.quality_bar.Minimum = 1;
            this.quality_bar.Name = "quality_bar";
            this.quality_bar.Size = new System.Drawing.Size(307, 45);
            this.quality_bar.TabIndex = 7;
            this.quality_bar.TabStop = false;
            this.quality_bar.Tag = "";
            this.quality_bar.TickFrequency = 5;
            this.quality_bar.Value = 25;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connect_status});
            this.statusStrip1.Location = new System.Drawing.Point(0, 349);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(784, 22);
            this.statusStrip1.TabIndex = 11;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // connect_status
            // 
            this.connect_status.BackColor = System.Drawing.SystemColors.Control;
            this.connect_status.Name = "connect_status";
            this.connect_status.Size = new System.Drawing.Size(38, 17);
            this.connect_status.Text = "status";
            // 
            // toggle_button
            // 
            this.toggle_button.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(255)))), ((int)(((byte)(128)))));
            this.toggle_button.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.toggle_button.Font = new System.Drawing.Font("Raleway", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.toggle_button.Location = new System.Drawing.Point(67, 201);
            this.toggle_button.Name = "toggle_button";
            this.toggle_button.Size = new System.Drawing.Size(300, 100);
            this.toggle_button.TabIndex = 15;
            this.toggle_button.TabStop = false;
            this.toggle_button.Text = "Connect";
            this.toggle_button.UseVisualStyleBackColor = false;
            this.toggle_button.Click += new System.EventHandler(this.Toggle_button_Click);
            // 
            // inhomeswitching_picture
            // 
            this.inhomeswitching_picture.BackColor = System.Drawing.Color.Transparent;
            this.inhomeswitching_picture.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.inhomeswitching_picture.Image = ((System.Drawing.Image)(resources.GetObject("inhomeswitching_picture.Image")));
            this.inhomeswitching_picture.Location = new System.Drawing.Point(12, 12);
            this.inhomeswitching_picture.Name = "inhomeswitching_picture";
            this.inhomeswitching_picture.Size = new System.Drawing.Size(760, 140);
            this.inhomeswitching_picture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.inhomeswitching_picture.TabIndex = 16;
            this.inhomeswitching_picture.TabStop = false;
            // 
            // under
            // 
            this.under.BackColor = System.Drawing.Color.RoyalBlue;
            this.under.Location = new System.Drawing.Point(0, -5);
            this.under.Name = "under";
            this.under.Size = new System.Drawing.Size(784, 157);
            this.under.TabIndex = 17;
            this.under.TabStop = false;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.Controls.Add(this.save_button);
            this.panel1.Controls.Add(this.fiftym_label);
            this.panel1.Controls.Add(this.onem_label);
            this.panel1.Controls.Add(this.settings_label);
            this.panel1.Controls.Add(this.ip_box);
            this.panel1.Controls.Add(this.quality_bar);
            this.panel1.Location = new System.Drawing.Point(456, 152);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(328, 197);
            this.panel1.TabIndex = 18;
            // 
            // save_button
            // 
            this.save_button.Font = new System.Drawing.Font("Raleway", 9.749999F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.save_button.Location = new System.Drawing.Point(118, 143);
            this.save_button.Name = "save_button";
            this.save_button.Size = new System.Drawing.Size(86, 28);
            this.save_button.TabIndex = 11;
            this.save_button.Text = "Save";
            this.save_button.UseVisualStyleBackColor = true;
            this.save_button.Click += new System.EventHandler(this.save_button_Click);
            // 
            // fiftym_label
            // 
            this.fiftym_label.AutoSize = true;
            this.fiftym_label.Location = new System.Drawing.Point(288, 24);
            this.fiftym_label.Name = "fiftym_label";
            this.fiftym_label.Size = new System.Drawing.Size(28, 13);
            this.fiftym_label.TabIndex = 10;
            this.fiftym_label.Text = "50M";
            // 
            // onem_label
            // 
            this.onem_label.AutoSize = true;
            this.onem_label.Location = new System.Drawing.Point(9, 24);
            this.onem_label.Name = "onem_label";
            this.onem_label.Size = new System.Drawing.Size(22, 13);
            this.onem_label.TabIndex = 9;
            this.onem_label.Text = "1M";
            // 
            // settings_label
            // 
            this.settings_label.AutoSize = true;
            this.settings_label.Font = new System.Drawing.Font("Raleway", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.settings_label.Location = new System.Drawing.Point(113, 12);
            this.settings_label.Name = "settings_label";
            this.settings_label.Size = new System.Drawing.Size(91, 25);
            this.settings_label.TabIndex = 8;
            this.settings_label.Text = "Settings";
            // 
            // Window
            // 
            this.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.ClientSize = new System.Drawing.Size(784, 371);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.inhomeswitching_picture);
            this.Controls.Add(this.toggle_button);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.under);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(800, 410);
            this.MinimumSize = new System.Drawing.Size(800, 410);
            this.Name = "Window";
            this.Text = "In Home Switching";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Window_FormClosed);
            this.Shown += new System.EventHandler(this.Window_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.quality_bar)).EndInit();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.inhomeswitching_picture)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.under)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TextBox ip_box;
        private System.Windows.Forms.TrackBar quality_bar;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel connect_status;
        private System.Windows.Forms.Button toggle_button;
        private System.Windows.Forms.PictureBox inhomeswitching_picture;
        private System.Windows.Forms.PictureBox under;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label settings_label;
        private System.Windows.Forms.Label onem_label;
        private System.Windows.Forms.Button save_button;
        private System.Windows.Forms.Label fiftym_label;
    }
}

