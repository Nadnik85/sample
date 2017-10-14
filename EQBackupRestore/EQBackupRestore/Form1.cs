using System;
using System.Configuration;
using System.Collections.Specialized;
using System.IO;
using System.IO.Compression;
using System.Drawing;
using System.Windows.Forms;

namespace EQBackupRestore
{
    public partial class Form1 : Form
    {
        FileInfo restoreFile;
        DirectoryInfo backupRootDir;
        DirectoryInfo backupDir;
        DirectoryInfo everquestDir;

        /**
         * <summary>Call these methods before the form is displayed</summary>
         */
        public Form1()
        {
            InitializeComponent();

            /* Set defaults based on configuration file */
            string eqDirPath = Properties.Settings.Default.EQDirPath;
            if ( String.IsNullOrEmpty(eqDirPath) )
                eqDirPath = "C:\\Users\\Public\\Daybreak Game Company\\Installed Games\\EverQuest";
            everquestDir = new DirectoryInfo(eqDirPath);
            backupEverquestDirTextBox.Text = everquestDir.FullName;
            restoreEverquestDirTextBox.Text = everquestDir.FullName;

            string backupRootDirPath = Properties.Settings.Default.BackupRootDirPath;
            if ( !String.IsNullOrEmpty(backupRootDirPath) )
            {
                backupRootDir = new DirectoryInfo(backupRootDirPath);
                backupDirTextBox.Text = backupRootDir.FullName;
                backupDirButton.Text = "Change Backup Dir";
                backupProgressTextBox.Text = "Ready to Backup!";
            }

            /* Set Checkbox status from settings */
            if (Properties.Settings.Default.BriniDefaults)
            {
                backupDefaultsCheckBox.Checked = true;
            }
            if (Properties.Settings.Default.BriniVoiceChat)
            {
                backupVoicechatCheckBox.Checked = true;
            }

            if ( String.IsNullOrEmpty(restoreFileTextBox.Text) )
                restoreButton.Enabled = false;
            if ( String.IsNullOrEmpty(backupDirTextBox.Text) )
                backupButton.Enabled = false;
        }

        /**
         * <summary>Print a message to the logging TextBox in the Backup tab</summary>
         * <param name="entryText">The log line</param>
         */
        private void logBackup(String entryText)
        {
            backupProgressTextBox.Text += entryText + Environment.NewLine;
        }

        /**
         * <summary>Attempt to create Bold font titles in the logging TextBox in the Backup tab</summary>
         */
        private void logBackupTitle(String entryText)
        {
            Font defaultFont = SystemFonts.DefaultFont;
            backupProgressTextBox.Font = new Font(defaultFont.FontFamily, defaultFont.Size + 2, System.Drawing.FontStyle.Bold);
            backupProgressTextBox.Text += "***" + entryText + Environment.NewLine;
            backupProgressTextBox.Font = SystemFonts.DefaultFont;
        }

        /**
         * <summary>Print a message to the logging TextBox in the Restore tab</summary>
         */
        private void logRestore(String entryText)
        {
            restoreProgressRichTextBox.Text += entryText + Environment.NewLine;
        }

        /**
         * <summary>Similar to logBackupTitle - print message preceeded by "***"</summary>
         */
        private void logRestoreTitle(String entryText)
        {
            restoreProgressRichTextBox.Text += "***" + entryText + Environment.NewLine;
        }

         /**
         * <summary>When the "Add Backup Dir" button is clicked, open a dialog,
         * assign the new path, and enable the "Backup" button</summary>
         */
        private void backupDirButton_Click(object sender, EventArgs e)
        {
            if (backupDirFolderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                backupRootDir = new DirectoryInfo(backupDirFolderBrowserDialog.SelectedPath);
                if (null == backupRootDir)
                {
                    backupProgressTextBox.Text = "Invalid Backup Directory";
                    backupDirButton.Text = "Add Backup Dir";
                }
                else
                {
                    backupDirTextBox.Text = backupRootDir.FullName;
                    Properties.Settings.Default.BackupRootDirPath = backupRootDir.FullName;
                    Properties.Settings.Default.Save();
                    backupDirButton.Text = "Change Backup Dir";
                    backupButton.Enabled = true;
                    backupProgressTextBox.Text = "Ready to Backup!";
                }
            }
        }

        /**
         * <summary>When the "Change Everquest Dir" button on the Backup tab is clicked,
         * open dialog, and then update the everquestDir variable and
         * both tabs' EverquestDirTextBox</summary>
         */
        private void backupEverquestDirButton_Click(object sender, EventArgs e)
        {
            if (everquestDirFolderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                if (checkEverquestDirOk(everquestDirFolderBrowserDialog.SelectedPath))
                {
                    everquestDir = new DirectoryInfo(everquestDirFolderBrowserDialog.SelectedPath);
                    backupEverquestDirTextBox.Text = everquestDir.FullName;
                    restoreEverquestDirTextBox.Text = everquestDir.FullName;
                    Properties.Settings.Default.EQDirPath = everquestDir.FullName;
                    Properties.Settings.Default.Save();
                }
                else
                {
                    //MessageBox.Show("Invalid EverQuest Directory");
                    DialogResult result = MessageBox.Show("Invalid EverQuest Directory: no eqgame.exe or userdata directory found, continue anyway?", 
                        "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
                    if (DialogResult.Yes == result)
                    {
                        everquestDir = new DirectoryInfo(everquestDirFolderBrowserDialog.SelectedPath);
                        backupEverquestDirTextBox.Text = everquestDir.FullName;
                        restoreEverquestDirTextBox.Text = everquestDir.FullName;
                    }
                }
            }
        }

        /**
         * <summary>The button event that starts the Backup</summary>
         */
        private void backupButton_Click(object sender, EventArgs e)
        {
            backupProgressTextBox.Text = "";
            backupFiles();
        }

        /**
         * <summary>When the "Use Restore File" button on the Restore Tab is clicked, 
         * open a dialog and update the restoreFile variable</summary>
         */
        private void restoreFileButton_Click(object sender, EventArgs e)
        {
            if (restoreOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                if (restoreOpenFileDialog.CheckFileExists)
                {
                    restoreFile = new FileInfo(restoreOpenFileDialog.FileName);
                    restoreFileTextBox.Text = restoreFile.FullName;
                    restoreButton.Enabled = true;

                    restoreFileButton.Text = "Change Restore File";
                    restoreProgressRichTextBox.Text = "";
                }
            }
        }

        /**
         * <summary>When the "Change Everquest Dir" button on the Restore tab is clicked,
         * open dialog, and then update the everquestDir variable and
         * both tabs' EverquestDirTextBox</summary>
         */
        private void restoreEverquestDirButton_Click(object sender, EventArgs e)
        {
            if (everquestDirFolderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                everquestDir = new DirectoryInfo(everquestDirFolderBrowserDialog.SelectedPath);
                restoreEverquestDirTextBox.Text = everquestDir.FullName;
                backupEverquestDirTextBox.Text = everquestDir.FullName;
                Properties.Settings.Default.EQDirPath = everquestDir.FullName;
                Properties.Settings.Default.Save();
            }
        }

        /**
         * <summary>The button that unzips the archive and restores the files to the
         * Everquest directory</summary>
         */
        private void restoreButton_Click(object sender, EventArgs e)
        {
            if (null == everquestDir)
            {
                everquestDir = new DirectoryInfo(restoreEverquestDirTextBox.Text);
            }
            restoreButton.Enabled = false;
            backupRootDir = restoreFile.Directory;
            String zipBasename = Path.GetFileNameWithoutExtension(restoreFile.Name);
            backupDir = new DirectoryInfo(Path.Combine(backupRootDir.FullName, zipBasename));
            backupDirTextBox.Text = backupDir.FullName;

            restoreFiles();
        }

        private void aboutToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This program backs up your EverQuest settings for every character. If you need help, see the thread on RedGuides. Made by Rephaite\nExclusively for RedGuides.com", "About");
        }

        /**
         * <summary>When the "Backup defaults.ini" checkbox is clicked, save preference</summary>
         */
        private void backupDefaultsCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (backupDefaultsCheckBox.Checked)
                Properties.Settings.Default.BriniDefaults = true;
            else
                Properties.Settings.Default.BriniDefaults = false;
            Properties.Settings.Default.Save();
        }

        /**
         * <summary>When the "Backup VoiceChat.ini" checkbox is clicked, save preference</summary>
         */
        private void backupVoicechatCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            if (backupVoicechatCheckBox.Checked)
                Properties.Settings.Default.BriniVoiceChat = true;
            else
                Properties.Settings.Default.BriniVoiceChat = false;
            Properties.Settings.Default.Save();
        }

        private void redGuidesLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // Specify that the link was visited.
            this.redGuidesLinkLabel.LinkVisited = true;

            // Navigate to a URL.
            System.Diagnostics.Process.Start("https://www.redguides.com/forums/threads/58150-A-New-Way-to-Backup-and-Restore-your-EverQuest-Settings-(EQBackupRestore-exe-release)");
        }
    }
}
