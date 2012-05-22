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
using System.Net;
using System.Xml;
using System.Xml.XPath;
using System.Xml.Serialization;
using System.Collections;
using XoapWeather.Provider;

namespace XoapWeather.Entity
{
	/// <summary>
	/// The WeatherRegionBase entity provides a container to define a specific region that the user
	/// has requested we track weather for. The configuration properties are serializable for persistance.
	/// </summary>
	public class WeatherRegionBase : WeatherLocation
	{
		// Property backing variables
		private bool			_metric;
		private int				_forecastDays;
		private string			_radarmap;

		/// <summary>
		/// Gets or sets the number of days of weather forecast information desired
		/// </summary>
		/// <value></value>
		[XmlElementAttribute("days", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public int ForecastDays
		{
			get { return _forecastDays; }
			set { _forecastDays = value; }
		}

		/// <summary>
		/// Gets or sets the metric flag (true or false) of the data units.
		/// </summary>
		/// <value>string (True or False)</value>
		[XmlElementAttribute("metric", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public string Metric
		{
			get { return (_metric ? "True" : "False"); }
			set { _metric = (value.ToLower() == "true"); }
		}

		/// <summary>
		/// Gets or sets the radar map.
		/// </summary>
		/// <value></value>
		[XmlElementAttribute("radar", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public string RadarMap
		{
			get { return (_radarmap == null) ? String.Empty : _radarmap; }
			set { _radarmap = value; }
		}

		/// <summary>
		/// Helper method gets or sets a value indicating whether we want metric values.
		/// This property is not serialized.
		/// </summary>
		/// <value>
		/// 	<c>true</c> if metric; otherwise, <c>false</c>.
		/// </value>
		[XmlIgnore]
		public bool IsMetric
		{
			get { return (_metric == true); }
			set { _metric = value; }
		}
	}
}