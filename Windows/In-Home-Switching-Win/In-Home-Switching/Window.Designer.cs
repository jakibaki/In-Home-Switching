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
            this.ip_box = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.quality_bar = new System.Windows.Forms.TrackBar();
            this.label3 = new System.Windows.Forms.Label();
            this.toggle_button = new System.Windows.Forms.Button();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.connect_status = new System.Windows.Forms.ToolStripStatusLabel();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.quality_bar)).BeginInit();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ip_box
            // 
            this.ip_box.Location = new System.Drawing.Point(72, 131);
            this.ip_box.Name = "ip_box";
            this.ip_box.Size = new System.Drawing.Size(199, 20);
            this.ip_box.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 134);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Switch IP:";
            // 
            // quality_bar
            // 
            this.quality_bar.Cursor = System.Windows.Forms.Cursors.Default;
            this.quality_bar.Location = new System.Drawing.Point(72, 157);
            this.quality_bar.Maximum = 50;
            this.quality_bar.Minimum = 1;
            this.quality_bar.Name = "quality_bar";
            this.quality_bar.Size = new System.Drawing.Size(257, 45);
            this.quality_bar.TabIndex = 7;
            this.quality_bar.Tag = "";
            this.quality_bar.TickFrequency = 5;
            this.quality_bar.Value = 21;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(24, 166);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(42, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Quality:";
            // 
            // toggle_button
            // 
            this.toggle_button.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.toggle_button.Location = new System.Drawing.Point(72, 32);
            this.toggle_button.Name = "toggle_button";
            this.toggle_button.Size = new System.Drawing.Size(199, 77);
            this.toggle_button.TabIndex = 10;
            this.toggle_button.Text = "Connect";
            this.toggle_button.UseVisualStyleBackColor = true;
            this.toggle_button.Click += new System.EventHandler(this.Toggle_button_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connect_status});
            this.statusStrip1.Location = new System.Drawing.Point(0, 200);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(335, 22);
            this.statusStrip1.TabIndex = 11;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // connect_status
            // 
            this.connect_status.Name = "connect_status";
            this.connect_status.Size = new System.Drawing.Size(38, 17);
            this.connect_status.Text = "status";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(74, 185);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(22, 13);
            this.label2.TabIndex = 12;
            this.label2.Text = "1M";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(189, 185);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(28, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "25M";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(303, 185);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(28, 13);
            this.label5.TabIndex = 14;
            this.label5.Text = "50M";
            // 
            // Window
            // 
            this.ClientSize = new System.Drawing.Size(335, 222);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toggle_button);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.quality_bar);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ip_box);
            this.MaximumSize = new System.Drawing.Size(351, 261);
            this.MinimumSize = new System.Drawing.Size(351, 261);
            this.Name = "Window";
            this.Opacity = 0.99D;
            this.Text = "In-Home-Switching v0.1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Window_FormClosed);
            this.Shown += new System.EventHandler(this.Window_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.quality_bar)).EndInit();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TextBox ip_box;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TrackBar quality_bar;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button toggle_button;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel connect_status;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
    }
}

