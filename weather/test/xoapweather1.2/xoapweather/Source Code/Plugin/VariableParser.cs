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
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.XPath;
using System.Data;

namespace XoapWeather.Plugin
{
	/// <summary>
	/// Summary description for Parser.
	/// </summary>
	internal class VariableParser
	{
		// Compile regular expression only once
		private static Regex matchExp = new Regex(@"(?:%(?<Variable>.*?)%|(?<Variable>^[a-zA-Z]+$))");

		// Internal state variables
		private DataTable _dt;
		private WeatherRegionForecast _regionForecast;
		private string _xPathReplace;

		/// <summary>
		/// Parses the input string for variable substitution.
		/// </summary>
		/// <param name="location">Weather Forecast Location.</param>
		/// <param name="replacement">Day number replacement.</param>
		/// <param name="dt">Variable to xpath translation table.</param>
		/// <param name="input">Input string.</param>
		/// <returns></returns>
		public string Parse(WeatherRegionForecast location, string replacement, DataTable dt, string input)
		{
			this._regionForecast = location;
			this._dt = dt;
			this._xPathReplace = replacement;
			return matchExp.Replace(input, new MatchEvaluator(this.VariableLookup));
		}

		/// <summary>
		/// Heavy lifting method called by the MatchEvaluator only
		/// </summary>
		/// <param name="m">Match.</param>
		/// <returns>String</returns>
		private string VariableLookup(Match m) 
		{
			string varName;
			string formatString = null;
			string results;
			
			// Get the variable name from the match
			varName = m.Groups["Variable"].Value;

			// Check for a formatting extension (signified by the : character)
			if (varName.LastIndexOf(':') > 0)
			{
				formatString = "{0:" + varName.Substring(varName.IndexOf(':')) + "}";
				varName = varName.Substring(0, varName.IndexOf(':')-1);
			}
			// Do a select on the variables table to find the name
			DataRow[] rows = _dt.Select("Variable='" + varName + "'");
			// If we don't find that variable name, return an informational error
			if (rows.Length == 0) return String.Format("Unknown variable '{0}'", m.Value);
			// Get the XPATH statement
			string xpath = (string) rows[0]["XPath"];
			// Format query and replace a # symbol in the xpath with another value passed in
			xpath = String.Format("string({0})", xpath.Replace("#", _xPathReplace));
			// Select the data from the Xml Document
			try 
			{ 
				results = _regionForecast.Evaluate(xpath);
			}
			catch ( SystemException e )
			{
				// Return the XPath exception error string for debugging
				return e.Message;
			}
			results = results.Trim();
			// Check for formatting requirement
			if (formatString != null)
				String.Format(System.Globalization.CultureInfo.CurrentCulture, formatString, results);
			return results;
		}
	}
}