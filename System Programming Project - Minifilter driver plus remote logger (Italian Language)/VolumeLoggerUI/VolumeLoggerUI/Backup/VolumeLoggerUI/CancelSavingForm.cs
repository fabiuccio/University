using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VolumeLoggerUI
{
    public partial class CancelSavingForm : Form
    {
        private bool mustStop=false;
        public bool MustStop
        {
            get { return mustStop; }
        }
        public CancelSavingForm()
        {
            InitializeComponent();
            
            //Aggiungo il comportamento personalizzato in una eventuale
            //chiusura di applicazione
            this.Closing += new CancelEventHandler(Form_Closing);
        }

        private void Form_Closing(object sender, CancelEventArgs cArgs)
        {
            mustStop = true;
            cArgs.Cancel = true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Close();
        }

    }
}