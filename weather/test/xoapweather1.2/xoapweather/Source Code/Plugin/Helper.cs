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
using System.Xml;
using System.Xml.Serialization;
using System.Data;
using System.Reflection;

namespace XoapWeather
{
	/// <summary>
	/// Helper Utilities
	/// </summary>
	internal sealed class Helper
	{
		/// <summary>
		/// Gets the assembly path.
		/// </summary>
		/// <returns></returns>
		public static string GetPath(string filename)
		{
			return Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetModules()[0].FullyQualifiedName) + "\\" + filename;
		}

		/// <summary>
		/// Deserializes this instance.
		/// </summary>
		/// <param name="filename">Filename.</param>
		/// <param name="type">Type.</param>
		/// <returns></returns>
		public static object DeserializeObject(string filename, System.Type type)
		{
			XmlSerializer serializer = new XmlSerializer(type);
			XmlTextReader reader;

			if (File.Exists(GetPath(filename)))
                reader = new XmlTextReader(GetPath(filename));
			else
			reader = new XmlTextReader(GetResource(filename));
			object obj = serializer.Deserialize(reader);
			reader.Close();
			return obj;
		}

		/// <summary>
		/// Serializes this instance.
		/// </summary>
		/// <param name="filename">Filename.</param>
		/// <param name="obj">Obj.</param>
		/// <param name="type">Type.</param>
		public static void SerializeObject(string filename, object obj, System.Type type)
		{
			XmlSerializer serializer = new XmlSerializer(type);
			XmlTextWriter writer = new XmlTextWriter(GetPath(filename), System.Text.Encoding.UTF8);
			writer.Formatting = System.Xml.Formatting.Indented;
			serializer.Serialize(writer, obj);
			writer.Close();
		}

		public static DataTable DeserializeTable(string filename)
		{
			DataSet ds = new DataSet();
			ds.Locale = System.Globalization.CultureInfo.InvariantCulture;

			if (File.Exists(GetPath(filename)))
				ds.ReadXml(GetPath(filename));
			else
				ds.ReadXml(GetResource(filename));
			DataTable dt = ds.Tables[0];
			ds.Tables.Remove(dt);
			return dt;
		}
	
		public static void SerializeTable(string filename, DataTable table)
		{
			DataSet ds = new DataSet("XoapWeather");
			ds.Locale = System.Globalization.CultureInfo.InvariantCulture;
			ds.Tables.Add(table);
			ds.WriteXml(GetPath(filename), XmlWriteMode.WriteSchema);
		}

		/// <summary>
		/// Gets the pretty-print version of this application/assembly
		/// </summary>
		/// <returns></returns>
		public static string GetVersion()
		{
			System.Version ver = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
			return String.Format("{0}.{1} (Build {2})", ver.Major, ver.Minor, ver.Build);
		}

		/// <summary>
		/// Gets a resource stream
		/// </summary>
		/// <param name="name">Name.</param>
		/// <returns></returns>
		public static Stream GetResource(string name)
		{
			string[] resources = Assembly.GetExecutingAssembly().GetManifestResourceNames();
			foreach (string resourcename in resources)
				if (resourcename.EndsWith(name))
					return Assembly.GetExecutingAssembly().GetManifestResourceStream(resourcename);
			throw new ArgumentException("Resource not found in manifest", name);
		}
	}
}