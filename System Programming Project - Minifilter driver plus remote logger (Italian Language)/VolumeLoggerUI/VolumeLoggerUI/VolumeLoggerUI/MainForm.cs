using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading; // Thread
using System.IO;

namespace VolumeLoggerUI
{
    public partial class MainForm : Form
    {
        private DataReceiver myDataReceiver=null;//Classe per la gestione acquisizione dati
        private Thread myDataReceiverThread=null;//Thread associato
        const int J_TIMEOUT = 3000; //Attesa fino a chiusura forzata dell'applicazione
        private volatile uint buffer = 0;
        private uint showSeq = 0;
        private ulong received = 0;
        public uint Buffer
        {
            set{ buffer=value;}
            get{ return buffer;}
        }
   
        public delegate void AddRecordDelegate(String[] myString);
        public AddRecordDelegate addRecordDelegate;

        public delegate void SetStopDelegate();
        public SetStopDelegate setStopDelegate;

        public MainForm()
        {
            InitializeComponent();
            myDataReceiver = new DataReceiver(this, "239.255.255.250", 1600);

            //Aggiungo il comportamento personalizzato in una eventuale
            //chiusura di applicazione
            this.Closing += new CancelEventHandler(Form_Closing);

            //Inizializzo il delegato usato dalla invoke
            addRecordDelegate = new AddRecordDelegate(AddRecord);
            setStopDelegate = new SetStopDelegate(SetStop);
        }
        private void button1_Click(object sender, EventArgs e)
        {
            SetStartPause();
        }
        private void Form_Closing(object sender, CancelEventArgs cArgs)
        {
            DialogResult x = MessageBox.Show(this, "Are you sure you want to quit?", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question,MessageBoxDefaultButton.Button1);
            if (x == DialogResult.Yes)
            {
                cArgs.Cancel = false;
                KillReceiver(J_TIMEOUT);
            }
            else
                cArgs.Cancel = true;

        }
        private void optionsToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            FormOptions myFormOptions = new FormOptions(this);
            myFormOptions.Status[1] = myDataReceiver.Address;
            myFormOptions.Status[2] = "" + myDataReceiver.Port;
            myFormOptions.Status[3] = "" + buffer;
            myFormOptions.Status[4] = "" + showSeq;

            myFormOptions.ShowDialog();
            if (myFormOptions.Status[0].Equals("ok"))
            {
                bool isChanged = false;
                if (!myDataReceiver.Address.Equals(myFormOptions.Status[1]))
                {
                    myDataReceiver.Address = myFormOptions.Status[1];
                    SetStop();
                    isChanged = true;
                }
                if (myDataReceiver.Port != int.Parse(myFormOptions.Status[2]))
                {
                    myDataReceiver.Port = int.Parse(myFormOptions.Status[2]);
                    SetStop();
                    isChanged = true;
                }
                if (buffer != uint.Parse(myFormOptions.Status[3]))
                {
                    buffer = uint.Parse(myFormOptions.Status[3]);
                    AddRecord(null); //Faccio l'update grafico
                                     //Ma non è una modifica radicale...
                }
                if (showSeq != uint.Parse(myFormOptions.Status[4]))
                {
                    showSeq = uint.Parse(myFormOptions.Status[4]);
                    AddRecord(null);
                }
                if (isChanged)
                    MessageBox.Show("UDP client must be restarted because of new changes!");
            }
            myFormOptions.Dispose();
            this.Focus();
        }
        private void closeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private void button4_Click(object sender, EventArgs e)
        {
            SetStop();
        }
        private void button3_Click(object sender, EventArgs e)
        {
            SaveFile();
        }
        private void button2_Click(object sender, EventArgs e)
        {
            OpenFile();
        }
        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFile();
        }
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFile();
        }
        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetStartPause();
        }
        private void stopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetStop();
        }
        private void pauseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetStartPause();
        }
        private void clearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetClear();
        }
        /* Usato per aggiungere record o semplicemente per l'update 
         * di myDataGridView (values=null)!*/
        public void AddRecord(String[] values)
        {
            try
            {
                if (values != null)
                {
                    received++;
                    DataGridViewRow r = new DataGridViewRow();
                    
                    DataGridViewCell[] cells = new DataGridViewCell[7];
                    cells[0] = new DataGridViewComboBoxCell();
                    if (values[0] != null)
                    {
                        ((DataGridViewComboBoxCell)cells[0]).Items.Add(values[0]);
                    }
                    if (values[1] != null)
                    {
                        ((DataGridViewComboBoxCell)cells[0]).Items.Add(values[1]);
                    }  

                    if (((DataGridViewComboBoxCell)cells[0]).Items.Count>0)
                        ((DataGridViewComboBoxCell)cells[0]).Value = ((DataGridViewComboBoxCell)cells[0]).Items[0];

                    cells[1] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[1]).Value = values[2];
                    cells[2] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[2]).Value = values[3];
                    cells[3] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[3]).Value = values[4];
                    cells[4] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[4]).Value = values[5];
                    cells[5] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[5]).Value = values[6];
                    cells[6] = new DataGridViewTextBoxCell();
                    ((DataGridViewTextBoxCell)cells[6]).Value = values[7];
                    
                    r.Cells.AddRange(cells);
                    this.myDataGridView.Rows.Add(r);
                }
                /* Operazioni di semplice aggiornamento */
                //SEGLABIBRO            
                //Faccio una copia per assicurarmi che di avere una lettura consistente
                //tra l'if() e il for() nei confronti di buffer (scritta da un altro thread)
                uint local_copy_buffer = buffer;

                if (local_copy_buffer > 0)
                    while (this.myDataGridView.RowCount > local_copy_buffer)
                        this.myDataGridView.Rows.RemoveAt(0);

                //myDataGridView update
                if (showSeq == 0)
                    myDataGridView.Columns[1].Visible = false;
                else
                    myDataGridView.Columns[1].Visible = true;
                //Graphic-stats update
                toolStripStatusLabel1.Text = ("Received Rows " + received).PadRight(20);
                toolStripStatusLabel3.Text = ("Current Rows " + myDataGridView.RowCount).PadRight(20); ;
                toolStripStatusLabel2.Text = ("Buffer Length " + ((buffer == 0) ? "inf" : "" + buffer)).PadRight(20);
                //Enable-Disable save button/menu
                button3.Enabled = (myDataGridView.RowCount != 0);
                saveToolStripMenuItem.Enabled = (myDataGridView.RowCount != 0);
               
            }
            catch (Exception e)
            {
                MessageBox.Show("Exception! Message=" + e.Message + "\nStack=" + e.ToString());
            }
        }
        public void SetClear()
        {
            myDataGridView.Rows.Clear();
            AddRecord(null);
            toolStripStatusLabel5.Text = "Application has been cleared";
        }
        public void SetStop()
        {
            button4.Enabled = false;
            stopToolStripMenuItem.Enabled = false;
            KillReceiver(J_TIMEOUT);
            button1.Text = "Start";
            startToolStripMenuItem.Enabled = true;
            pauseToolStripMenuItem.Enabled = false;
            toolStripStatusLabel5.Text = "Application status is stopped";
        }
        public void SetStartPause()
        {
            button4.Enabled = true;
            stopToolStripMenuItem.Enabled = true;


            if (myDataReceiver.IsRunning)
            {
                /* Pause action */
                button1.Text = "Start";
                startToolStripMenuItem.Enabled = true;
                pauseToolStripMenuItem.Enabled = false;
                toolStripStatusLabel5.Text = "Data retrieval is paused.";
            }
            else
            {
                /* Start action */
                if (!myDataReceiver.HasBeenLaunched)
                {
                    ThreadStart MyDelegate = new ThreadStart(myDataReceiver.StartRetrieval);
                    myDataReceiverThread = new Thread(MyDelegate);
                    myDataReceiverThread.Start();
                    toolStripStatusLabel5.Text = "Data retrieval started.";
                }else
                    toolStripStatusLabel5.Text = "Data retrieval resumed.";
                button1.Text = "Pause";
                startToolStripMenuItem.Enabled = false;
                pauseToolStripMenuItem.Enabled = true;
                
            }
            myDataReceiver.IsRunning = !myDataReceiver.IsRunning;
        }
        private void KillReceiver(int timeout)
        {
            if (myDataReceiver.HasBeenLaunched)
            {
                myDataReceiver.Kill();
                if (myDataReceiverThread != null && myDataReceiverThread.IsAlive)
                {
                    myDataReceiverThread.Join(timeout);
                }
            }
        }
        private void OpenFile()
        {
            toolStripStatusLabel5.Text = "Choose the XML file to be opened";
            toolStripProgressBar1.Maximum = 0;
            toolStripProgressBar1.Value = 0;
            toolStripProgressBar1.Style= ProgressBarStyle.Marquee;
            toolStripProgressBar1.Visible = true;

            this.Enabled = false;
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            openFileDialog1.InitialDirectory = ".";
            openFileDialog1.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if ((openFileDialog1.FileName) != null)
                {
                    XMLHandler.ReadXML(openFileDialog1.FileName, addRecordDelegate);
                }
            }
            this.Enabled = true;
            this.Focus();
            toolStripProgressBar1.Visible = false;
            toolStripStatusLabel5.Text = "Opening operation terminated";
        }
        private void SaveFile()
        {
            toolStripStatusLabel5.Text = "Choose where to save data retrieved";
            SetStop();
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();

            saveFileDialog1.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            saveFileDialog1.FilterIndex = 1;
            saveFileDialog1.RestoreDirectory = true;
            saveFileDialog1.CheckPathExists = true;

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (saveFileDialog1.FileName != null)
                {
                    toolStripStatusLabel5.Text = "Saving XML file (please wait)";
                    toolStripProgressBar1.Maximum = myDataGridView.RowCount;
                    toolStripProgressBar1.Value=0;
                    toolStripProgressBar1.Visible= true;
                    toolStripProgressBar1.Style = ProgressBarStyle.Continuous;
                   
                    CancelSavingForm csf = new CancelSavingForm();
                    csf.Show();
                    this.Enabled = false;

                    for (int i = 0; i < myDataGridView.RowCount; i++)
                    {
                        if (csf.MustStop) break;
                        DataGridViewRow row = myDataGridView.Rows[i];
                        if (((DataGridViewComboBoxCell)row.Cells[0]).Items.Count > 1)
                        {
                            XMLHandler.WriteXML(saveFileDialog1.FileName,
                                            new string[]{
                                               (string)((DataGridViewComboBoxCell)row.Cells[0]).Items[0],
                                               (string)((DataGridViewComboBoxCell)row.Cells[0]).Items[1],
                                               (string)row.Cells[1].Value,
                                               (string)row.Cells[2].Value,
                                               (string)row.Cells[3].Value,
                                               (string)row.Cells[4].Value,
                                               (string)row.Cells[5].Value,
                                               (string)row.Cells[6].Value,
                                               (string)row.Cells[7].Value
                                            });
                        }
                        else if (((DataGridViewComboBoxCell)row.Cells[0]).Items.Count ==1)
                        {
                            XMLHandler.WriteXML(saveFileDialog1.FileName,
                                            new string[]{
                                               null,
                                               (string)((DataGridViewComboBoxCell)row.Cells[0]).Items[0],
                                               (string)row.Cells[1].Value,
                                               (string)row.Cells[2].Value,
                                               (string)row.Cells[3].Value,
                                               (string)row.Cells[4].Value,
                                               (string)row.Cells[5].Value,
                                               (string)row.Cells[6].Value,
                                               (string)row.Cells[7].Value
                                            });
                        }
                        toolStripProgressBar1.Value++;
                        System.Windows.Forms.Application.DoEvents(); 
                    }
                    csf.Dispose();
                    this.Enabled = true;
                    this.Focus();
                    toolStripProgressBar1.Visible = false;
                    saveFileDialog1.Dispose();
                    toolStripStatusLabel5.Text = "Saving operation terminated";
                }

            }
        }

        private void aboutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            AboutBox1 bt = new AboutBox1();
            bt.ShowDialog();
            bt.Dispose();
        }
    }
}