/*
 * XoapWeather Client Plugin for XLobby2
 * Copyright (c) 2004 Jonathan Bradshaw
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE. 
 */
using System;
using System.IO;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using System.Collections;
using System.Collections.Specialized;
using System.Globalization;
using System.Diagnostics;

namespace XoapWeather.Provider
{
	/// <summary>
	/// XoapClient is a singleton provider capable of requesting and 
	/// retrieving data from the Weather.com Xoap server.
	/// </summary>
	public sealed class XoapProvider : IWeatherProvider
	{
		#region Member Constants
		const string   XOAP_SEARCH_URL = "http://xoap.weather.com/search/search?where={0}";
		const string   XOAP_WX_URL	   = "http://xoap.weather.com/weather/local/{0}?{1}";
		#endregion

		#region Singleton Pattern Implementation
		// Property to store singleton instance of this class
		static readonly XoapProvider _instance = new XoapProvider();

		// Explicit static constructor to tell C# compiler not to mark type as beforefieldinit
		static XoapProvider() { }

		// Explicit constructor
		XoapProvider() { }

		/// <summary>
		/// Factory method returns singleton instance of XoapProvider
		/// </summary>
		/// <returns>XoapProvider singleton</returns>
		public static XoapProvider Instance
		{
			get { return _instance; }
		}
		#endregion

		#region IWeatherProvider Methods
		/// <summary>
		/// Calls the Weather.com location search service with the given location parameter
		/// and returns a StringDictionary with the following useful fields:
		///		key	  : Location ID to be used for forecast
		///		value : Display description of location
		/// </summary>
		/// <param name="locationSearchText">Name of location to search for (e.g. London)</param>
		/// <returns>StringDictionary with results or null on error</returns>
		public Hashtable GetLocationID(string locationSearchText)
		{
			string  url = String.Format(CultureInfo.InvariantCulture, XOAP_SEARCH_URL, locationSearchText);
			XmlNodeList locNodes = GetDataFromServer(locationSearchText, url).SelectNodes("/search/loc");
			Hashtable results = new Hashtable();
			foreach (XmlNode location in locNodes)
				results.Add(location.Attributes["id"].Value, location.InnerText);
			return results;
		}

		/// <summary>
		/// Returns an XmlDocument with current condition weather data for the
		/// specified location using the specified unit type. If the provider
		/// cannot provide current conditions this method will return null.
		/// </summary>
		/// <param name="locationID">A valid location ID string</param>
		/// <param name="units">Imperial or Metric units</param>
		/// <returns>XmlDocument or null</returns>
		public XmlDocument GetCurrentConditions(string locationID, XoapWeather.Provider.WeatherUnits units)
		{
			XoapRequest request = new XoapRequest();

			request.CurrentConditionReport = true;
			request.Units = units;
			string url = String.Format(CultureInfo.InvariantCulture, XOAP_WX_URL, locationID, request.QueryString);
			return GetDataFromServer(locationID, url);
		}

		/// <summary>
		/// Returns an XmlDocument with forecast weather data for the
		/// specified location using the specified unit type for the specified
		/// number of days. If the provider cannot provide a forecast this method
		/// will return null.
		/// </summary>
		/// <param name="locationID">A valid location ID string</param>
		/// <param name="units">Imperial or Metric units</param>
		/// <param name="days">Number of days (0 means today)</param>
		/// <returns>XmlDocument or null</returns>
		public XmlDocument GetForecast(string locationID, XoapWeather.Provider.WeatherUnits units, int days)
		{
			XoapRequest request = new XoapRequest();

			request.DailyForecast = days;
			request.DetailDailyForecast = days;
			request.RegionalConditionReport = true;
			request.Units = units;
			request.HourByHourForecast = 12;
			string url = String.Format(CultureInfo.InvariantCulture, XOAP_WX_URL, locationID, request.QueryString);
			return GetDataFromServer(locationID, url);
		}

		/// <summary>
		/// Gets the radar URL for the specified location
		/// </summary>
		/// <param name="locationId">Location id.</param>
		/// <returns></returns>
		public string GetRadarUrl(string locationId)
		{
			const string RADAR_PAGE = "http://www.w3.weather.com/outlook/travel/local/{0}";

//			Regex  radarRegex = new Regex("SRC=\"(?<image>.*?/web/radar/.*?\\.jpg)\"");
//			Regex  satRegex   = new Regex("SRC=\"(?<image>.*?/images/sat/.*?\\.jpg)\"");
			Regex  radarRegex = new Regex(",'(?<image>.*?/web/radar/.*?\\.jpg)");
			Regex  satRegex   = new Regex(",'(?<image>.*?/images/sat/.*?\\.jpg)");
			string imageUrl   = String.Empty;

			try 
			{
				string url = String.Format(CultureInfo.InvariantCulture, RADAR_PAGE, locationId);
				WebRequest webRequest = WebRequest.Create(url);

				// Grab the HTML web page
				StreamReader sr = new StreamReader(webRequest.GetResponse().GetResponseStream());
				string pageContent = sr.ReadToEnd();
				sr.Close();

				Match radarMatch = radarRegex.Match(pageContent);
				if (radarMatch.Success)
				{
					imageUrl = radarMatch.Groups["image"].Value.Trim();
				}
				else
				{
					Match satMatch = satRegex.Match(pageContent);
					if (satMatch.Success)
						imageUrl = satMatch.Groups["image"].Value.Trim();
				}
			} 
			catch 
				(SystemException) { }
			return imageUrl;
		}

		#endregion

		#region Private Methods
		private XmlDocument GetDataFromServer(string locationID, string url)
		{
			XmlDocument doc = new XmlDocument();
			XmlTextReader reader = new XmlTextReader(url);
			doc.Load(reader);
			reader.Close();
			if (doc.SelectNodes("/error").Count > 0) 
			{
				string errmsg = doc.SelectSingleNode("/error").InnerText;
				throw new WeatherException(locationID + ": " + errmsg);
			}
			return doc;
		}		
		#endregion
	}

	/// <summary>
	/// Used to build the query string for a forecast request with the desired parameters
	/// for the type of forecast information to retrive. Also computes how long based on
	/// Weather.com rules the information is valid (and should be cached) for.
	/// </summary>
	class XoapRequest
	{
		#region Member Constants
		public static string XOAP_PARTNER_ID  = "1004126345";
		public static string XOAP_LICENSE_KEY = "0c9166d0c24b7ca0";
		#endregion

		#region Member Variables
		private NameValueCollection query = new NameValueCollection();
		private int expires = 0;
		#endregion

		#region Constructors
		/// <summary>
		/// Returns a new instance of ForeCast request for building the parameters to be given
		/// to the XoapClient when requesting weather data.
		/// </summary>
		public XoapRequest()
		{
			query.Set("par", XOAP_PARTNER_ID);
			query.Set("key", XOAP_LICENSE_KEY);
		}
		#endregion

		#region Properties
		/// <summary>
		/// Gets the query string.
		/// </summary>
		/// <returns>String</returns>
		internal string QueryString
		{
			get 
			{
				StringBuilder sb = new StringBuilder();
				foreach (string key in query)
				{
					sb.AppendFormat("{0}={1}&", key, query[key]);
				}
				sb.Length -= 1;
				return sb.ToString();
			}
		}

		/// <summary>
		/// Gets the validity period.
		/// </summary>
		/// <returns>Timespan with validity</returns>
		public TimeSpan ValidityPeriod
		{
			get { return new TimeSpan(0, expires, 0); }
		}

		/// <summary>
		/// Gets or sets the units.
		/// </summary>
		/// <value>WeatherUnits value</value>
		public WeatherUnits Units
		{
			get 
			{
				return (query["unit"] != null && query["unit"].Equals("m") ? WeatherUnits.Metric : WeatherUnits.Imperial); 
			}
			set 
			{ 
				query.Set("unit", value == WeatherUnits.Metric ? "m" : "s"); 
			}
		}

		/// <summary>
		/// Gets or sets a value indicating whether current condition report is retrieved.
		/// </summary>
		/// <value>
		/// 	<c>true</c> if current condition report requested; otherwise, <c>false</c>.
		/// </value>
		public bool CurrentConditionReport
		{
			get 
			{
				return (query["cc"] != null ? true : false);
			}
			set
			{
				if (value == true) 
				{
					query.Set("cc", "*");
				}
				else 
				{
					query.Remove("cc");
				}
				expires = Math.Max(expires, 30);
			}
		}

		/// <summary>
		/// Gets or sets a value indicating whether regional condition report is retrieved.
		/// </summary>
		/// <value>
		/// 	<c>true</c> if regional condition report requested; otherwise, <c>false</c>.
		/// </value>
		public bool RegionalConditionReport
		{
			get 
			{
				return (query["rgnf"] != null ? true : false);
			}
			set
			{
				if (value == true) 
				{
					query.Set("rgnf", "*");
				}
				else 
				{
					query.Remove("rgnf");
				}
				expires = Math.Max(expires, 120);
			}
		}

		/// <summary>
		/// Gets or sets the daily forecast.
		/// </summary>
		/// <value></value>
		public int DailyForecast
		{
			get
			{
				return (query["dayf"] == null ? 0 : (int) Convert.ToInt16(query["dayf"], CultureInfo.InvariantCulture));
			}
			set 
			{
				int days = Math.Min(value, 10);
				query.Set("dayf", Convert.ToString(days, CultureInfo.InvariantCulture));
				expires = Math.Max(expires, 120);
			}
		}

		/// <summary>
		/// Gets or sets the detail daily forecast.
		/// </summary>
		/// <value></value>
		public int DetailDailyForecast
		{
			get
			{
				return (query["dayd"] == null ? 0 : (int) Convert.ToInt16(query["dayd"], CultureInfo.InvariantCulture));
			}
			set
			{
				int days = Math.Min(value, 5);
				query.Set("dayd", Convert.ToString(days, CultureInfo.InvariantCulture));
				expires = Math.Max(expires, 120);
			}
		}

		/// <summary>
		/// Gets or sets the hour by hour forecast.
		/// </summary>
		/// <value></value>
		public int HourByHourForecast
		{
			get
			{
				return (query["hbhf"] == null ? 0 : (int) Convert.ToInt16(query["hbhf"], CultureInfo.InvariantCulture));
			}
			set
			{
				int hours = Math.Min(value, 24);
				query.Set("hbhf", Convert.ToString(hours, CultureInfo.InvariantCulture));
				expires = Math.Max(expires, 120);
			}
		}
		#endregion
	}
}