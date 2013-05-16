﻿/***************************************************************************
 *
 * This code is based on this article:
 * http://justinangel.net/WindowsPhone7EmulatorAutomation
 *
 * This program uses ICSharpCode.SharpZipLib library to unzip the XAP file
 * You'll need to add that library (freely downloadable) to the project in
 * order to compile it.
 *
 * https://github.com/icsharpcode/SharpZipLib
 *
 * This program was modified by MoSync in 2013.
 *
 * Original version:
 * http://wpapplauncher.codeplex.com/
 *
 * WP7 App Launcher
 * Copyright (C) 2011 anderZubi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *********************************************************************************/

using System;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Microsoft.SmartDevice.Connectivity;

namespace WPAppLauncher
{
	class Program
	{
		private static void ShowHelp()
		{
			Console.WriteLine("WPAppLauncher.exe [/wait:<filename>] <platform> <target> <yourXapFile.xap>");
			Console.WriteLine("");
			Console.WriteLine("\t/wait causes the launcher to wait until the specified file exists.");
			Console.WriteLine("\t<yourXapFile.xap> is the XAP file you want to launch.");
		}

		static string sPlatform;
		static string sTarget;
		static string waitFile = null;

		static string xapFile;

		static void Main(string[] args)
		{
#if !DEBUG
			try
#endif
			{
				if (args.Length >= 3)
					ProcessOptions(args);
				else
					ShowHelp();
			}
#if !DEBUG
			catch (Exception e)
			{
				Console.WriteLine("Exception: " + e.Message);
			}
#endif
		}

		private static void ProcessOptions(string[] args)
		{
			for (int i = 0; i < args.Length - 3; i++)
			{
				if (args[i].StartsWith("/wait:"))
					waitFile = args[i].Substring(6);
				else
				{
					throw new Exception("Exception: option not recognized: " + args[i]);
				}
			}
			sPlatform = args[args.Length - 3];
			sTarget = args[args.Length - 2];
			xapFile = args[args.Length - 1];
			if (!File.Exists(xapFile))
			{
				throw new Exception("File '" + xapFile + "' does not exist!");
			}
			LaunchApp();
		}

		private static void LaunchApp()
		{
			// Get CoreCon WP7 SDK
			var platforms = new DatastoreManager(1033).GetPlatforms();
			Platform platform;
			try
			{
				platform = platforms.Single(p => p.Name == sPlatform);
			}
			catch (InvalidOperationException e)
			{
				Console.WriteLine("Platform '" + sPlatform + "' not found. Available platforms:");
				foreach (var p in platforms)
				{
					Console.WriteLine(p.Name);
				}
				throw e;
			}


			// Get Emulator / Device
			var devices = platform.GetDevices();
			Device device = null;
			try
			{
				device = devices.Single(d => d.Name == sTarget);
			}
			catch (InvalidOperationException e)
			{
				Console.WriteLine("Target '" + sTarget + "' not found. Available targets:");
				foreach (var d in devices)
				{
					Console.WriteLine(d.Name);
				}
				throw e;
			}

			// Connect to WP7 Emulator / Device
			Console.WriteLine("Connecting to '" + device.Name + "'...");
			device.Connect();
			Console.WriteLine(device + " Connected.");

			// Get AppID.
			var xipFile = new ICSharpCode.SharpZipLib.Zip.ZipFile(xapFile);
			Guid appID = GetAppID(xipFile.GetInputStream(xipFile.GetEntry(("WMAppManifest.xml"))));

			// Remove old version, if installed.
			RemoteApplication app;
			if (device.IsApplicationInstalled(appID))
			{
				Console.WriteLine("Uninstalling XAP...");
				app = device.GetApplication(appID);
				app.Uninstall();
				Console.WriteLine("XAP Uninstalled...");
			}

			// Extract icon.
			Console.WriteLine("Extracting application icon...");
			string extractionFolder = Path.GetTempPath() + Path.DirectorySeparatorChar + Path.GetRandomFileName();
			Directory.CreateDirectory(extractionFolder);
			string iconPath = extractionFolder + Path.DirectorySeparatorChar + "ApplicationIcon.png";
			var iconFile = new FileStream(iconPath, FileMode.Create);
			xipFile.GetInputStream(xipFile.GetEntry("ApplicationIcon.png")).CopyTo(iconFile);
			iconFile.Close();
			Console.WriteLine("Extracted application icon.");

			// Install XAP.
			Console.WriteLine("Installing XAP...");
			app = device.InstallApplication(appID, appID, "NormalApp", iconPath, xapFile);
			Console.WriteLine("XAP installed.");

			// Launch Application.
			Console.WriteLine("Launching app...");
			app.Launch();
			Console.WriteLine("Launched app.");

			// Delete icon.
			Console.WriteLine("Deleting application icon...");
			Directory.Delete(extractionFolder, true);
			Console.WriteLine("Deleted application icon.");

			if (waitFile != null)
			{
				Console.WriteLine("Waiting for the app to create '" + waitFile + "'...");
#if false	// IsRunning() is not implemented.
				while (app.IsRunning())
				{
					System.Threading.Thread.Sleep(1000);
				}
				Console.WriteLine("App has exited, retrieving output...");
				app.GetIsolatedStore().ReceiveFile("log.txt", "log.txt");
#else
				while (true)
				{
					try
					{
						app.GetIsolatedStore().ReceiveFile(waitFile, waitFile);
						break;
					}
					catch (System.IO.FileNotFoundException)
					{
						System.Threading.Thread.Sleep(1000);
					}
#if false
					catch (Exception e)
					{
						Console.WriteLine("Exception: " + e.ToString());
					}
#endif
				}
#endif
				Console.WriteLine(waitFile + " retrieved.");
			}
		}

		static Guid GetAppID(Stream appManifestStream)
		{
			Guid guid = new Guid();

			using (StreamReader sReader = new StreamReader(appManifestStream))
			{
				Regex regex = new Regex(@"(\{{0,1}([0-9a-fA-F]){8}-([0-9a-fA-F]){4}-([0-9a-fA-F]){4}-([0-9a-fA-F]){4}-([0-9a-fA-F]){12}\}{0,1})");

				bool idFound = false;
				string currentLine = sReader.ReadLine();


				while (!idFound && currentLine != null)
				{
					Match m = regex.Match(currentLine);
					if (m.Success)
					{
						idFound = true;
						Guid.TryParse(m.Value, out guid);
						break;
					}
					currentLine = sReader.ReadLine();
				}
			}

			Console.WriteLine("AppID: " + guid.ToString());
			return guid;
		}
	}
}
