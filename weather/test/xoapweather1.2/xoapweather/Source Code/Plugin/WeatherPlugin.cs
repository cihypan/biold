/*
 * XoapWeather Client Plugin for XLobby2
 * Copyright (c) 2004 Jonathan Bradshaw
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. 
 */
using System;
using System.Collections;
using System.Diagnostics;
using System.Windows.Forms;
using System.Drawing;
using System.Threading;
using System.Xml;
using System.Data;
using System.IO;
using System.Net;
using System.Text.RegularExpressions;

using XPluginSDK;
using XoapWeather.Entity;
using XoapWeather.Provider;

namespace XoapWeather.Plugin
{
	/// <summary>
	/// The Xoap Weather Plugin for Xlobby2 provides weather text and images from
	/// Weather.com through the Xoap.Weather.Com XML interface.
	/// </summary>
	public class WeatherPlugin : XPlugin
	{
		#region Member Constants
		/// <summary>
		/// Plugin information
		/// </summary>
		private const string PLUGIN_NAME		= "XoapWeather";
		private const string PLUGIN_TYPE		= "Weather";
		private const string PLUGIN_DESCRIPTION = "Weather Plugin";

		/// <summary>
		/// Filenames used for configuration files.
		/// These are marked internal to allow the configuration class to access them.
		/// </summary>
		internal const string FORECASTS_XML  = "forecasts.xml";
		internal const string REGIONMAPS_XML = "regionmaps.xml";
		internal const string VARIABLES_XML  = "variables.xml";

		// XPath Query used to determine if there are severe weather alerts active
		private const string SWA_XPATH = "/weather/swa/a[@uc=1]";
		#endregion

		#region Member Variables
		/// <summary>
		/// Private variables used to hold plugin state
		/// </summary>
		private WeatherRegionForecast[]     wxRegions;
		private DataTable					wxVariables;
		private readonly Thread				wxUpdateThread;
		private int							wxPointer = 0;
		private XPluginHelper				xHelper;
		private string						xStatus = "Starting";
		private	readonly VariableParser		variableParser = new VariableParser();
		#endregion

		#region Constructors
		/// <summary>
		/// Called from XLobby to create a new <see cref="WeatherPlugin"/> instance.
		/// </summary>
		public WeatherPlugin()
		{
			Logger.Start();
			Trace.WriteLine("Starting XoapWeather Plugin", "plugin");

			// Set the default weather provider (must be done prior to config load)
			WeatherRegionForecast.DefaultWeatherProvider = XoapProvider.Instance;

			// Read configuration and set the status flag
			this.xStatus = (ReadConfiguration() ? "Active" : "Error");

			// Set up the background update thread
			wxUpdateThread = new Thread(new ThreadStart(UpdateForecastsThread));
			wxUpdateThread.Name = "Weather Updater";
			wxUpdateThread.IsBackground = true;
			wxUpdateThread.Priority = System.Threading.ThreadPriority.Lowest;

		}
		#endregion

		#region XPlugin Methods
		/// <summary>
		/// <para>
		/// GetVariable provides the main interface from Xlobby button text variables to the actual data.
		/// The format of the button text should be either:
		/// <code>
		///		plugin>xoapweather>[regionid]>[imagevariable|%textvariable%]
		///	or  plugin>xoapweather>[regionid]>[daynum]>[imagevariable|%textvariable%]
		///	</code>
		///	</para>
		///	<example>
		///	For example:
		///	<code>
		///	    plugin>xoapweather>current>radar  will return the radar image for the current region forecast
		///	    plugin>xoapweather>0>1>High: %high% will return the high temp for tomorrow for the first region
		///	</code>
		///	</example>
		///	<para>
		///	Specifying a variable with no percent (%) signs indicates an image is to be returned otherwise you
		///	can mix static text and variable replacements in the string. If the high temperature was 79 the 
		///	above example will return a string of "High: 79".
		///	</para>
		///	<para>
		///	The following variables are hardcoded:
		///	<code>
		///		radar        - The radar image (the url can be set in the configuration screen)
		///		weatheralert - A weather alert image (shown if there is an active alert for region)
		///	</code>
		///	</para>
		///	<para>
		///	All other image and text variables are defined in the configuration utility specifying the name of
		///	the variable and the XPath to use to query the weather.com XML. 
		///	</para>
		///	<para>
		///	If a day number is specified, you can
		///	use the '#' character in the XPath to have it replaced with the day value. If the variable requested is
		///	for an image (it does not have % marks around it) then the resulting XPath query is assumed to be an
		///	icon index number and the actual icon image is returned.
		///	</para>
		/// </summary>
		/// <param name="name">Requested variable name</param>
		/// <returns>String text or an Image</returns>
		public Object GetVariable(string name)
		{
			int            forecastNum = wxPointer;
			string[]       args;
			bool		   isImage;

			// Set up
			args = name.Split('>');
			// Text uses %variable%, images use variable name without percent marks
			isImage = (name.IndexOf('%') < 0);

			// Check to see if we have any locations defined
			if (wxRegions == null || wxRegions.Length == 0) 
				return "No weather locations defined";

			// Check we have enough parameters
			if (args.Length < 2) 
				return "Invalid number of parameters";

			// Set the forecast number
			if (args[0] == "current") 
				args[0] = forecastNum.ToString();
			else try 
			{ 
				forecastNum = Convert.ToInt16(args[0]);
			}
			catch (FormatException) 
			{
				return "Invalid forecast location number specified";
			}				

			// Check for an out of range forecast number
			if (forecastNum < 0 || forecastNum >= wxRegions.Length) 
				forecastNum = 0;

			// Check to see if we have any weather xml data
			if (! wxRegions[forecastNum].IsLoaded)
			{
				if (isImage)
					return GetWeatherIcon("NOCONNECTION");
				else
					return "n/c";
			}
	
			// Check for request for radar image
			if (args[1] == "radar") 
			{
				Image radar = null;
				string mapUrl = wxRegions[forecastNum].RadarMap;
				if (mapUrl.Length  == 0) 
					return GetWeatherIcon("NOCONNECTION");
				else
				{
					radar = GetRegionMap(new Uri(mapUrl));
					return (radar == null) ? GetWeatherIcon("NOCONNECTION") : radar;
				}
			}

			// The alert image is shown if there are any severe weather alerts in the XML
			if (args[1] == "weatheralert")
			{
				if (wxRegions[forecastNum].Evaluate(SWA_XPATH).Length > 0)
				{
					Trace.WriteLine("Active Weather Alert Detected", "plugin");
					return GetWeatherIcon("WEATHERALERT");
				}
				else
					return String.Empty; // This blanks out the image, returning null doesn't seem to work
			}

			// Check if hourly.  This is temporary solution so users can separate an hourly
 			// number from a daily number and maintain backwards compatibility (for now)
			if (args[1] == "hourly")
			{
				args[1] = args[2] ;
				args[2] = "" ;
			}
				

			string key = String.Join(String.Empty, args);
			string text = Cache.Instance.Get(key) as string;
			if (text == null)
			{	
				string replace = (args.Length > 2 ? args[1] : "0");
				text = variableParser.Parse(wxRegions[forecastNum], replace, wxVariables, args[args.Length-1]);
				Cache.Instance.Insert(key, text, CacheItemLifetime.Permanent);
				Trace.WriteLine("Parsing: " + args[args.Length-1] + " -> " + text, "plugin");
			}
			// Check to see if we should return the text or an image (using the text filename)
			if (isImage)
				return GetWeatherIcon(text);
			else
				return text;
		}

		/// <summary>
		/// This method is triggered through the plugin manager and will show the
		/// XoapWeather configuration dialog. If the dialogue is closed normally
		/// the configuration will be reloaded and new weather data requested.
		/// </summary>
		public void ShowDialog()
		{
			ConfigurationForm cfg = new ConfigurationForm(xHelper);
			cfg.TopMost = true;
			if (cfg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				// Reload the configuration and forecast data
				if (ReadConfiguration())
				{
					ForceWeatherUpdate();
					this.xStatus = "Active";
				}
				else
					this.xStatus = "Error";
			}
		}

		/// <summary>
		/// This method returns an ArrayList of accepted plugin command strings used in 
		/// the event manager to allow the end user to see all the commands available.
		///
		/// Currently the following commands are available:
		/// <code>
		///		refresh forecasts
		///		refresh forecasts (background)
		///		next forecast
		///		previous forecast
		///		dump xml to file
		/// </code>
		/// </summary>
		/// <returns>ArrayList of strings or null if there are no commands</returns>
		public ArrayList GetCommands()
		{
			Trace.WriteLine("Command list requested", "plugin");
			ArrayList commands = new ArrayList();
			commands.Add("refresh forecasts");
			commands.Add("next forecast");
			commands.Add("previous forecast");
			commands.Add("dump xml to file");
			return commands;
		}

		/// <summary>
		/// This method provides the implementation for the event 
		/// manager to request a command be executed by this plugin.
		/// See the <see cref="GetCommands">GetCommands()</see> method 
		/// for a list of valid commands.
		/// </summary>
		/// <param name="command">The command string to be executed</param>
		public void Execute(string command)
		{
			switch (command)
			{
				case "refresh forecasts":
					Trace.WriteLine("refresh forecasts", "command");
					ForceWeatherUpdate();
					break;

				case "next forecast":
					wxPointer++;
					if (wxPointer >= wxRegions.Length) wxPointer = 0;
					Trace.WriteLine("next forecast - Switching to forecast " + wxPointer, "command");
					break;

				case "previous forecast":
					wxPointer--;
					if (wxPointer < 0) wxPointer = wxRegions.Length-1;
					Trace.WriteLine("previous forecast - Switching to forecast " + wxPointer, "command");
					break;

				case "dump xml to file":
					Trace.WriteLine("Dump weather xml to xoapdump.xml", "command");
					if (wxRegions != null && wxRegions[wxPointer] != null)
					{
						XmlTextWriter writer = new XmlTextWriter(Helper.GetPath("xoapdump.xml"), System.Text.Encoding.UTF8);
						writer.Formatting = Formatting.Indented;
						wxRegions[wxPointer].WriteTo(writer);
						writer.Close();
					}
					break;
				default:
					Trace.WriteLine("Unknown command received: " + command, "command");
					break;
			}
		}

		/// <summary>
		/// Returns the name of this plugin.
		/// </summary>
		public override string ToString()
		{
			return this.GetName();
		}

		/// <summary>
		/// Provides registration information about this module to Xlobby
		/// for display in the Xlobby plugin setup configuration dialog.
		/// </summary>
		public string GetName()
		{
			return PLUGIN_NAME;
		}

		/// <summary>
		/// Provides registration information about this module to Xlobby
		/// for display in the Xlobby plugin setup configuration dialog.
		/// </summary>
		public string GetDescription()
		{
			return String.Format("{0} v{1}", PLUGIN_DESCRIPTION, Helper.GetVersion());
		}

		/// <summary>
		/// Provides registration information about this module to Xlobby
		/// for display in the Xlobby plugin setup configuration dialog.
		/// </summary>
		public string GetPluginType()
		{
			return PLUGIN_TYPE;
		}

		/// <summary>
		/// Provides registration information about this module to Xlobby
		/// for display in the Xlobby plugin setup configuration dialog.
		/// </summary>
		public string GetStatus()
		{
			return this.xStatus;
		}

		/// <summary>
		/// This method is called by Xlobby to pass the plugin SDK helper object
		/// to us. Upon receiving this XoapWeather will initiate a background
		/// update of forecasts.
		/// </summary>
		/// <param name="xhelper">Helper object</param>
		public void SetHelper(XPluginHelper xhelper)
		{			
			// Store the pointer to the xhelper object
			this.xHelper = xhelper;
			// Start the background update thread
			if ((wxUpdateThread.ThreadState & System.Threading.ThreadState.Unstarted) == System.Threading.ThreadState.Unstarted) 
			{
				Trace.WriteLine("Starting background weather update thread", "plugin");
				wxUpdateThread.Start();
			}
		}
		#endregion

		#region Private Methods
		/// <summary>
		/// Initializes the plugin by loading the configuration files
		/// </summary>
		private bool ReadConfiguration()
		{
			Trace.WriteLine("Reading configuration files", "plugin");
			try
			{
				PluginConfig cfg = (PluginConfig) Helper.DeserializeObject(WeatherPlugin.FORECASTS_XML, typeof(PluginConfig));
				Trace.WriteLineIf(cfg.Upgrade(), "Configuration file upgraded", "plugin");
				wxRegions = cfg.Regions;
				wxVariables = Helper.DeserializeTable(WeatherPlugin.VARIABLES_XML);
				return true;
			}
			catch (SystemException ex) 
			{ 
				Trace.WriteLine("Error: " + ex.GetBaseException().Message, "error");
				return false;
			}
		}

		/// <summary>
		/// Gets the weather icon for ths specified ID number. 
		/// </summary>
		/// <param name="iconId">Icon id.</param>
		/// <returns>Image object</returns>
		private Image GetWeatherIcon(string iconId)
		{
			string filename = Helper.GetPath(@"images\" + iconId + ".png");
			Image  image    = Cache.Instance.Get(filename) as Image;

			if (image == null && File.Exists(filename))
			{
				image = Image.FromFile(filename, true);
				Cache.Instance.Insert(filename, image, CacheItemLifetime.Permanent);
			}
			return image;
		}

		/// <summary>
		/// Gets the region map.
		/// </summary>
		/// <param name="url">URL.</param>
		/// <returns></returns>
		private Image GetRegionMap(Uri url)
		{
			Image map = Cache.Instance.Get(url) as Image;
			if (map == null)
				try 
				{
					Stream ImageStream = new WebClient().OpenRead(url.AbsoluteUri);
					map = Image.FromStream(ImageStream);
					ImageStream.Close();
					Cache.Instance.Insert(url, map, CacheItemLifetime.Permanent);
				}
				catch {
					map = GetWeatherIcon("NOCONNECTION");
				}
			return map;
		}

		private void ForceWeatherUpdate()
		{
			wxUpdateThread.Interrupt();
		}

		/// <summary>
		/// Updates the forecasts (implemented on a seperate thread).
		/// </summary>
		private void UpdateForecastsThread()
		{
			DateTime nextUpdate = DateTime.MaxValue;
			// This thread never dies
			while (true)
			{
				if (wxRegions == null || wxRegions.Length == 0)
					Trace.WriteLine("Not updating weather - No weather locations defined", "wxupdate");
				else if (xHelper != null && xHelper.SendCommand("connectedToInternet") == "false") 
					Trace.WriteLine("Not updating weather - No internet connection available", "wxupdate");
				else
				{
					// Start weather data update
					Trace.WriteLine("Starting weather forecast data update", "wxupdate");
					nextUpdate = DateTime.MaxValue;
					foreach (WeatherRegionForecast wxRegion in wxRegions)
						try 
						{
							Trace.WriteLine("Retrieving weather data for " + wxRegion.LocationId, "wxupdate");
							DateTime regionUpdate = wxRegion.Refresh();
							if (DateTime.Compare(regionUpdate, nextUpdate) < 0)
								nextUpdate = regionUpdate;
						}
						catch (ApplicationException ex)
						{
							Trace.WriteLine("Error: " + wxRegion.LocationId + ": " + ex.Message, "wxupdate");
						}
						finally 
						{
							if (!wxRegion.IsLoaded)
								Trace.WriteLine("Error: No data is loaded for " + wxRegion.LocationId, "wxupdate");
						}
					// Clear the cache so we don't have stale data in it
					Cache.Instance.Clear();
					if (xHelper != null) 
					{
						Trace.WriteLine("Requesting screen update", "wxupdate");
						xHelper.SendCommand("invalidate");
					}
				}
				// If we don't know when to update (maybe we are offline), then try again in 5 minutes
				if (nextUpdate == DateTime.MaxValue || nextUpdate <= DateTime.Now)
					nextUpdate = DateTime.Now.AddMinutes(5);
				Trace.WriteLine("Next weather data update at " + nextUpdate, "wxupdate");
				try 
				{
					Thread.Sleep(nextUpdate - DateTime.Now);
				}
				catch (ThreadInterruptedException) 
				{
					Trace.WriteLine("Immediate weather update forced", "wxupdate");
				}
			}
		}
		#endregion
	}
}
