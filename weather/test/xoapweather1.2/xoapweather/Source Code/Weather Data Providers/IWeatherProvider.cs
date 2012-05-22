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
using System.Collections;
using System.Xml;

namespace XoapWeather.Provider
{
	/// <summary>
	/// WeatherUnits defines the measurement units used in a forecast.
	/// </summary>
	public enum WeatherUnits 
	{ 
		/// <summary>
		/// Imperial (also known as Standard or US) measurements
		/// </summary>
		Imperial, 
		/// <summary>
		/// Metric measurements
		/// </summary>
		Metric 
	};

	/// <summary>
	/// Generic weather exception to be thrown by the weather provider
	/// </summary>
	public class WeatherException : System.ApplicationException 
	{
		/// <summary>
		/// Creates a new <see cref="WeatherException"/> instance.
		/// </summary>
		/// <param name="message">Message.</param>
		public WeatherException ( string message ) : base ( message )
		{
		}
	}	

	/// <summary>
	/// Interface definition for a weather service provider (e.g. Weather.com)
	/// </summary>
	public interface IWeatherProvider
	{
		/// <summary>
		/// Returns results for a location search request as a StringDictionary. If
		/// the provider cannot process a search request this method will return null.
		/// </summary>
		/// <param name="locationSearchText">Search string</param>
		/// <returns>StringDictionary with results or null</returns>
		Hashtable GetLocationID(string locationSearchText);

		/// <summary>
		/// Returns an XmlDocument with current condition weather data for the
		/// specified location using the specified unit type. If the provider
		/// cannot provide current conditions this method will return null.
		/// </summary>
		/// <param name="locationID">A valid location ID string</param>
		/// <param name="units">Imperial or Metric units</param>
		/// <returns>XmlDocument or null</returns>
		XmlDocument GetCurrentConditions(string locationID, WeatherUnits units);

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
		XmlDocument GetForecast(string locationID, WeatherUnits units, int days);
	}
}