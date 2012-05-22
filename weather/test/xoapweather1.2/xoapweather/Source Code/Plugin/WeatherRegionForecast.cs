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
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Threading;
using System.Xml;
using System.Xml.XPath;

using XoapWeather.Entity;
using XoapWeather.Provider;

namespace XoapWeather.Plugin
{
	/// <summary>
	/// The WeatherRegionForecast object tracks weather for a partiular WeatherRegion 
	/// through the use of a WeatherProvider. A default weather provider can be specified
	/// or it can be overridden during object construction.
	/// This class is thread safe.
	/// </summary>
	public class WeatherRegionForecast : WeatherRegionBase
	{
		#region Member Constants
		/// <summary>
		/// These constants define the Time To Live in minutes.
		/// </summary>
		private const int CONDITIONS_TTL = 20;
		private const int FORECAST_TTL   = 120;
		private const int LOCK_TIMEOUT   = 60000;
		#endregion

		#region Member Variables
		/// <summary>
		/// The default weather provider is set on the class. This provider will be used
		/// for any objects instantiated without a weather provider being specified.
		/// </summary>
		private static IWeatherProvider	_defaultWeatherProvider;
		
		/// <summary>
		/// Instance variables provide stateful weather data.
		/// </summary>
		private volatile IWeatherProvider	_weatherProvider;
		private volatile XmlDocument		_forecastXmlData;
		private volatile XPathNavigator		_xmlNavigator;
		private DateTime					_forecastExpiration = DateTime.MinValue;
		private DateTime					_conditionsExpiration = DateTime.MinValue;
		private volatile ReaderWriterLock	_rwLock = new ReaderWriterLock();
		#endregion

		#region Constructors
		/// <summary>
		/// Creates a new <see cref="WeatherRegionForecast"/> instance using the
		/// default weather provider. If the default weather provider has not
		/// been set, this constructor will throw an InvalidOperationException.
		/// </summary>
		public WeatherRegionForecast() : this(_defaultWeatherProvider)
		{ 
			if (this._weatherProvider == null)
				throw new InvalidOperationException("You must set a default or explicit weather provider");
		}

		/// <summary>
		/// Creates a new <see cref="WeatherRegionForecast"/> instance using an
		/// explicit weather provider.
		/// </summary>
		/// <param name="weatherProvider">Override the default Weather provider.</param>
		public WeatherRegionForecast(IWeatherProvider weatherProvider)
		{
			this._weatherProvider = weatherProvider;
		}

		/// <summary>
		/// Creates a new <see cref="WeatherRegionForecast"/> instance with the
		/// default weather provider.
		/// </summary>
		/// <param name="locationID">Location ID.</param>
		/// <param name="Description">Description.</param>
		public WeatherRegionForecast(string locationID, string Description): this(_defaultWeatherProvider)
		{
			this.LocationId = locationID;
			this.Description = Description;
		}

		/// <summary>
		/// Creates a new <see cref="WeatherRegionForecast"/> instance using an
		/// explicit weather provider.
		/// </summary>
		/// <param name="weatherProvider">Weather provider.</param>
		/// <param name="locationID">Location ID.</param>
		/// <param name="description">Description.</param>
		public WeatherRegionForecast(IWeatherProvider weatherProvider, string locationID, string description): this(_defaultWeatherProvider)
		{
			this.LocationId = locationID;
			this.Description = description;
		}
		#endregion

		#region Properties
		/// <summary>
		/// Gets or sets the default weather provider. Any newly constructed instances 
		/// of this class will use this weather provider if not overridden in the constructor.
		/// </summary>
		/// <value>IWeatherProvider object</value>
		[XmlIgnore]
		public static IWeatherProvider DefaultWeatherProvider
		{
			get { return _defaultWeatherProvider; }
			set { _defaultWeatherProvider = value; }
		}

		/// <summary>
		/// Returns true if the weather forecast data is loaded.
		/// </summary>
		/// <returns>Boolean value.</returns>
		[XmlIgnore]
		public bool IsLoaded
		{
			get	{ return (this._forecastXmlData != null); }
		}
		#endregion

		#region Public Methods
		/// <summary>
		/// Searches the for location string specified and returns an array of
		/// instances of WeatherRegionForecast.
		/// </summary>
		/// <param name="searchString">Search string.</param>
		/// <returns>WeatherRegionForecast[] array.</returns>
		public static WeatherRegionForecast[] SearchForLocations(string searchString)
		{
			WeatherRegionForecast[] locationList;

			Hashtable results = DefaultWeatherProvider.GetLocationID(searchString);
			locationList = new WeatherRegionForecast[results.Count];
			int i = 0;
			foreach (DictionaryEntry entry in results)
				locationList[i++] = new WeatherRegionForecast(entry.Key as string, entry.Value as string);
			return locationList;
		}

		/// <summary>
		/// Evaluates the XPathExpression against the weather data and returns the result.
		/// This method is thread safe.
		/// </summary>
		/// <param name="xpath">An xpath string that can be evaluated.</param>
		/// <returns>A string with the result or the empty string</returns>
		public string Evaluate(string xpath)
		{
			return Evaluate(xpath, String.Empty);
		}

		/// <summary>
		/// Evaluates the XPathExpression against the weather data and returns the result.
		/// This method is thread safe.
		/// </summary>
		/// <param name="xpath">An xpath string that can be evaluated.</param>
		/// <param name="defaultValue">Default value.</param>
		/// <returns>A string with the result or the default value</returns>
		public string Evaluate(string xpath, string defaultValue)
		{
			string result = null;
			if (this._forecastXmlData == null)
				return defaultValue;
			try 
			{
				AcquireReadLock();
				result = GetNavigator().Evaluate(xpath) as String;
			} 
			finally 
			{
				ReleaseReadLock();
			}
			return (result == null ? defaultValue : result);
		}

		/// <summary>
		/// Refreshes the weather data as required.
		/// This method is thread safe.
		/// </summary>
		/// <returns>DateTime for next update</returns>
		public DateTime Refresh()
		{
			try 
			{
				AcquireWriteLock();
				if (this._forecastXmlData == null || DateTime.Now >= this._forecastExpiration)
					UpdateFullForecast();
				else
					UpdateCurrentConditions();
			} 
			finally 
			{
				ReleaseWriteLock();
			}
			return this._conditionsExpiration;
		}

		/// <summary>
		/// Writes weather xml data to the xml writer.
		/// </summary>
		/// <param name="w">An XmlWriter.</param>
		public void WriteTo(XmlWriter w)
		{
			if (this._forecastXmlData != null)
			{
				AcquireReadLock();
				this._forecastXmlData.WriteTo(w);
				ReleaseReadLock();
			}
		}

		#endregion

		#region Private Methods
		/// <summary>
		/// Returns an existing XPath navigator or instantiates one if required.
		/// </summary>
		/// <returns></returns>
		private XPathNavigator GetNavigator()
		{
			if (this._xmlNavigator == null)
				this._xmlNavigator = _forecastXmlData.CreateNavigator();
			return this._xmlNavigator;
		}

		/// <summary>
		/// Gets the weather provider.
		/// </summary>
		/// <value></value>
		private IWeatherProvider WeatherProvider
		{
			get { return this._weatherProvider; }
		}

		/// <summary>
		/// Acquires the read lock.
		/// </summary>
		private void AcquireReadLock()
		{
			this._rwLock.AcquireReaderLock(LOCK_TIMEOUT);
		}

		/// <summary>
		/// Releases the read lock.
		/// </summary>
		private void ReleaseReadLock()
		{
			this._rwLock.ReleaseReaderLock();
		}

		/// <summary>
		/// Acquires the write lock.
		/// </summary>
		private void AcquireWriteLock()
		{
			this._rwLock.AcquireWriterLock(LOCK_TIMEOUT);
		}

		/// <summary>
		/// Releases the write lock.
		/// </summary>
		private void ReleaseWriteLock()
		{
			this._rwLock.ReleaseWriterLock();
		}

		/// <summary>
		/// Retrieves the current conditions.
		/// </summary>
		private void UpdateCurrentConditions()
		{
			XmlDocument currentConditionsXml = WeatherProvider.GetCurrentConditions(this.LocationId, this.IsMetric ? WeatherUnits.Metric : WeatherUnits.Imperial);
			if (currentConditionsXml != null)
			{
				this._conditionsExpiration = DateTime.Now.AddMinutes(CONDITIONS_TTL);
				// Merge the new child nodes into the existing forecast data
				XmlDocument destDoc = this._forecastXmlData;
				foreach (XmlNode srcNode in currentConditionsXml.DocumentElement.ChildNodes)
				{
					XmlNode newNode = destDoc.ImportNode(srcNode, true);
					XmlNode oldNode = destDoc.DocumentElement[srcNode.LocalName];
					if (oldNode == null)
						destDoc.DocumentElement.AppendChild(newNode);
					else
						destDoc.DocumentElement.ReplaceChild(newNode, oldNode);
				}
			}
		}

		/// <summary>
		/// Retrieves the full 'n' day forecast.
		/// </summary>
		private void UpdateFullForecast()
		{
			// Retrieve the data
			this._forecastXmlData = WeatherProvider.GetForecast(this.LocationId, this.IsMetric ? WeatherUnits.Metric : WeatherUnits.Imperial, this.ForecastDays);
			this._forecastExpiration = DateTime.Now.AddMinutes(FORECAST_TTL);
			this._xmlNavigator = null;
			UpdateCurrentConditions();
		}
		#endregion
	}
}