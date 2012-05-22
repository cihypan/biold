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
using System.Net;
using System.Drawing;
using System.Collections;
using System.Globalization;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Serialization;
using System.Text.RegularExpressions;

using XoapWeather.Provider;
using XoapWeather.Entity;
using XPluginSDK;

namespace XoapWeather.Plugin
{
	/// <summary>
	/// Provides the weather plugin configuration GUI interface from Xlobby's plugin screen.
	/// </summary>
	public class ConfigurationForm : System.Windows.Forms.Form
	{
		#region Form Controls
		private System.Windows.Forms.TabControl tabControl;
		private System.Windows.Forms.GroupBox forecastGroup;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.NumericUpDown forecastDays;
		private System.Windows.Forms.GroupBox unitBox;
		private System.Windows.Forms.RadioButton rbUnitMetric;
		private System.Windows.Forms.RadioButton rbUnitStandard;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Button btnRemove;
		private System.Windows.Forms.Button btnAdd;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.ListBox forecastRegions;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ListBox searchResults;
		private System.Windows.Forms.Button saveButton;
		private System.Windows.Forms.DataGrid variablesGrid;
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Label versionLabel;
		private System.Windows.Forms.ToolTip toolTip;
		private System.Windows.Forms.TabPage tabLocations;
		private System.Windows.Forms.TabPage tabVariables;
		private System.Windows.Forms.ComboBox radarMap;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox searchText;
		private System.Windows.Forms.Button searchButton;
		private System.Windows.Forms.PictureBox twcLogo;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.ContextMenu radarMenu;
		private System.Windows.Forms.MenuItem GetDefaultMap;
		private System.Windows.Forms.GroupBox radarGroup;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.TabPage tabLog;
		private System.Windows.Forms.ListBox lbEvents;
		private System.Windows.Forms.Button btnStartLogging;
		private System.Windows.Forms.Button btnStopLogging;
		#endregion
		private System.Windows.Forms.Button btnDumpLog;
		private XPluginHelper xHelper;

		/// <summary>
		/// Creates a new <see cref="ConfigurationForm"/> instance.
		/// </summary>
		public ConfigurationForm(XPluginHelper xHelper)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			// Custom Initializations
			ReadLocations();
			radarMap.DataSource = ReadRegionMaps();
			ReadVariables();
			// Check to see if we are in the US to default the unit checkbox correctly
			if (CultureInfo.CurrentCulture.LCID == 1033)
				this.rbUnitStandard.Checked = true;
			else
				this.rbUnitMetric.Checked = true;
			// Set version code
			versionLabel.Text += Helper.GetVersion();
			lbEvents.DataSource = Logger.Log;
			Logger.Changed += new EventHandler(Logger_Changed);
			this.xHelper = xHelper;
		}

		#region Configuration Files Load/Save Methods
		private void ReadLocations()
		{
			PluginConfig cfg = (PluginConfig) Helper.DeserializeObject(WeatherPlugin.FORECASTS_XML, typeof(PluginConfig));
			if (cfg.Regions != null)
			{
				cfg.Upgrade();
				forecastRegions.Items.AddRange(cfg.Regions);
			}
		}

		private void WriteLocations()
		{
			PluginConfig cfg = new PluginConfig();
			cfg.Regions = new WeatherRegionForecast[forecastRegions.Items.Count];
			for (int i = 0; i < forecastRegions.Items.Count; i++)
				cfg.Regions[i] = (WeatherRegionForecast) forecastRegions.Items[i];
			Helper.SerializeObject(WeatherPlugin.FORECASTS_XML, cfg, typeof(PluginConfig));
		}

		private void ReadVariables()
		{
			variablesGrid.DataSource = Helper.DeserializeTable(WeatherPlugin.VARIABLES_XML);
		}

		/// <summary>
		/// Reads the region maps.
		/// </summary>
		private DataTable ReadRegionMaps()
		{
			return Helper.DeserializeTable(WeatherPlugin.REGIONMAPS_XML);
		}

		private void WriteVariables()
		{
			Helper.SerializeTable(WeatherPlugin.VARIABLES_XML, (DataTable) variablesGrid.DataSource);
		}

		private void Error(string message)
		{
			MessageBox.Show(message, "XoapWeather", MessageBoxButtons.OK, MessageBoxIcon.Error);
		}
		#endregion

		#region Form Event Handlers
		private void searchButton_Click(object sender, System.EventArgs e)
		{
			if (xHelper != null && xHelper.SendCommand("connectedToInternet") == "false") 
			{
				Error("Unable to search for location - You are not connected to the Internet");
				return;
			}
			if (searchText.Text == "")
			{
				MessageBox.Show("Enter a valid search string", "Weather Plugin", 
					MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
			else
			{
				string text = searchButton.Text;
				searchButton.Text = "Searching...";
				searchButton.Enabled = false;
				searchResults.DataSource = WeatherRegionForecast.SearchForLocations(searchText.Text);
				if (searchResults.Items.Count == 0)
					MessageBox.Show("No locations were found for '" + searchText.Text + "'", "Weather Plugin", 
						MessageBoxButtons.OK, MessageBoxIcon.Error);
				else
					searchText.Text = "";
				searchButton.Enabled = true;
				searchButton.Text = text;
			}
		}

		private void saveButton_Click(object sender, System.EventArgs e)
		{
			WriteLocations();
			WriteVariables();
		}

		private void btnAdd_Click(object sender, System.EventArgs e)
		{
			if (searchResults.SelectedIndex != -1)
			{
				WeatherLocation srcLoc = (WeatherLocation) searchResults.Items[searchResults.SelectedIndex];
				WeatherRegionForecast dstLoc = new WeatherRegionForecast();

				dstLoc.LocationId = srcLoc.LocationId;
				dstLoc.Description = srcLoc.Description;
				dstLoc.ForecastDays = Convert.ToInt16(forecastDays.Value);
				dstLoc.IsMetric = rbUnitMetric.Checked;
				dstLoc.RadarMap = XoapProvider.Instance.GetRadarUrl(srcLoc.LocationId);
				forecastRegions.Items.Add(dstLoc);
				forecastRegions.SelectedIndex = forecastRegions.FindStringExact(dstLoc.Description);
			}
		}

		private void btnRemove_Click(object sender, System.EventArgs e)
		{
			if (forecastRegions.SelectedIndex != -1)
			{
				forecastRegions.Items.RemoveAt(forecastRegions.SelectedIndex);
				forecastRegions.SelectedIndex = -1;
			}
		}

		private void searchResults_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			btnAdd.Enabled = (searchResults.SelectedIndex != -1);
		}

		private void weatherLocations_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			// Enable and disable sections based on index selection
			bool enabled = (forecastRegions.SelectedIndex != -1);
			btnRemove.Enabled = enabled;
			unitBox.Enabled = enabled;
			forecastGroup.Enabled = enabled;
			radarGroup.Enabled = enabled;

			if (forecastRegions.SelectedIndex != -1)
			{
				WeatherRegionForecast region = (WeatherRegionForecast) forecastRegions.Items[forecastRegions.SelectedIndex];
				forecastDays.Value = Convert.ToDecimal(region.ForecastDays);
				rbUnitMetric.Checked = region.IsMetric;
				rbUnitStandard.Checked = !region.IsMetric;

				// Find the url in the region map list
				DataTable dt = (DataTable) radarMap.DataSource;
				DataRow[] dr = dt.Select("url='"+region.RadarMap+"'");
				if (dr.Length == 1)
				{
					radarMap.Text = "";
					radarMap.SelectedIndex = radarMap.FindStringExact(dr[0]["name"].ToString());
				}
				else
				{
					radarMap.SelectedIndex = -1;
					radarMap.Text = region.RadarMap;
				}
			}
		}

		private void forecastDays_Leave(object sender, System.EventArgs e)
		{
			if (forecastRegions.SelectedIndex != -1)
			{
				WeatherRegionForecast region = (WeatherRegionForecast) forecastRegions.SelectedItem;
				region.ForecastDays = Convert.ToInt16(forecastDays.Value);
			}		
		}

		private void unitBox_Leave(object sender, System.EventArgs e)
		{
			if (forecastRegions.SelectedIndex != -1)
			{
				WeatherRegionForecast region = (WeatherRegionForecast) forecastRegions.SelectedItem;
				region.IsMetric = rbUnitMetric.Checked;
			}
		}

		private void radarMap_Leave(object sender, System.EventArgs e)
		{
			if (forecastRegions.SelectedIndex != -1)
			{
				WeatherRegionForecast region = (WeatherRegionForecast) forecastRegions.SelectedItem;
				if (radarMap.SelectedIndex != -1)
					region.RadarMap = (string) radarMap.SelectedValue;
				else
					region.RadarMap = radarMap.Text;					
			}
		}

		private void GetDefaultMap_Click(object sender, System.EventArgs e)
		{
			if (forecastRegions.SelectedIndex != -1)
			{
				WeatherRegionForecast region = (WeatherRegionForecast) forecastRegions.SelectedItem;
				radarMap.Text = XoapProvider.Instance.GetRadarUrl(region.LocationId);
			}
		}

		/// <summary>
		/// Shows the weather channel web site when clicked.
		/// </summary>
		/// <param name="sender">Sender.</param>
		/// <param name="e">E.</param>
		private void twcLogo_Click(object sender, System.EventArgs e)
		{
			System.Diagnostics.Process.Start("http://www.weather.com/");
		}

		/// <summary>
		/// Starts event logging.
		/// </summary>
		/// <param name="sender">Sender.</param>
		/// <param name="e">E.</param>
		private void btnStartLogging_Click(object sender, System.EventArgs e)
		{
			Logger.Start();
		}

		/// <summary>
		/// Stop event logging.
		/// </summary>
		/// <param name="sender">Sender.</param>
		/// <param name="e">E.</param>
		private void btnStopLogging_Click(object sender, System.EventArgs e)
		{
			Logger.Stop();
		}

		/// <summary>
		/// Event signals a new event and to update the event log display
		/// </summary>
		/// <param name="sender">Sender.</param>
		/// <param name="e">E.</param>
		private void Logger_Changed(object sender, EventArgs e)
		{
			lbEvents.BeginUpdate();
			CurrencyManager cManager = this.BindingContext[Logger.Log] as CurrencyManager;
			cManager.Refresh();
			lbEvents.SelectedIndex = lbEvents.Items.Count-1;
			lbEvents.EndUpdate();
		}

		/// <summary>
		/// Click event to dump log to file.
		/// </summary>
		/// <param name="sender">Sender.</param>
		/// <param name="e">E.</param>
		private void btnDumpLog_Click(object sender, System.EventArgs e)
		{
			Logger.DumpToFile(Helper.GetPath("logdump.txt"));
			MessageBox.Show("Log written to logdump.txt", "XoapWeather", MessageBoxButtons.OK, MessageBoxIcon.Information);
		}
		#endregion

		#region Drag and Drop Operation
		private void searchResults_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			ListBox lb = (ListBox) sender;
			int index = lb.IndexFromPoint(e.X,e.Y);
			if(index >= 0)
			{
				WeatherRegionForecast src = (WeatherRegionForecast) lb.Items[index];
				lb.DoDragDrop(src, DragDropEffects.Move);
			}
		}

		private void forecastRegions_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if (e.Data.GetDataPresent(typeof(WeatherRegionForecast)))
				e.Effect = DragDropEffects.Move;
			else
				e.Effect = DragDropEffects.None;
		}

		private void forecastRegions_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{
			WeatherRegionForecast loc = (WeatherRegionForecast) e.Data.GetData(typeof(WeatherLocation));
			loc.ForecastDays = Convert.ToInt16(forecastDays.Value);
			loc.IsMetric = rbUnitMetric.Checked;
			forecastRegions.Items.Add(loc);
		}
		#endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(ConfigurationForm));
			this.tabControl = new System.Windows.Forms.TabControl();
			this.tabLocations = new System.Windows.Forms.TabPage();
			this.label5 = new System.Windows.Forms.Label();
			this.radarGroup = new System.Windows.Forms.GroupBox();
			this.radarMap = new System.Windows.Forms.ComboBox();
			this.radarMenu = new System.Windows.Forms.ContextMenu();
			this.GetDefaultMap = new System.Windows.Forms.MenuItem();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.searchText = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.searchButton = new System.Windows.Forms.Button();
			this.btnRemove = new System.Windows.Forms.Button();
			this.btnAdd = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.forecastRegions = new System.Windows.Forms.ListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.searchResults = new System.Windows.Forms.ListBox();
			this.forecastGroup = new System.Windows.Forms.GroupBox();
			this.label3 = new System.Windows.Forms.Label();
			this.forecastDays = new System.Windows.Forms.NumericUpDown();
			this.unitBox = new System.Windows.Forms.GroupBox();
			this.rbUnitMetric = new System.Windows.Forms.RadioButton();
			this.rbUnitStandard = new System.Windows.Forms.RadioButton();
			this.twcLogo = new System.Windows.Forms.PictureBox();
			this.tabVariables = new System.Windows.Forms.TabPage();
			this.variablesGrid = new System.Windows.Forms.DataGrid();
			this.tabLog = new System.Windows.Forms.TabPage();
			this.btnDumpLog = new System.Windows.Forms.Button();
			this.btnStopLogging = new System.Windows.Forms.Button();
			this.btnStartLogging = new System.Windows.Forms.Button();
			this.lbEvents = new System.Windows.Forms.ListBox();
			this.saveButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.versionLabel = new System.Windows.Forms.Label();
			this.toolTip = new System.Windows.Forms.ToolTip(this.components);
			this.tabControl.SuspendLayout();
			this.tabLocations.SuspendLayout();
			this.radarGroup.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.forecastGroup.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.forecastDays)).BeginInit();
			this.unitBox.SuspendLayout();
			this.tabVariables.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.variablesGrid)).BeginInit();
			this.tabLog.SuspendLayout();
			this.SuspendLayout();
			// 
			// tabControl
			// 
			this.tabControl.Controls.Add(this.tabLocations);
			this.tabControl.Controls.Add(this.tabVariables);
			this.tabControl.Controls.Add(this.tabLog);
			this.tabControl.Location = new System.Drawing.Point(8, 8);
			this.tabControl.Name = "tabControl";
			this.tabControl.SelectedIndex = 0;
			this.tabControl.Size = new System.Drawing.Size(744, 320);
			this.tabControl.TabIndex = 15;
			// 
			// tabLocations
			// 
			this.tabLocations.Controls.Add(this.label5);
			this.tabLocations.Controls.Add(this.radarGroup);
			this.tabLocations.Controls.Add(this.groupBox1);
			this.tabLocations.Controls.Add(this.forecastGroup);
			this.tabLocations.Controls.Add(this.unitBox);
			this.tabLocations.Controls.Add(this.twcLogo);
			this.tabLocations.Location = new System.Drawing.Point(4, 22);
			this.tabLocations.Name = "tabLocations";
			this.tabLocations.Size = new System.Drawing.Size(736, 294);
			this.tabLocations.TabIndex = 0;
			this.tabLocations.Text = "Weather Locations";
			// 
			// label5
			// 
			this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label5.Location = new System.Drawing.Point(550, 248);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(80, 32);
			this.label5.TabIndex = 2;
			this.label5.Text = "Weather data provided by";
			// 
			// radarGroup
			// 
			this.radarGroup.Controls.Add(this.radarMap);
			this.radarGroup.Enabled = false;
			this.radarGroup.Location = new System.Drawing.Point(536, 176);
			this.radarGroup.Name = "radarGroup";
			this.radarGroup.Size = new System.Drawing.Size(176, 56);
			this.radarGroup.TabIndex = 0;
			this.radarGroup.TabStop = false;
			this.radarGroup.Text = "Radar Image Map";
			this.toolTip.SetToolTip(this.radarGroup, "Specify the map to use for the radar image");
			// 
			// radarMap
			// 
			this.radarMap.ContextMenu = this.radarMenu;
			this.radarMap.DisplayMember = "name";
			this.radarMap.DropDownWidth = 300;
			this.radarMap.Items.AddRange(new object[] {
														  "(automatic)"});
			this.radarMap.Location = new System.Drawing.Point(8, 24);
			this.radarMap.MaxDropDownItems = 20;
			this.radarMap.Name = "radarMap";
			this.radarMap.Size = new System.Drawing.Size(160, 21);
			this.radarMap.Sorted = true;
			this.radarMap.TabIndex = 0;
			this.toolTip.SetToolTip(this.radarMap, "Right click for options");
			this.radarMap.ValueMember = "url";
			this.radarMap.Leave += new System.EventHandler(this.radarMap_Leave);
			// 
			// radarMenu
			// 
			this.radarMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.GetDefaultMap});
			// 
			// GetDefaultMap
			// 
			this.GetDefaultMap.Index = 0;
			this.GetDefaultMap.Text = "Get default map from Weather.com";
			this.GetDefaultMap.Click += new System.EventHandler(this.GetDefaultMap_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.searchText);
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.searchButton);
			this.groupBox1.Controls.Add(this.btnRemove);
			this.groupBox1.Controls.Add(this.btnAdd);
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.forecastRegions);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.searchResults);
			this.groupBox1.Location = new System.Drawing.Point(24, 16);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(496, 264);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Weather Locations";
			// 
			// searchText
			// 
			this.searchText.Location = new System.Drawing.Point(16, 232);
			this.searchText.MaxLength = 50;
			this.searchText.Name = "searchText";
			this.searchText.Size = new System.Drawing.Size(200, 20);
			this.searchText.TabIndex = 36;
			this.searchText.Text = "";
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(16, 216);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(192, 17);
			this.label4.TabIndex = 35;
			this.label4.Text = "Type the name of the location to find:";
			// 
			// searchButton
			// 
			this.searchButton.Location = new System.Drawing.Point(224, 230);
			this.searchButton.Name = "searchButton";
			this.searchButton.Size = new System.Drawing.Size(88, 24);
			this.searchButton.TabIndex = 37;
			this.searchButton.Text = "Search";
			this.toolTip.SetToolTip(this.searchButton, "Click here to start the search and show the results in the window above");
			this.searchButton.Click += new System.EventHandler(this.searchButton_Click);
			// 
			// btnRemove
			// 
			this.btnRemove.Enabled = false;
			this.btnRemove.Location = new System.Drawing.Point(224, 140);
			this.btnRemove.Name = "btnRemove";
			this.btnRemove.Size = new System.Drawing.Size(56, 40);
			this.btnRemove.TabIndex = 4;
			this.btnRemove.Text = "Remove Location";
			this.toolTip.SetToolTip(this.btnRemove, "Click here to remove the selected forecast from the active list");
			this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
			// 
			// btnAdd
			// 
			this.btnAdd.Enabled = false;
			this.btnAdd.Location = new System.Drawing.Point(224, 68);
			this.btnAdd.Name = "btnAdd";
			this.btnAdd.Size = new System.Drawing.Size(56, 40);
			this.btnAdd.TabIndex = 3;
			this.btnAdd.Text = "Add Location";
			this.toolTip.SetToolTip(this.btnAdd, "Click here to add the selected search location to your active forecasts");
			this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(288, 28);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(192, 16);
			this.label2.TabIndex = 34;
			this.label2.Text = "Active Forecast Regions:";
			// 
			// forecastRegions
			// 
			this.forecastRegions.AllowDrop = true;
			this.forecastRegions.DisplayMember = "Description";
			this.forecastRegions.Location = new System.Drawing.Point(288, 44);
			this.forecastRegions.Name = "forecastRegions";
			this.forecastRegions.ScrollAlwaysVisible = true;
			this.forecastRegions.Size = new System.Drawing.Size(200, 160);
			this.forecastRegions.TabIndex = 5;
			this.toolTip.SetToolTip(this.forecastRegions, "This window shows you the list of active forecasts you can see");
			this.forecastRegions.DragDrop += new System.Windows.Forms.DragEventHandler(this.forecastRegions_DragDrop);
			this.forecastRegions.DragEnter += new System.Windows.Forms.DragEventHandler(this.forecastRegions_DragEnter);
			this.forecastRegions.SelectedIndexChanged += new System.EventHandler(this.weatherLocations_SelectedIndexChanged);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 28);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(184, 16);
			this.label1.TabIndex = 32;
			this.label1.Text = "Current Search Results:";
			// 
			// searchResults
			// 
			this.searchResults.DisplayMember = "Description";
			this.searchResults.Location = new System.Drawing.Point(16, 44);
			this.searchResults.Name = "searchResults";
			this.searchResults.ScrollAlwaysVisible = true;
			this.searchResults.Size = new System.Drawing.Size(200, 160);
			this.searchResults.TabIndex = 2;
			this.toolTip.SetToolTip(this.searchResults, "This window shows you the results of your location search");
			this.searchResults.ValueMember = "Id";
			this.searchResults.MouseDown += new System.Windows.Forms.MouseEventHandler(this.searchResults_MouseDown);
			this.searchResults.SelectedIndexChanged += new System.EventHandler(this.searchResults_SelectedIndexChanged);
			// 
			// forecastGroup
			// 
			this.forecastGroup.Controls.Add(this.label3);
			this.forecastGroup.Controls.Add(this.forecastDays);
			this.forecastGroup.Enabled = false;
			this.forecastGroup.Location = new System.Drawing.Point(536, 104);
			this.forecastGroup.Name = "forecastGroup";
			this.forecastGroup.Size = new System.Drawing.Size(176, 56);
			this.forecastGroup.TabIndex = 0;
			this.forecastGroup.TabStop = false;
			this.forecastGroup.Text = "Forecast Time Frame";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(59, 27);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(96, 16);
			this.label3.TabIndex = 0;
			this.label3.Text = "Day(s) Forecast";
			this.toolTip.SetToolTip(this.label3, "Numbers of days of data to request from the weather server");
			// 
			// forecastDays
			// 
			this.forecastDays.Location = new System.Drawing.Point(16, 24);
			this.forecastDays.Maximum = new System.Decimal(new int[] {
																		 12,
																		 0,
																		 0,
																		 0});
			this.forecastDays.Minimum = new System.Decimal(new int[] {
																		 1,
																		 0,
																		 0,
																		 0});
			this.forecastDays.Name = "forecastDays";
			this.forecastDays.Size = new System.Drawing.Size(40, 20);
			this.forecastDays.TabIndex = 10;
			this.toolTip.SetToolTip(this.forecastDays, "Numbers of days of data to request from the weather server");
			this.forecastDays.Value = new System.Decimal(new int[] {
																	   6,
																	   0,
																	   0,
																	   0});
			this.forecastDays.Leave += new System.EventHandler(this.forecastDays_Leave);
			// 
			// unitBox
			// 
			this.unitBox.Controls.Add(this.rbUnitMetric);
			this.unitBox.Controls.Add(this.rbUnitStandard);
			this.unitBox.Enabled = false;
			this.unitBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.unitBox.Location = new System.Drawing.Point(536, 16);
			this.unitBox.Name = "unitBox";
			this.unitBox.Size = new System.Drawing.Size(176, 72);
			this.unitBox.TabIndex = 0;
			this.unitBox.TabStop = false;
			this.unitBox.Text = "Weather Units";
			this.unitBox.Leave += new System.EventHandler(this.unitBox_Leave);
			// 
			// rbUnitMetric
			// 
			this.rbUnitMetric.Location = new System.Drawing.Point(16, 40);
			this.rbUnitMetric.Name = "rbUnitMetric";
			this.rbUnitMetric.Size = new System.Drawing.Size(144, 24);
			this.rbUnitMetric.TabIndex = 9;
			this.rbUnitMetric.Text = "Metric Units";
			this.toolTip.SetToolTip(this.rbUnitMetric, "Metric units are celcius, kmh and millibars etc.");
			// 
			// rbUnitStandard
			// 
			this.rbUnitStandard.Location = new System.Drawing.Point(16, 16);
			this.rbUnitStandard.Name = "rbUnitStandard";
			this.rbUnitStandard.Size = new System.Drawing.Size(144, 24);
			this.rbUnitStandard.TabIndex = 8;
			this.rbUnitStandard.Text = "US Standard Units";
			this.toolTip.SetToolTip(this.rbUnitStandard, "Standard units are degrees F, mph, inches etc.");
			// 
			// twcLogo
			// 
			this.twcLogo.Image = ((System.Drawing.Image)(resources.GetObject("twcLogo.Image")));
			this.twcLogo.Location = new System.Drawing.Point(627, 230);
			this.twcLogo.Name = "twcLogo";
			this.twcLogo.Size = new System.Drawing.Size(65, 65);
			this.twcLogo.TabIndex = 1;
			this.twcLogo.TabStop = false;
			this.twcLogo.Click += new System.EventHandler(this.twcLogo_Click);
           		this.toolTip.SetToolTip(this.twcLogo, "Click on icon to go to weather.com");
			// 
			// tabVariables
			// 
			this.tabVariables.Controls.Add(this.variablesGrid);
			this.tabVariables.Location = new System.Drawing.Point(4, 22);
			this.tabVariables.Name = "tabVariables";
			this.tabVariables.Size = new System.Drawing.Size(736, 294);
			this.tabVariables.TabIndex = 1;
			this.tabVariables.Text = "Display Variables";
			// 
			// variablesGrid
			// 
			this.variablesGrid.AllowNavigation = false;
			this.variablesGrid.AlternatingBackColor = System.Drawing.Color.Lavender;
			this.variablesGrid.BackColor = System.Drawing.Color.WhiteSmoke;
			this.variablesGrid.BackgroundColor = System.Drawing.Color.LightGray;
			this.variablesGrid.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.variablesGrid.CaptionBackColor = System.Drawing.Color.LightSteelBlue;
			this.variablesGrid.CaptionForeColor = System.Drawing.Color.MidnightBlue;
			this.variablesGrid.CaptionVisible = false;
			this.variablesGrid.DataMember = "";
			this.variablesGrid.FlatMode = true;
			this.variablesGrid.Font = new System.Drawing.Font("Tahoma", 8F);
			this.variablesGrid.ForeColor = System.Drawing.Color.MidnightBlue;
			this.variablesGrid.GridLineColor = System.Drawing.Color.Gainsboro;
			this.variablesGrid.GridLineStyle = System.Windows.Forms.DataGridLineStyle.None;
			this.variablesGrid.HeaderBackColor = System.Drawing.Color.MidnightBlue;
			this.variablesGrid.HeaderFont = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold);
			this.variablesGrid.HeaderForeColor = System.Drawing.Color.WhiteSmoke;
			this.variablesGrid.LinkColor = System.Drawing.Color.Teal;
			this.variablesGrid.Location = new System.Drawing.Point(8, 7);
			this.variablesGrid.Name = "variablesGrid";
			this.variablesGrid.ParentRowsBackColor = System.Drawing.Color.Gainsboro;
			this.variablesGrid.ParentRowsForeColor = System.Drawing.Color.MidnightBlue;
			this.variablesGrid.PreferredColumnWidth = 330;
			this.variablesGrid.SelectionBackColor = System.Drawing.Color.CadetBlue;
			this.variablesGrid.SelectionForeColor = System.Drawing.Color.WhiteSmoke;
			this.variablesGrid.Size = new System.Drawing.Size(720, 281);
			this.variablesGrid.TabIndex = 0;
			// 
			// tabLog
			// 
			this.tabLog.Controls.Add(this.btnDumpLog);
			this.tabLog.Controls.Add(this.btnStopLogging);
			this.tabLog.Controls.Add(this.btnStartLogging);
			this.tabLog.Controls.Add(this.lbEvents);
			this.tabLog.Location = new System.Drawing.Point(4, 22);
			this.tabLog.Name = "tabLog";
			this.tabLog.Size = new System.Drawing.Size(736, 294);
			this.tabLog.TabIndex = 4;
			this.tabLog.Text = "Event Log";
			// 
			// btnDumpLog
			// 
			this.btnDumpLog.Location = new System.Drawing.Point(632, 248);
			this.btnDumpLog.Name = "btnDumpLog";
			this.btnDumpLog.TabIndex = 103;
			this.btnDumpLog.Text = "Dump Log";
			this.toolTip.SetToolTip(this.btnDumpLog, "Dump log to file");
			this.btnDumpLog.Click += new System.EventHandler(this.btnDumpLog_Click);
			// 
			// btnStopLogging
			// 
			this.btnStopLogging.Location = new System.Drawing.Point(632, 208);
			this.btnStopLogging.Name = "btnStopLogging";
			this.btnStopLogging.TabIndex = 102;
			this.btnStopLogging.Text = "Stop Log";
			this.toolTip.SetToolTip(this.btnStopLogging, "Stop logging events");
			this.btnStopLogging.Click += new System.EventHandler(this.btnStopLogging_Click);
			// 
			// btnStartLogging
			// 
			this.btnStartLogging.Location = new System.Drawing.Point(632, 168);
			this.btnStartLogging.Name = "btnStartLogging";
			this.btnStartLogging.TabIndex = 101;
			this.btnStartLogging.Text = "Start Log";
			this.toolTip.SetToolTip(this.btnStartLogging, "Starts logging events");
			this.btnStartLogging.Click += new System.EventHandler(this.btnStartLogging_Click);
			// 
			// lbEvents
			// 
			this.lbEvents.HorizontalScrollbar = true;
			this.lbEvents.Location = new System.Drawing.Point(8, 8);
			this.lbEvents.Name = "lbEvents";
			this.lbEvents.ScrollAlwaysVisible = true;
			this.lbEvents.Size = new System.Drawing.Size(600, 277);
			this.lbEvents.TabIndex = 0;
			// 
			// saveButton
			// 
			this.saveButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.saveButton.Location = new System.Drawing.Point(576, 344);
			this.saveButton.Name = "saveButton";
			this.saveButton.TabIndex = 100;
			this.saveButton.Text = "Save";
			this.toolTip.SetToolTip(this.saveButton, "Click here to save your configuration and close this screen");
			this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
			// 
			// cancelButton
			// 
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(675, 344);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.TabIndex = 101;
			this.cancelButton.Text = "Cancel";
			this.toolTip.SetToolTip(this.cancelButton, "Click here to cancel and lose any changes made");
			// 
			// versionLabel
			// 
			this.versionLabel.Location = new System.Drawing.Point(4, 358);
			this.versionLabel.Name = "versionLabel";
			this.versionLabel.Size = new System.Drawing.Size(320, 16);
			this.versionLabel.TabIndex = 0;
			this.versionLabel.Text = "Version:";
			// 
			// ConfigurationForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(762, 376);
			this.Controls.Add(this.versionLabel);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.saveButton);
			this.Controls.Add(this.tabControl);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "ConfigurationForm";
			this.Text = "Xoap Weather Plugin Configuration";
			this.tabControl.ResumeLayout(false);
			this.tabLocations.ResumeLayout(false);
			this.radarGroup.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.forecastGroup.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.forecastDays)).EndInit();
			this.unitBox.ResumeLayout(false);
			this.tabVariables.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.variablesGrid)).EndInit();
			this.tabLog.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}
		#endregion	
	}
}