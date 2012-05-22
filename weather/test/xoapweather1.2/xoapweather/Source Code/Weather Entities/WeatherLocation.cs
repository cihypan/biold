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
using System.Xml.Serialization;

namespace XoapWeather.Entity
{
	/// <summary>
	/// Search Location Entity returned as an array by the SearchForLocation method
	/// containing the location ID (ID field) and description (Description).
	/// </summary>
	public class WeatherLocation
	{
		string _description;
		string _locationId;

		/// <summary>
		/// Gets or sets the region full description.
		/// Supports legacy config files that has no description and so grabs the code instead.
		/// </summary>
		/// <value>Description string.</value>
		[XmlElementAttribute("description", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public string Description
		{
			get { return _description; }
			set { _description = value; }
		}

		/// <summary>
		/// Gets or sets the weather location id code.
		/// </summary>
		/// <value>Location Code string.</value>
		[XmlElementAttribute("code", Form=System.Xml.Schema.XmlSchemaForm.Unqualified)]
		public string LocationId
		{
			get { return this._locationId; }
			set { this._locationId = value; }
		}
	}
}