using System;
using System.IO;
using System.Windows.Forms;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip;

/**
 * <remarks>zipCreateFolder and zipCompressFolder are based on code from
 * https://github.com/icsharpcode/SharpZipLib/wiki/Zip-Samples,
 * distributed under the MIT License.</remarks>
 */
namespace EQBackupRestore
{
    public partial class Form1 : Form
    {
        /**
         * <summary>Compresses the files in the nominated folder, and creates a zip file on disk named as outPathname.</summary>
         * <param name="outPathname">Target zip file on disk</param>
         * <param name="password">password for Zip file, null for none</param>
         * <param name="folderName">The folder to compress</param>
         */
        public void zipCreateFolder(string outPathname, string password, string folderName)
        {
            FileStream fsOut = File.Create(outPathname);
            ZipOutputStream zipStream = new ZipOutputStream(fsOut);

            zipStream.SetLevel(3); //0-9, 9 being the highest level of compression

            zipStream.Password = password;  // optional. Null is the same as not setting. Required if using AES.

            /* This setting will strip the leading part of the folder path in the entries, to
             * make the entries relative to the starting folder.
             * To include the full path for each entry up to the drive root,
             * assign folderOffset = 0.
             */
            int folderOffset = folderName.Length + (folderName.EndsWith("\\") ? 0 : 1);
            //int folderOffset = backupRootDir.FullName.Length + (backupRootDir.FullName.EndsWith("\\") ? 0 : 1);

            zipCompressFolder(folderName, zipStream, folderOffset);

            zipStream.IsStreamOwner = true; // Makes the Close also Close the underlying stream
            zipStream.Close();
        }

        /**
         * <summary>Recurses down the folder structure</summary>
         */
        private void zipCompressFolder(string path, ZipOutputStream zipStream, int folderOffset)
        {

            string[] files = Directory.GetFiles(path);

            foreach (string filename in files)
            {

                FileInfo fi = new FileInfo(filename);

                string entryName = filename.Substring(folderOffset); // Makes the name in zip based on the folder
                entryName = ZipEntry.CleanName(entryName); // Removes drive from name and fixes slash direction
                ZipEntry newEntry = new ZipEntry(entryName);
                newEntry.DateTime = fi.LastWriteTime; // Note the zip format stores 2 second granularity

                // Specifying the AESKeySize triggers AES encryption. Allowable values are 0 (off), 128 or 256.
                // A password on the ZipOutputStream is required if using AES.
                //   newEntry.AESKeySize = 256;

                // To permit the zip to be unpacked by built-in extractor in WinXP and Server2003, WinZip 8, Java, and other older code,
                // you need to do one of the following: Specify UseZip64.Off, or set the Size.
                // If the file may be bigger than 4GB, or you do not need WinXP built-in compatibility, you do not need either,
                // but the zip will be in Zip64 format which not all utilities can understand.
                //   zipStream.UseZip64 = UseZip64.Off;
                newEntry.Size = fi.Length;

                zipStream.PutNextEntry(newEntry);

                // Zip the file in buffered chunks
                // the "using" will close the stream even if an exception occurs
                byte[] buffer = new byte[4096];
                using (FileStream streamReader = File.OpenRead(filename))
                {
                    StreamUtils.Copy(streamReader, zipStream, buffer);
                }
                zipStream.CloseEntry();
            }
            string[] folders = Directory.GetDirectories(path);
            foreach (string folder in folders)
            {
                zipCompressFolder(folder, zipStream, folderOffset);
            }
        }
    }
}
