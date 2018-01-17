using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VolumeLoggerUI
{
    public partial class FormOptions : Form
    {
        private string[] status = new string[5];

        public string[] Status {
            get { return status; }
            set
            {
                status = value;
            }
        }
        private MainForm formRef;
        public FormOptions(MainForm formRef)
        {
            this.formRef = formRef;
            for (int i = 0; i < status.Length; i++)
                status[i] = "";
            InitializeComponent();
        }
        private void button1_Click(object sender, EventArgs e)
        {
            status[0] = "ok";
            checkBox2_CheckedChanged(null, null);
            status[1] = textBox2.Text + "." + textBox3.Text + "." +
                        textBox4.Text + "." + textBox5.Text;
            status[2] = textBox1.Text;
            status[3] = (checkBox1.Checked) ? ""+0 : ""+NumUpDown1.Value;
            status[4] = (checkBox3.Checked) ? "" + 1 : "" + 0;
            this.Hide();
        }
        private void button2_Click(object sender, EventArgs e)
        {
            status[0] = "cancel";
            this.Hide();
        }
        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            this.NumUpDown1.Enabled = !checkBox1.Checked;
        }
        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            textBox1_LostFocus(sender, e);
            textBox2_LostFocus(sender, e);
            textBox3_LostFocus(sender, e);
            textBox4_LostFocus(sender, e);
            textBox5_LostFocus(sender, e);
        }
        private void textBox2_GotFocus(object sender, EventArgs e)
        {
            textBox2.SelectionStart = 0;
        }
        private void textBox2_LostFocus(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                string s = textBox2.Text.PadRight(3, '0').Replace(' ', '0');
                if (int.Parse(s) > 239)
                    s = "239";
                if (int.Parse(s) < 224)
                    s = "224";
                textBox2.Text = s;
            }
        }
        private void textBox2_KeyEvent(object sender, EventArgs e)
        {
            if (textBox2.SelectionStart == 3)
            {
                textBox3.Focus();
                textBox3.SelectionStart = 0;
                
            }
        }
        private void textBox3_GotFocus(object sender, EventArgs e)
        {
            textBox3.SelectionStart = 0;
        }
        private void textBox3_LostFocus(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                string s = textBox3.Text.PadRight(3, '0').Replace(' ', '0');
                if (int.Parse(s) > 255)
                    s = "255";
                textBox3.Text = s;
            }
        }
        private void textBox3_KeyEvent(object sender, EventArgs e)
        {
            if (textBox3.SelectionStart == 3)
            {
                textBox4.Focus();
                textBox4.SelectionStart = 0;
            }
        }
        private void textBox4_GotFocus(object sender, EventArgs e)
        {
            textBox4.SelectionStart = 0;
        }
        private void textBox4_LostFocus(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                string s = textBox4.Text.PadRight(3, '0').Replace(' ', '0');
                if (int.Parse(s) > 255)
                    s = "255";
                textBox4.Text = s;
            }
        }
        private void textBox4_KeyEvent(object sender, EventArgs e)
        {
            if (textBox4.SelectionStart == 3)
            {
                textBox5.Focus();
                textBox5.SelectionStart = 0;
            }
        }
        private void textBox5_GotFocus(object sender, EventArgs e)
        {
            textBox5.SelectionStart = 0;
        }
        private void textBox5_LostFocus(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                string s = textBox5.Text.PadRight(3, '0').Replace(' ', '0');
                if (int.Parse(s) > 255)
                    s = "255";
                textBox5.Text = s;
            }
        }
        private void textBox5_KeyEvent(object sender, EventArgs e)
        {
            if (textBox5.SelectionStart == 3)
            {
                textBox1.Focus();
                textBox1.SelectionStart = 0;
            }
        }
        private void textBox1_GotFocus(object sender, EventArgs e)
        {
            textBox1.SelectionStart = 0;
        }
        private void textBox1_LostFocus(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                if (textBox1.Text.Length > 4 && int.Parse(textBox1.Text) > 65535)
                {
                    textBox1.Text = "" + 65535;
                }
                if (textBox1.Text.Trim().Trim().Equals("") ||
                   (textBox1.Text.Length > 0 && int.Parse(textBox1.Text) == 0)
                    )
                    textBox1.Text = "" + 1;
            }
        }
        private void FormOptions_Load(object sender, EventArgs e)
        {
            textBox2.Text = status[1].Substring(0,3);
            textBox3.Text = status[1].Substring(4, 3);
            textBox4.Text = status[1].Substring(8, 3);
            textBox5.Text = status[1].Substring(12, 3);

            textBox1.Text = status[2];
            if (status[3].Equals("0"))
            {
                NumUpDown1.Enabled = false;
                checkBox1.Checked = true;
            }
            else
            {
                NumUpDown1.Enabled = true;
                checkBox1.Checked = false;
                NumUpDown1.Value = uint.Parse(status[3]);
            }

            if (int.Parse(status[4]) == 0) checkBox3.Checked = false;
            else checkBox3.Checked = true;
            button2.Select();//Not yet visible, so Focus doesn't work!
        }
    }
}