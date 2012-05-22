/*
 * XoapWeather Client Plugin for XLobby2
 * Copyright (c) 2004 Jonathan Bradshaw
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. 
 */
using System;
using System.Xml;
using System.Xml.Serialization;
using System.Collections;

using XoapWeather.Entity;
using XoapWeather.Provider;

namespace XoapWeather.Plugin
{
	/// <summary>
	/// PluginConfig provides a container for holding one or more weather regions. 
	/// It is serialiazable and persisted as the "weather.xml" configuration file.
	/// </summary>
	[XmlRootAttribute("forecasts", Namespace="", IsNullable=false)]
	public class PluginConfig
	{
		private WeatherRegionForecast[] _regions;

		/// <summary>
		/// Contains an array of one or more weather regions.
		/// </summary>
		[XmlElementAttribute("forecast", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public WeatherRegionForecast[] Regions
		{
			get { return _regions; }
			set { _regions = value; }
		}

		/// <summary>
		/// Upgrades configuration data from previous versions
		/// </summary>
		public bool Upgrade()
		{
			bool isUpgraded = false;
			if (this.Regions != null) 
			{
				foreach (WeatherRegionForecast region in this.Regions)
				{
					// XWeather configuration had no description or days properties
					if (region.ForecastDays == 0) 
					{
						region.ForecastDays = 6;
						isUpgraded = true;
					}
					if (region.Description == null || region.Description.Length == 0)
					{
						region.Refresh();
						region.Description = region.Evaluate("string(/weather/loc/dnam)");
						isUpgraded = true;
					}
					// XWeather and XoapWeather v1.0 configuration had no radar image property
					if (region.RadarMap == null || region.RadarMap.Length == 0) 
					{
						region.RadarMap = XoapProvider.Instance.GetRadarUrl(region.LocationId);
						isUpgraded = true;
					}
				}
			}
			return isUpgraded;
		}
	}
}