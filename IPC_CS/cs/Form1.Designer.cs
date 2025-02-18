namespace CSClient
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            textBox1 = new TextBox();
            label1 = new Label();
            msg_label = new Label();
            SuspendLayout();
            // 
            // textBox1
            // 
            textBox1.Font = new Font("Times New Roman", 16.8000011F);
            textBox1.Location = new Point(12, 12);
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(558, 40);
            textBox1.TabIndex = 0;
            textBox1.KeyDown += textBox1_KeyDown;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Font = new Font("Times New Roman", 16.8000011F, FontStyle.Regular, GraphicsUnit.Point, 204);
            label1.Location = new Point(12, 63);
            label1.Name = "label1";
            label1.Size = new Size(122, 33);
            label1.TabIndex = 1;
            label1.Text = "Message:";
            // 
            // msg_label
            // 
            msg_label.BorderStyle = BorderStyle.FixedSingle;
            msg_label.Font = new Font("Times New Roman", 16.8000011F);
            msg_label.Location = new Point(12, 108);
            msg_label.Name = "msg_label";
            msg_label.Size = new Size(558, 316);
            msg_label.TabIndex = 2;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(582, 433);
            Controls.Add(msg_label);
            Controls.Add(label1);
            Controls.Add(textBox1);
            Name = "Form1";
            Text = "CS Client";
            FormClosing += Form1_FormClosing;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox textBox1;
        private Label label1;
        private Label msg_label;
    }
}
