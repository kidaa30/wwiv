﻿/**************************************************************************/
/*                                                                        */
/*                              WWIV Version 5.0x                         */
/*                 Copyright (C) 2014, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace WWIV5TelnetServer
{
    public partial class MainForm : Form
    {
        private TelnetServer server = new TelnetServer();
    
        public MainForm()
        {
            InitializeComponent();
            startToolStripMenuItem.Enabled = true;
            stopToolStripMenuItem.Enabled = false;
            server.StatusMessageChanged += server_StatusMessage;
            server.NodeStatusChanged += server_NodeStatusChanged;
        }

        private void server_NodeStatusChanged(object sender, NodeStatusEventArgs e)
        {
            Console.WriteLine("server_NodeStatusChanged");
            // TODO(rushfan): Build list of strings for model on this side.
            Action<List<NodeStatus>> update = delegate(List<NodeStatus> nodes)
            {
                // Hack. can't figure out C# databinding. According to stackoverflow, many others can't either.
                listBoxNodes.Items.Clear();
                listBoxNodes.Items.AddRange(nodes.ToArray());
            };
            if (InvokeRequired)
            {
                this.Invoke(update, new object[] { server.Nodes });
            }
            else
            {
                update(server.Nodes);
            }
        }

        void server_StatusMessage(object sender, StatusMessageEventArgs e)
        {
            var message = String.Format("{0}: {1}", DateTime.Now.ToString(), e.Message);
            Console.WriteLine(message);
            Action a = delegate() { AppendMessage(message); };
            if (this.messages.InvokeRequired)
            {
                this.Invoke(a);
            }
            else
            {
                a();
            }
        }

        void AppendMessage(string message)
        {
            messages.AppendText(message + "\r\n");
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
             MessageBox.Show("WWIV5TelnetServer/C# (experimental)");
        }

        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {
            server.Start();
            // Hack. can't figure out C# databinding. According to stackoverflow, many others can't either.
            listBoxNodes.Items.Clear();
            listBoxNodes.Items.AddRange(server.Nodes.ToArray());
            startToolStripMenuItem.Enabled = false;
            stopToolStripMenuItem.Enabled = true;
            preferencesToolStripMenuItem.Enabled = false;
        }

        private void stopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            listBoxNodes.DataSource = null;
            server.Stop();
            startToolStripMenuItem.Enabled = true;
            stopToolStripMenuItem.Enabled = false;
            preferencesToolStripMenuItem.Enabled = true;
        }

        private void preferencesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PreferencesDialog prefs = new PreferencesDialog();
            prefs.ShowDialog();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Console.WriteLine("MainForm_FormClosing");
            server.Stop();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.autostart)
            {
                Console.WriteLine("AutoStarting server.");
                startToolStripMenuItem_Click(sender, e);
            }
        }

    }
}
