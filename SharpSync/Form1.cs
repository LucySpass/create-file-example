using System;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using System.Text;
using System.Linq;

namespace SharpSync
{
    public partial class Form1 : Form
	{
        int allThreads = 0;

		Process ChildProcess = null;
		EventWaitHandle evStop = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStop");
		EventWaitHandle evQuit = new EventWaitHandle(false, EventResetMode.AutoReset, "EventQuit");
        EventWaitHandle evStartThread = new EventWaitHandle(false, EventResetMode.AutoReset, "EventStartThread");
        EventWaitHandle evConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "EventConfirm");
        EventWaitHandle evSend = new EventWaitHandle(false, EventResetMode.AutoReset, "EventSend");

        public Form1()
		{
			InitializeComponent();
		}

        [DllImport("beg-liba.dll", CharSet =CharSet.Ansi)]
        private static extern void form_send(int num, int size, StringBuilder txt);

        private void button1_Click(object sender, EventArgs e) // START BUTTON
		{
			if (ChildProcess == null || ChildProcess.HasExited)
			{
				ChildProcess = Process.Start("MFCApplication.exe");
                comboBox1.Items.Clear();
                comboBox1.Items.Add("Main thread");
                comboBox1.Items.Add("All threads");

                textBox1.Enabled = true;
                comboBox1.Enabled = true;
                threadsBox.Enabled = true;
                button2.Enabled = true;
                button3.Enabled = true;
            }
            else if (threadsBox.Text != null && threadsBox.Text != "")
            {
                int threadsNum = 0;
                Int32.TryParse(threadsBox.Text, out threadsNum);

                for(int i = 0; i < threadsNum; i++)
                {
                    evStartThread.Set();
                    listBox1.Items.Add(i);
                    evConfirm.WaitOne();
                    if (allThreads == 0) listBox1.Items.Add("Started main thread.");
                    listBox1.Items.Add("Started thread number: " + (allThreads));
                    comboBox1.Items.Add("Thread " + (allThreads));
                    allThreads++;
                }
            }
            else
            {
                textBox1.Enabled = false;
                comboBox1.Enabled = false;
                threadsBox.Enabled = false;
                button2.Enabled = false;
                button3.Enabled = false;
            }
        }

		private void button2_Click(object sender, EventArgs e) // STOP BUTTON
		{
			if (ChildProcess != null && !ChildProcess.HasExited)
			{
                if (allThreads > 0)
                {
                    evStop.Set();

                    evConfirm.WaitOne();
                    listBox1.Items.Add("Got the confirm that thread " + (allThreads) + " stopped!");
                    comboBox1.Items.RemoveAt(allThreads+1);
                    allThreads--;

                    if (allThreads == 0) listBox1.Items.Add("Stopped main thread.");
                }
                else
                {
                    listBox1.Items.Add("There are 0 threads active! Start some.");
                }                
            }
            else
            {
                listBox1.Items.Add("Start the c++ .exe first!");
                button2.Enabled = false;
                threadsBox.Enabled = false;
                button3.Enabled = false;
            }
		}

        private void button3_Click(object sender, EventArgs e) // SEND BUTTON
        {
            if (ChildProcess != null && !ChildProcess.HasExited)
            {
                int selectIndex = comboBox1.SelectedIndex;
                if (selectIndex >= 0 && textBox1.Text.Count() != 0)
                {
                    string sms = textBox1.Text;
                    StringBuilder _sms = new StringBuilder(sms);
                    form_send(selectIndex, _sms.Length, _sms);
                    listBox1.Items.Add("Sending the message: " + _sms);
                    evSend.Set();
                    evConfirm.WaitOne();
                }
            }
            else
            {
                listBox1.Items.Add("Start the c++ .exe first!");
                button2.Enabled = false;
                threadsBox.Enabled = false;
                button3.Enabled = false;
            }

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e) // CLOSE BUTTON
		{
            if (ChildProcess != null && !ChildProcess.HasExited)
            {
                listBox1.Items.Add("Waiting for handles to close...");
                evQuit.Set();
                evConfirm.WaitOne();
            }            
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
