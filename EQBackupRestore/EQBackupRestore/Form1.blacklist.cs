using System;
using System.IO;
using System.Windows.Forms;

namespace EQBackupRestore
{
    public partial class Form1 : Form
    {
        /* 
         * <summary>Backup all *.ini files in root and all files in userdata/ unless blacklisted</summary>
         */
        public void backupBlacklist(DirectoryInfo sourceDir)
        {
            /* "Attempting to backup eqclient file." */
            logBackupTitle("Backing up EverQuest *.ini files");

            String[] backupBlacklist = new String[] {
                "eqlsUIConfig.ini", //Has screen position information that shouldn't transfer between computers
                "eqlsClient.ini", // Everquest Help URLS
                "LaunchPad-user.ini",
                "LaunchPad.ini",
                "Uninstaller.ini"
            };

            FileInfo[] Files = sourceDir.GetFiles("*.ini");
            foreach (FileInfo file in Files)
            {
                Boolean isBlacklisted = false;
               
                /* Check the file against each file in the Blacklist */
                foreach (String blackFilename in backupBlacklist)
                {
                    if (blackFilename == file.Name)
                        isBlacklisted = true;
                }
                if (! backupDefaultsCheckBox.Checked && "defaults.ini" == file.Name)
                    isBlacklisted = true;
                if (! backupVoicechatCheckBox.Checked && "VoiceChat.ini" == file.Name)
                    isBlacklisted = true;

                if (isBlacklisted)
                {
                    logBackup("Skipped: " + file.Name);
                }
                else
                {
                    String sourceFileName = file.FullName;
                    String destFileName = Path.Combine(backupDir.FullName, file.Name);

                    if (File.Exists(sourceFileName))
                    {
                        File.Copy(sourceFileName, destFileName);
                        logBackup("Copied: " + file.Name);
                    }

                }
            }

            /**
             * <remarks>Userdata backup section begins here, and contains Audio triggers, user configs, and addressbook/ignorelist text files</remarks>
             */
            logBackupTitle("Backing up EverQuest userdata files");
            backupUserdataFilePattern("*.*");
        }

    }
}
