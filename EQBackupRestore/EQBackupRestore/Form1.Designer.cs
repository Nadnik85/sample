namespace EQBackupRestore
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.backupTabPage = new System.Windows.Forms.TabPage();
            this.backupVoicechatCheckBox = new System.Windows.Forms.CheckBox();
            this.backupDefaultsCheckBox = new System.Windows.Forms.CheckBox();
            this.backupProgressTextBox = new System.Windows.Forms.TextBox();
            this.backupButton = new System.Windows.Forms.Button();
            this.backupEverquestDirButton = new System.Windows.Forms.Button();
            this.backupDirButton = new System.Windows.Forms.Button();
            this.backupEverquestDirTextBox = new System.Windows.Forms.TextBox();
            this.backupDirTextBox = new System.Windows.Forms.TextBox();
            this.restoreTabPage = new System.Windows.Forms.TabPage();
            this.restoreProgressRichTextBox = new System.Windows.Forms.RichTextBox();
            this.restoreButton = new System.Windows.Forms.Button();
            this.restoreEverquestDirTextBox = new System.Windows.Forms.TextBox();
            this.restoreFileTextBox = new System.Windows.Forms.TextBox();
            this.restoreEverquestDirButton = new System.Windows.Forms.Button();
            this.restoreFileButton = new System.Windows.Forms.Button();
            this.backupDirFolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.everquestDirFolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.restoreOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.restoreTimer = new System.Windows.Forms.Timer(this.components);
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.licenseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.redGuidesLinkLabel = new System.Windows.Forms.LinkLabel();
            this.tabControl1.SuspendLayout();
            this.backupTabPage.SuspendLayout();
            this.restoreTabPage.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.backupTabPage);
            this.tabControl1.Controls.Add(this.restoreTabPage);
            this.tabControl1.Location = new System.Drawing.Point(13, 44);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(689, 262);
            this.tabControl1.TabIndex = 0;
            // 
            // backupTabPage
            // 
            this.backupTabPage.Controls.Add(this.backupVoicechatCheckBox);
            this.backupTabPage.Controls.Add(this.backupDefaultsCheckBox);
            this.backupTabPage.Controls.Add(this.backupProgressTextBox);
            this.backupTabPage.Controls.Add(this.backupButton);
            this.backupTabPage.Controls.Add(this.backupEverquestDirButton);
            this.backupTabPage.Controls.Add(this.backupDirButton);
            this.backupTabPage.Controls.Add(this.backupEverquestDirTextBox);
            this.backupTabPage.Controls.Add(this.backupDirTextBox);
            this.backupTabPage.Location = new System.Drawing.Point(4, 22);
            this.backupTabPage.Name = "backupTabPage";
            this.backupTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.backupTabPage.Size = new System.Drawing.Size(681, 236);
            this.backupTabPage.TabIndex = 0;
            this.backupTabPage.Text = "Backup";
            this.backupTabPage.UseVisualStyleBackColor = true;
            // 
            // backupVoicechatCheckBox
            // 
            this.backupVoicechatCheckBox.AutoSize = true;
            this.backupVoicechatCheckBox.Location = new System.Drawing.Point(7, 85);
            this.backupVoicechatCheckBox.Name = "backupVoicechatCheckBox";
            this.backupVoicechatCheckBox.Size = new System.Drawing.Size(128, 17);
            this.backupVoicechatCheckBox.TabIndex = 7;
            this.backupVoicechatCheckBox.Text = "Backup VoiceChat.ini";
            this.backupVoicechatCheckBox.UseVisualStyleBackColor = true;
            this.backupVoicechatCheckBox.CheckedChanged += new System.EventHandler(this.backupVoicechatCheckBox_CheckedChanged);
            // 
            // backupDefaultsCheckBox
            // 
            this.backupDefaultsCheckBox.AutoSize = true;
            this.backupDefaultsCheckBox.Location = new System.Drawing.Point(7, 61);
            this.backupDefaultsCheckBox.Name = "backupDefaultsCheckBox";
            this.backupDefaultsCheckBox.Size = new System.Drawing.Size(116, 17);
            this.backupDefaultsCheckBox.TabIndex = 6;
            this.backupDefaultsCheckBox.Text = "Backup defaults.ini";
            this.backupDefaultsCheckBox.UseVisualStyleBackColor = true;
            this.backupDefaultsCheckBox.CheckedChanged += new System.EventHandler(this.backupDefaultsCheckBox_CheckedChanged);
            // 
            // backupProgressTextBox
            // 
            this.backupProgressTextBox.Location = new System.Drawing.Point(7, 140);
            this.backupProgressTextBox.Multiline = true;
            this.backupProgressTextBox.Name = "backupProgressTextBox";
            this.backupProgressTextBox.ReadOnly = true;
            this.backupProgressTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.backupProgressTextBox.Size = new System.Drawing.Size(668, 92);
            this.backupProgressTextBox.TabIndex = 5;
            this.backupProgressTextBox.Text = "Add the directory where the backup file will be saved.";
            // 
            // backupButton
            // 
            this.backupButton.Location = new System.Drawing.Point(7, 111);
            this.backupButton.Name = "backupButton";
            this.backupButton.Size = new System.Drawing.Size(75, 23);
            this.backupButton.TabIndex = 4;
            this.backupButton.Text = "Backup";
            this.backupButton.UseVisualStyleBackColor = true;
            this.backupButton.Click += new System.EventHandler(this.backupButton_Click);
            // 
            // backupEverquestDirButton
            // 
            this.backupEverquestDirButton.Location = new System.Drawing.Point(539, 31);
            this.backupEverquestDirButton.Name = "backupEverquestDirButton";
            this.backupEverquestDirButton.Size = new System.Drawing.Size(136, 23);
            this.backupEverquestDirButton.TabIndex = 3;
            this.backupEverquestDirButton.Text = "Change Everquest Dir";
            this.backupEverquestDirButton.UseVisualStyleBackColor = true;
            this.backupEverquestDirButton.Click += new System.EventHandler(this.backupEverquestDirButton_Click);
            // 
            // backupDirButton
            // 
            this.backupDirButton.Location = new System.Drawing.Point(539, 5);
            this.backupDirButton.Name = "backupDirButton";
            this.backupDirButton.Size = new System.Drawing.Size(136, 23);
            this.backupDirButton.TabIndex = 2;
            this.backupDirButton.Text = "Add Backup Dir";
            this.backupDirButton.UseVisualStyleBackColor = true;
            this.backupDirButton.Click += new System.EventHandler(this.backupDirButton_Click);
            // 
            // backupEverquestDirTextBox
            // 
            this.backupEverquestDirTextBox.Location = new System.Drawing.Point(7, 34);
            this.backupEverquestDirTextBox.Name = "backupEverquestDirTextBox";
            this.backupEverquestDirTextBox.Size = new System.Drawing.Size(526, 20);
            this.backupEverquestDirTextBox.TabIndex = 1;
            this.backupEverquestDirTextBox.Text = "C:\\Users\\Public\\Daybreak Game Company\\Installed Games\\EverQuest";
            // 
            // backupDirTextBox
            // 
            this.backupDirTextBox.Location = new System.Drawing.Point(7, 8);
            this.backupDirTextBox.Name = "backupDirTextBox";
            this.backupDirTextBox.Size = new System.Drawing.Size(526, 20);
            this.backupDirTextBox.TabIndex = 0;
            // 
            // restoreTabPage
            // 
            this.restoreTabPage.Controls.Add(this.restoreProgressRichTextBox);
            this.restoreTabPage.Controls.Add(this.restoreButton);
            this.restoreTabPage.Controls.Add(this.restoreEverquestDirTextBox);
            this.restoreTabPage.Controls.Add(this.restoreFileTextBox);
            this.restoreTabPage.Controls.Add(this.restoreEverquestDirButton);
            this.restoreTabPage.Controls.Add(this.restoreFileButton);
            this.restoreTabPage.Location = new System.Drawing.Point(4, 22);
            this.restoreTabPage.Name = "restoreTabPage";
            this.restoreTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.restoreTabPage.Size = new System.Drawing.Size(681, 236);
            this.restoreTabPage.TabIndex = 1;
            this.restoreTabPage.Text = "Restore";
            this.restoreTabPage.UseVisualStyleBackColor = true;
            // 
            // restoreProgressRichTextBox
            // 
            this.restoreProgressRichTextBox.Location = new System.Drawing.Point(7, 90);
            this.restoreProgressRichTextBox.Name = "restoreProgressRichTextBox";
            this.restoreProgressRichTextBox.ReadOnly = true;
            this.restoreProgressRichTextBox.Size = new System.Drawing.Size(668, 142);
            this.restoreProgressRichTextBox.TabIndex = 6;
            this.restoreProgressRichTextBox.Text = "Add the directory where the files to be restored are saved.";
            // 
            // restoreButton
            // 
            this.restoreButton.Location = new System.Drawing.Point(7, 60);
            this.restoreButton.Name = "restoreButton";
            this.restoreButton.Size = new System.Drawing.Size(75, 23);
            this.restoreButton.TabIndex = 4;
            this.restoreButton.Text = "Restore";
            this.restoreButton.UseVisualStyleBackColor = true;
            this.restoreButton.Click += new System.EventHandler(this.restoreButton_Click);
            // 
            // restoreEverquestDirTextBox
            // 
            this.restoreEverquestDirTextBox.Location = new System.Drawing.Point(7, 33);
            this.restoreEverquestDirTextBox.Name = "restoreEverquestDirTextBox";
            this.restoreEverquestDirTextBox.Size = new System.Drawing.Size(531, 20);
            this.restoreEverquestDirTextBox.TabIndex = 3;
            this.restoreEverquestDirTextBox.Text = "C:\\Users\\Public\\Daybreak Game Company\\Installed Games\\EverQuest";
            // 
            // restoreFileTextBox
            // 
            this.restoreFileTextBox.Location = new System.Drawing.Point(7, 7);
            this.restoreFileTextBox.Name = "restoreFileTextBox";
            this.restoreFileTextBox.Size = new System.Drawing.Size(531, 20);
            this.restoreFileTextBox.TabIndex = 2;
            // 
            // restoreEverquestDirButton
            // 
            this.restoreEverquestDirButton.Location = new System.Drawing.Point(544, 30);
            this.restoreEverquestDirButton.Name = "restoreEverquestDirButton";
            this.restoreEverquestDirButton.Size = new System.Drawing.Size(131, 23);
            this.restoreEverquestDirButton.TabIndex = 1;
            this.restoreEverquestDirButton.Text = "Change Everquest Dir";
            this.restoreEverquestDirButton.UseVisualStyleBackColor = true;
            this.restoreEverquestDirButton.Click += new System.EventHandler(this.restoreEverquestDirButton_Click);
            // 
            // restoreFileButton
            // 
            this.restoreFileButton.Location = new System.Drawing.Point(544, 4);
            this.restoreFileButton.Name = "restoreFileButton";
            this.restoreFileButton.Size = new System.Drawing.Size(131, 23);
            this.restoreFileButton.TabIndex = 0;
            this.restoreFileButton.Text = "Use Restore File";
            this.restoreFileButton.UseVisualStyleBackColor = true;
            this.restoreFileButton.Click += new System.EventHandler(this.restoreFileButton_Click);
            // 
            // restoreOpenFileDialog
            // 
            this.restoreOpenFileDialog.FileName = "openFileDialog1";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.licenseToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(714, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // licenseToolStripMenuItem
            // 
            this.licenseToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem1});
            this.licenseToolStripMenuItem.Name = "licenseToolStripMenuItem";
            this.licenseToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.licenseToolStripMenuItem.Text = "Help";
            // 
            // aboutToolStripMenuItem1
            // 
            this.aboutToolStripMenuItem1.Name = "aboutToolStripMenuItem1";
            this.aboutToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.aboutToolStripMenuItem1.Text = "About";
            this.aboutToolStripMenuItem1.Click += new System.EventHandler(this.aboutToolStripMenuItem1_Click);
            // 
            // redGuidesLinkLabel
            // 
            this.redGuidesLinkLabel.AutoSize = true;
            this.redGuidesLinkLabel.Location = new System.Drawing.Point(17, 28);
            this.redGuidesLinkLabel.Name = "redGuidesLinkLabel";
            this.redGuidesLinkLabel.Size = new System.Drawing.Size(83, 13);
            this.redGuidesLinkLabel.TabIndex = 2;
            this.redGuidesLinkLabel.TabStop = true;
            this.redGuidesLinkLabel.Text = "RedGuides.com";
            this.redGuidesLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.redGuidesLinkLabel_LinkClicked);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(714, 310);
            this.Controls.Add(this.redGuidesLinkLabel);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "EQBackupRestore";
            this.tabControl1.ResumeLayout(false);
            this.backupTabPage.ResumeLayout(false);
            this.backupTabPage.PerformLayout();
            this.restoreTabPage.ResumeLayout(false);
            this.restoreTabPage.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage backupTabPage;
        private System.Windows.Forms.Button backupEverquestDirButton;
        private System.Windows.Forms.Button backupDirButton;
        private System.Windows.Forms.TextBox backupEverquestDirTextBox;
        private System.Windows.Forms.TextBox backupDirTextBox;
        private System.Windows.Forms.TabPage restoreTabPage;
        private System.Windows.Forms.TextBox restoreEverquestDirTextBox;
        private System.Windows.Forms.TextBox restoreFileTextBox;
        private System.Windows.Forms.Button restoreEverquestDirButton;
        private System.Windows.Forms.Button restoreFileButton;
        private System.Windows.Forms.Button backupButton;
        private System.Windows.Forms.Button restoreButton;
        private System.Windows.Forms.FolderBrowserDialog backupDirFolderBrowserDialog;
        private System.Windows.Forms.FolderBrowserDialog everquestDirFolderBrowserDialog;
        private System.Windows.Forms.OpenFileDialog restoreOpenFileDialog;
        private System.Windows.Forms.Timer restoreTimer;
        private System.Windows.Forms.TextBox backupProgressTextBox;
        private System.Windows.Forms.RichTextBox restoreProgressRichTextBox;
        private System.Windows.Forms.CheckBox backupVoicechatCheckBox;
        private System.Windows.Forms.CheckBox backupDefaultsCheckBox;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem licenseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem1;
        private System.Windows.Forms.LinkLabel redGuidesLinkLabel;
    }
}

