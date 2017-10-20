using System;
using System.IO;
using System.Windows.Forms;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip;

namespace EQBackupRestore
{
    public partial class Form1 : Form
    {
        /**
          * <summary>Backup all files in the Everquest root directory matching a pattern</summary>
          * <param name="filePattern">a Windows Cmdline file*name pattern</param>
          */
        private void backupFilePattern(String filePattern)
        {
            FileInfo[] Files = everquestDir.GetFiles(filePattern);
            foreach (FileInfo file in Files)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(backupDir.FullName, file.Name);
                File.Copy(sourceFileName, destFileName);
                logBackup(file.Name);
            }
        }

        /** 
          * <summary>Backup the given file path</summary>
          * <param name="fileName">The non-path name of the file to be archived</param>
          */
        private void backupFileName(String fileName)
        {
            FileInfo file = new FileInfo(Path.Combine(everquestDir.FullName, fileName));
            if (file.Exists)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(backupDir.FullName, fileName);
                File.Copy(sourceFileName, destFileName);
                logBackup(file.Name);
            }
        }

        /**
         * <summary>Backup all files in the Everquest root/userdate directory matching a pattern</summary>
         * <param name="filePattern">a Windows Cmdline file*name pattern</param>
         */
        private void backupUserdataFilePattern(String filePattern)
        {
            String userdataDirPath = Path.Combine(everquestDir.FullName, "userdata");
            DirectoryInfo userdataDir = new DirectoryInfo(userdataDirPath);
            String backupUserdataDirPath = Path.Combine(backupDir.FullName, "userdata");
            DirectoryInfo backupUserdataDir = new DirectoryInfo(backupUserdataDirPath);

            if (Directory.Exists(userdataDirPath))
            {
                if (!backupUserdataDir.Exists)
                    backupUserdataDir.Create();

                FileInfo[] Files = userdataDir.GetFiles(filePattern);
                foreach (FileInfo file in Files)
                {
                    String sourceFileName = file.FullName;
                    String destFileName = Path.Combine(backupUserdataDir.FullName, file.Name);
                    File.Copy(sourceFileName, destFileName);
                    logBackup("userdata > " + file.Name);
                }
            }
            else
            {
                logBackup("***EverQuest Userdata directory not found.");
            }
        }

        /**
         * <summary>The main method for backing up files - called when backupButton is clicked</summary>
         */
        private void backupFiles()
        {
            if (null == everquestDir)
            {
                everquestDir = new DirectoryInfo(backupEverquestDirTextBox.Text);
            }
            String zipDirName = "EQBackup_" + DateTime.Now.ToString("yyyyMMdd_hhmmss");
            String zipFileName = zipDirName + ".zip";
            backupDir = backupRootDir.CreateSubdirectory(zipDirName);
            String zipPath = Path.Combine(backupRootDir.FullName, zipFileName);

            backupBlacklist(everquestDir);

            zipCreateFolder(zipPath, null, backupDir.FullName);
            Directory.Delete(backupDir.FullName, true);
            logBackupTitle("Backup Complete!");
        }

        /**
         * <summary>Check to make sure the given EverQuest directory contains both the .exe file and the userdata directory</summary>
         */
        private Boolean checkEverquestDirOk(String eqTenativeDirPath)
        {
            String eqexePath = Path.Combine(eqTenativeDirPath, "eqgame.exe");
            String userdataDirPath = Path.Combine(eqTenativeDirPath, "userdata");
            if (File.Exists(eqexePath) && Directory.Exists(userdataDirPath))
                return true;
            else
                return false;
        }

        /**
         * <summary>Restore all files to the Everquest root directory matching a pattern</summary>
         * <param name="filePattern">a Windows Cmdline file*name pattern</param>
         */
        private void dummyRestoreFilePattern(String filePattern)
        {
            logRestore("File Pattern: " + filePattern);
            logRestore("Backup Dir: " + backupDir.FullName);

            FileInfo[] Files = backupDir.GetFiles(filePattern);
            foreach (FileInfo file in Files)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(everquestDir.FullName, file.Name);

                if (File.Exists(destFileName))
                {
                    logRestore(sourceFileName);
                    logRestore(destFileName);
                    logRestore("Replaced: " + file.Name);
                }
                else
                {
                    logRestore("Copied: " + file.Name);
                }
            }
        }

        /**
         * <summary>Restore all files to the Everquest root directory matching a pattern</summary>
         * <param name="filePattern">a Windows Cmdline file*name pattern</param>
         */
        private void restoreFilePattern(String filePattern)
        {
            FileInfo[] Files = backupDir.GetFiles(filePattern);
            foreach (FileInfo file in Files)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(everquestDir.FullName, file.Name);

                if (File.Exists(destFileName))
                {
                    File.Copy(sourceFileName, destFileName, true);
                    logRestore("Replaced: " + file.Name);
                }
                else
                {
                    File.Copy(sourceFileName, destFileName);
                    logRestore("Copied: " + file.Name);
                }
            }
        }

        /** 
         * <summary>Restore the given file path</summary>
         * <param name="fileName">The non-path name of the file to be restored</param>
         */
        private void restoreFileName(String fileName)
        {
            FileInfo file = new FileInfo(Path.Combine(backupDir.FullName, fileName));
            if (file.Exists)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(everquestDir.FullName, fileName);

                if (File.Exists(destFileName))
                {
                    File.Copy(sourceFileName, destFileName, true);
                    logRestore("Replaced: " + file.Name);
                }
                else
                {
                    File.Copy(sourceFileName, destFileName);
                    logRestore("Copied: " + file.Name);
                }
            }
        }

        /**
         * <summary>Restore all files to the Everquest root directory matching a pattern</summary>
         * <param name="filePattern">a Windows Cmdline file*name pattern</param>
         */
        private void restoreUserdata()
        {
            FileInfo[] Files = backupDir.GetFiles("userdata/*.*");
            String eqUserdataDirPath = Path.Combine(everquestDir.FullName, "userdata");
            DirectoryInfo eqUserdataDir = new DirectoryInfo(eqUserdataDirPath);

            /* The EverQuest userdata directory should exist as a precondition to restoring 
             * data, but it's possible the user forced the program to use an EverQuest directory
             * without a userdata/ subdirectory. In that case, create a new userdata/ subdirectory.
             */
            if (!eqUserdataDir.Exists) 
                eqUserdataDir.Create();

            foreach (FileInfo file in Files)
            {
                String sourceFileName = file.FullName;
                String destFileName = Path.Combine(eqUserdataDir.FullName, file.Name);

                if (File.Exists(destFileName))
                {
                    File.Copy(sourceFileName, destFileName, true);
                    logRestore("Replaced: userdata > " + file.Name);
                }
                else
                {
                    File.Copy(sourceFileName, destFileName);
                    logRestore("Copied: userdata > " + file.Name);
                }
            }
        }

        /** 
         * <summary>Restore the given file path</summary>
         * <param name="fileName">The non-path name of the file to be restored</param>
         */
        private void restoreFiles() //(DirectoryInfo backupDir)
        {
            /* https://github.com/icsharpcode/SharpZipLib/wiki/FastZip */
            FastZip fastZip = new FastZip();
            String fileFilter = null;

            // Will always overwrite if target filenames already exist
            fastZip.ExtractZip(restoreFile.FullName, backupDir.FullName, fileFilter);

            // Copy files to everquest Dir
            restoreFilePattern("*.ini");
            restoreUserdata();
            backupDir.Delete(true);
            logRestoreTitle("Restore Complete!");
        }
    }
}
