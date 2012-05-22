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
using System.IO;
using System.Globalization;
using System.Collections.Specialized;
using System.Threading;
using System.Diagnostics;


namespace XoapWeather.Plugin
{
	/// <summary>
	/// The Xoap Weather Plugin for Xlobby2 provides weather text and images from
	/// Weather.com through the Xoap.Weather.Com XML interface.
	/// </summary>
	public sealed class Logger : TraceListener
	{
		#region Member Constants
		/// <summary>
		/// Default size of log buffer
		/// </summary>
		private const int BUFFER_SIZE  = 200;   // specified in lines
		private const int LOCK_TIMEOUT = 15000; // 15 seconds
		#endregion

		#region Member Variables
		/// <summary>
		/// Log buffer
		/// </summary>
		private StringCollection _buffer = new StringCollection();
		private ReaderWriterLock _rwLock = new ReaderWriterLock();
		#endregion

		// Explicit static constructor to tell C# compiler not to mark type as beforefieldinit
		static Logger() { }

		#region Singleton Pattern Implementation
		// Property to store singleton instance of this class
		static readonly Logger _instance = new Logger();


		/// <summary>
		/// Factory method returns singleton instance of XoapProvider
		/// </summary>
		/// <returns>XoapProvider singleton</returns>
		private static Logger Instance
		{
			get { return _instance; }
		}
		#endregion

		#region Properties
		/// <summary>
		/// Gets the log datasource.
		/// </summary>
		/// <value></value>
		internal static StringCollection Log
		{
			get { return Instance._buffer; }
		}
		#endregion

		#region Events
		/// <summary>
		/// Changed event is called each time a new log entry is written.
		/// </summary>
		public static event EventHandler Changed;

		private void OnChanged(EventArgs e) 
		{
			if (Changed != null)
				Changed(this,e);
		}
		#endregion

		#region Public Methods
		/// <summary>
		/// Starts this logger.
		/// </summary>
		public static void Start()
		{
			if (! Trace.Listeners.Contains(Instance))
				Trace.Listeners.Add(Instance);
		}

		/// <summary>
		/// Stops this instance of the event logger.
		/// </summary>
		public static void Stop()
		{
			Trace.Listeners.Remove(Instance);
		}

		/// <summary>
		/// Gets a value indicating whether the logger has been started.
		/// </summary>
		/// <value>
		/// 	<c>true</c> if [is started]; otherwise, <c>false</c>.
		/// </value>
		public static bool IsStarted
		{
			get { return Trace.Listeners.Contains(Instance); }
		}

		/// <summary>
		/// Writes the specified string to the log.
		/// </summary>
		/// <param name="s">S.</param>
		public override void Write(string s)
		{
			lock (_buffer.SyncRoot)
			{
				while (_buffer.Count > BUFFER_SIZE)
					_buffer.RemoveAt(0);
				_buffer.Add(String.Format("[{0:HH:mm:ss}] {1}", DateTime.Now, s));
			}
			OnChanged(EventArgs.Empty);
		}

		/// <summary>
		/// Writes the string to the log.
		/// </summary>
		/// <param name="s">S.</param>
		public override void WriteLine(string s)
		{
			Write(s);
		}

		/// <summary>
		/// Dumps the log to file.
		/// </summary>
		/// <param name="fileName">Name of the file.</param>
		public static void DumpToFile(string fileName)
		{
			lock (Instance._buffer.SyncRoot)
			{
				using (StreamWriter sw = new StreamWriter(fileName))
					foreach (object line in Instance._buffer)
						sw.WriteLine(line);
			}
		}
		
		#endregion

		#region Private Methods
		#endregion

	}
}
