/*******************************************************************************
 Cache.cs

 Author:
    Michael Bouck
    
 Description:
    A general purpose in-memory cache (singleton instance per application
	domain) that provides transient storage for any	number of arbritrary
	objects.
 
 Version History:
    2003/03/11 - Initial version.
	
 Design Notes:
    There is a fundamental problem with System.Web.Caching.Cache -- what if your
	application is not a web application and you need some general-purpose cache
	functionality?  What if you're writing middle-tier logic and you're going
	to deploy to a seperate application cluster?  Since there is no equivilent 
	System.Caching.Cache class I decided to write one.  I wanted my cache to 
	"look and feel" as close as possible to the System.Web.Caching.Cache class
	but have no dependency on IIS.	In addition, I really didn't want to have a
	background processing thread so	gave-up some niceities such as sliding and 
	absoulute expiration -- a small	sacrifice for maximum performance.   The 
	design hinges around using weak	references to point to the cache items and
    relying on the GC to decide when an expendable cache item needs to be 
	reclaimed.  In theory, the GC algorithm should provide for an inherent
	"priority mechanism" so that frequently-referenced cache items will be
	collected less often than rarely-referenced items.  Consequently, there 
	is no explicit priority level mechanism provided other than being able to
	specify whether a particular cache item is expendable (i.e. we can GC it)
	or not.
	
 This software is provided "as-is", without any express or implied warranty. 
 In no event will the authors be held liable for any damages arising from the
 use of this software.

 Permission is granted to anyone to use this software for any purpose, 
 including commercial applications, and to alter it and redistribute it 
 freely, but keeping in mind the following would be appreciated:

 1) If you use this software in a product, an acknowledgment in the product
    documentation would give you good karma.

 2) Altered source versions should be plainly marked as such, and should
    not be misrepresented as being the original software.
******************************************************************************/
using System;
using System.Collections;
using System.Collections.Specialized;
using System.Text;
using System.Threading;

// alias collection used by cache
using CacheCollection = System.Collections.Specialized.HybridDictionary;

namespace XoapWeather
{
	/// <summary>
	/// Specifies a <see cref="Cache"/> item's lifetime -- that is whether the item is
	/// to be always available or is expendable.
	/// <seealso cref="Cache"/>
	/// </summary>
	/// <remarks>
	/// A <b>Temporary</b> item added to the cache may be collected by
	/// the garbage collector and is not guaranteed to be available.
	/// By contrast, a <b>Permanent</b> item added to the cache is
	/// guaranteed to always be available unless explicitly removed
	/// from the cache.  The default is <b>Temporary</b>.
	/// </remarks>
	/// <example>
	/// The following example demonstrates how to insert an item in the <see cref="Cache"/>
	/// object with a <b>CacheItemLifetime</b> of <b>Permanent</b>:
	/// <code>
	/// Cache.Instance["key", CacheItemLifetime.Permanent] = "value";
	/// </code>
	/// To insert a temporary item into the cache you can use the single-parameter indexer which
	/// defaults to a <b>CacheItemLifetime</b> of <b>Temporary</b>:
	/// <code>
	/// Cache.Instance["key"] = "value";
	/// </code>
	/// </example>
	public enum CacheItemLifetime
	{
		/// <summary>
		/// The item added to the cache may be collected and is not
		/// guaranteed to be available.  This is the default value.
		/// </summary>
		Temporary,
		/// <summary>
		/// The item added to the cache will <b>not</b> be collected and is
		/// guaranteed to be available for the life of the application domain.
		/// </summary>
		Permanent
	}

	/// <summary>
	/// Defines a key-and-value pair that can be retrieved from the <see cref="Cache"/> object.
	/// <seealso cref="Cache"/>
	/// <seealso cref="ICacheEnumerator"/>
	/// </summary>
	/// <remarks>
	/// The <see cref="ICacheEnumerator.Entry"/> method of the <b>ICacheEnumerator</b> interface returns an instance of this class.
	/// <p>The <b>foreach</b> statement requires the type of each element in the collection.
	/// Since each element in the <see cref="Cache"/> collection is a key-and-value pair, the 
	/// element type is not the type of the key or the type of the value.  Instead, the element 
	/// type is <b>CacheEntry</b>.  For example: <c>foreach (CacheEntry entry in Cache.Instance) {...}</c></p>
	/// </remarks>
	public struct CacheEntry
	{
		#region Fields
		private object _key;
		private object _value;
		#endregion

		#region Properties
		/// <summary>Gets or sets the key in the key-and-value pair.</summary>
		public object Key
		{
			get { return _key; }
			set { _key = value; }
		}
		/// <summary>Gets or sets the value in the key-and-value pair.</summary>
		public object Value
		{
			get { return _value; }
			set { _value = value; }
		}
		#endregion

		#region Methods
		/// <summary>Returns a string representation of the key, value pair.</summary>
		/// <returns>A string representation of the key, value pair (e.g. "Key = MaxThreads, Value = 14").</returns>
		public override string ToString()
		{
			return String.Format("Key = {0}, Value = {1}", Key, Value);
		}
		#endregion
	}

	/// <summary>
	/// Enumerates the items of a <see cref="Cache"/>.
	/// <seealso cref="System.Collections.IEnumerator"/>
	/// </summary>
	/// <remarks>
	/// Enumerators only allow reading the data in the collection. Enumerators cannot be used to modify the underlying collection.
	/// <p>Initially, the enumerator is positioned before the first element in the collection. <see cref="System.Collections.IEnumerator.Reset"/> also brings the enumerator back 
	/// to this position. At this position, calling <see cref="System.Collections.IEnumerator.Current"/> throws an exception. Therefore, 
	/// you must call <see cref="System.Collections.IEnumerator.MoveNext"/> to advance the 
	/// enumerator to the first element of the collection before reading the value of <b>Current</b>.</p>
	/// <p><b>Current</b> returns the same object until either <b>MoveNext</b> or <b>Reset</b> is called. <b>MoveNext</b> sets <b>Current</b> to the next element.</p>
	/// <p>After the end of the collection is passed, the enumerator is positioned after the last element in the collection, and calling <b>MoveNext</b> returns <b>false</b>. 
	/// If the last call to <b>MoveNext</b> returned <b>false</b>, calling <b>Current</b> throws an exception. To set <b>Current</b> to the first element of the collection again, 
	/// you can call <b>Reset</b> followed by <b>MoveNext</b>.</p>
	/// <p>An enumerator remains valid as long as the collection remains unchanged. If changes are made to the collection, such as adding, modifying or deleting elements, 
	/// the enumerator is irrecoverably invalidated and the next call to <b>MoveNext</b> or <b>Reset</b> throws an <see cref="System.InvalidOperationException"/>. If the collection
	/// is modified between <b>MoveNext</b> and <b>Current</b>, <b>Current</b> will return the element that it is set to, even if the enumerator is already invalidated.</p>
	/// </remarks>
	public interface ICacheEnumerator : IEnumerator, IDisposable
	{
		/// <summary>When implemented by a class, gets both the key and the value of the current <see cref="Cache"/> entry.</summary>
		CacheEntry Entry { get; }
		/// <summary>When implemented by a class, gets the key of the current <see cref="Cache"/> entry.</summary>
		object Key { get; }
		/// <summary>When implemented by a class, gets the value of the current <see cref="Cache"/> entry.</summary>
		object Value { get; }
	}

	/// <summary>
	/// A general purpose in-memory cache that provides transient storage for any number of arbritrary objects.
	/// <p><b>Thread Safety</b><p>This type is safe for multithreaded operations.</p></p>
	/// </summary>
	/// <remarks>
	/// One instance of this class is created per application domain, and it remains valid as 
	/// long as the application domain remains active.
	/// </remarks>
	/// <example>
	/// The following example illustrates inserting items into the cache, retrieving items from the cache and iterating over the cache:
	/// <code>
	/// void ExerciseCache()
	/// {
	///     Cache.Instance["key1", CacheItemLifetime.Temporary] = "value1"; // this item will have "temporary" lifetime and may be collected in the future
	///     Cache.Instance["key2"] = "value2";                              // ...so will this item (defaults to CacheItemLifetime.Temorary)
	///     Cache.Instance["key3", CacheItemLifetime.Permanent] = "value3"; // this item is guaranteed to be in the cache as long as the cache is alive
	///     
	///     Console.WriteLine(Cache.Instance["key1"]); // display contents of "key1" from cache
	///     
	///     foreach (CacheEntry entry in Cache.Instance)
	///     {
	///         Console.WriteLine(entry);
	///     }
	///     
	///     Cache.Instance.Clear(); // remove all items from cache
	/// }
	/// </code>
	/// </example>
	public sealed class Cache : IEnumerable
	{
		#region Nested Types
	    private class CacheCollectionElement
		{
			#region Fields
			private	WeakReference	_value;
			private object			_valueStrongRef;
			#endregion

			#region Construction
			public CacheCollectionElement(object value, CacheItemLifetime lifetime)
			{
				_value = new WeakReference(value);
				IsCollectable = (lifetime == CacheItemLifetime.Temporary);
			}
			#endregion

			#region Properties
			public object Value
			{
				get { return _value.Target; }
				set { _value.Target = value; }
			}
			public bool IsCollectable
			{
				get { return (_valueStrongRef == null); }
				set 
				{ 
					if (!value)
					{
						_valueStrongRef = Value;
					}
					else
					{
						_valueStrongRef = null;
					}
				}
			}
			public bool IsCollected
			{
				get { return !_value.IsAlive; }
			}
			#endregion
		}
		private class CacheEnumerator : ICacheEnumerator
		{
			#region Fields
			private IDictionaryEnumerator	_enumerator;
			#endregion

			#region Construction
			public CacheEnumerator(IDictionaryEnumerator enumerator)
			{
				_enumerator = enumerator;
				Cache.Instance._rwl.AcquireReaderLock(Cache.LOCK_TIMEOUT_MSECS);
			}
			#endregion
		
			#region Implementation of ICacheEnumerator
			public CacheEntry Entry
			{
				get
				{
					CacheEntry entry = new CacheEntry();

					entry.Key = _enumerator.Key;
                    
					Cache.CacheCollectionElement element = _enumerator.Value as Cache.CacheCollectionElement;                    
					if (element != null)
					{
						entry.Value = element.Value;
					}
				
					return entry;
				}
			}
			public object Key
			{
				get { return _enumerator.Key; }
			}
			public object Value
			{
				get 
				{ 
					Cache.CacheCollectionElement element = _enumerator.Value as Cache.CacheCollectionElement;                    
					if (element != null)
					{
						return element.Value;
					}

					return null; 
				}
			}
			#endregion

			#region Implementation of IEnumerator
			public void Reset()
			{
				_enumerator.Reset();
			}
			public bool MoveNext()
			{
				return _enumerator.MoveNext();
			}
			public CacheEntry Current
			{
				get { return Entry; }
			}
			object IEnumerator.Current
			{
				get { return Current; }
			}
			#endregion

			#region Implementation of IDisposable
			public void Dispose()
			{
				Cache.Instance._rwl.ReleaseReaderLock();
			}
			#endregion
		}
        #endregion

		#region Constants
		private const int LOCK_TIMEOUT_MSECS = 5000;
		#endregion

		#region Fields
		// We use a public static readonly field for providing singleton access to the cache. This technique takes advantage 
		// of the fact the the CLR will only initialize readonly fields if they are called and also that the CLR guarantees 
		// thread synchronization for static type initialization.  The alternative is to use a static volatile (for thread safety)
		// instance field and a readonly Instance property that uses the common "double-check locking" pattern seen in most Java
		// implementations (although it actually works correctly in .NET).  
		// For more info see: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnbda/html/singletondespatt.asp 
		//
		/// <summary>Provides access to a singleton instance of the <see cref="Cache"/> object.</summary>
		public static readonly Cache	Instance = new Cache();
		private CacheCollection			_data;
		private ReaderWriterLock		_rwl;
		#endregion

		#region Construction
		private Cache()
		{
			_data = new CacheCollection();
			_rwl = new ReaderWriterLock();
		}
		#endregion

		#region Implementation of IEnumerable
		/// <summary>Retrieves a cache enumerator used to iterate through the key settings and their values contained in the cache.</summary>
		/// <returns>An enumerator to iterate through the <see cref="Cache"/> object.</returns>
		public ICacheEnumerator GetEnumerator()
		{
			Purge();

			return new CacheEnumerator(_data.GetEnumerator());
		}
		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}	
		#endregion

		#region Properties
		/// <summary>The current count of items in the <see cref="Cache"/>.</summary>
		public int Count
		{
			get	{ return Purge(); }
		}
		/// <overloads>In C#, this property is the indexer for the <see cref="Cache"/> class.</overloads>
		/// <summary>Gets or sets the cache item at the specified key.</summary>
		public object this[object key]
		{
			get	{ return Get(key); }
			set	{ Insert(key, value); }
		}
		/// <summary>Sets the cache item at the specified key with the specified lifetime.</summary>
		public object this[object key, CacheItemLifetime lifetime]
		{
			set { Insert(key, value, lifetime); }
		}
		#endregion

		#region Methods
		/// <overloads>Inserts an item into the <see cref="Cache"/> object.  Use one of the versions of this method to overwrite an existing <b>Cache</b> item with the same key parameter.</overloads>
		/// <summary>
		/// Inserts an item into the <see cref="Cache"/> object with a temporary lifetime using a cache key to reference its location.
		/// </summary>
		/// <param name="key">The cache key used to reference the item.</param>
		/// <param name="value">The object to be inserted into the cache.</param>
		public void Insert(object key, object value)
		{
			Insert(key, value, CacheItemLifetime.Temporary);
		}
		/// <summary>
		/// Inserts an item into the <see cref="Cache"/> object with a specified lifetime using a cache key to reference its location.
		/// </summary>
		/// <param name="key">The cache key used to reference the item.</param>
		/// <param name="value">The object to be inserted into the cache.</param>
		/// <param name="lifetime">The lifetime of the inserted item (i.e. is it expendable or not).</param>
		public void Insert(object key, object value, CacheItemLifetime lifetime)
		{
			if (key != null)
			{
				_rwl.AcquireWriterLock(LOCK_TIMEOUT_MSECS);
				try
				{
					// remove any existing item for this key
					CacheCollectionElement element = _data[key] as CacheCollectionElement;
					if (element != null)
					{
						element.IsCollectable = true;
						_data.Remove(key);
					}

					// add new item
					element = new CacheCollectionElement(value, lifetime);
					_data[key] = element;
				}
				finally
				{
					_rwl.ReleaseWriterLock();
				}
			}
			else
			{
				throw new ArgumentNullException("key");
			}
		}
		/// <summary>Retrieves the specified item from the <see cref="Cache"/> object.</summary>
		/// <param name="key">The cache key of the item to retrieve.</param>
		/// <returns>The retrieved cache item, or a null reference if the key is not found.</returns>
		public object Get(object key)
		{
			if (key != null)
			{
				_rwl.AcquireReaderLock(LOCK_TIMEOUT_MSECS);
				try
				{
					CacheCollectionElement element = _data[key] as CacheCollectionElement;
					if (element != null)
					{
						if (!element.IsCollected)
						{
							return element.Value;
						}
						else
						{
							// we're modifying the collection so upgrade to writer lock
							LockCookie lockCookie = _rwl.UpgradeToWriterLock(LOCK_TIMEOUT_MSECS);
							
							_data.Remove(key);

							// done -- downgrade to reader lock
							_rwl.DowngradeFromWriterLock(ref lockCookie);
						}
					}
				}
				finally
				{
					// ReleaseLock is used in lieu of ReleaseWriterLock/ReleaseReaderLock due to the lock upgrade logic.
					// Since we don't need to support nested locks here this should be OK...
					_rwl.ReleaseLock();
				}
			}
			else
			{
				throw new ArgumentNullException("key");
			}
            
			return null;
		}
		/// <summary>Removes the specified item from the <see cref="Cache"/> object.</summary>
		/// <param name="key">The cache key of the item to remove.</param>
		/// <returns>The item removed from the cache.  If the value in the key parameter is not found, returns a null reference.</returns>
		public object Remove(object key)
		{
			if (key != null)
			{
				_rwl.AcquireWriterLock(LOCK_TIMEOUT_MSECS);
				try
				{
					CacheCollectionElement element = _data[key] as CacheCollectionElement;
					if (element != null)
					{
						object value = element.Value;
						element.IsCollectable = true;
						_data.Remove(key);

						return value;
					}
				}
				finally
				{
					_rwl.ReleaseWriterLock();
				}
			}
			else
			{
				throw new ArgumentNullException("key");
			}

			return null;
		}
		/// <summary>Removes all items from the <see cref="Cache"/> object.</summary>
		public void Clear()
		{	
			_rwl.AcquireWriterLock(LOCK_TIMEOUT_MSECS);
			try
			{
				if (_data.Count > 0)
				{
					foreach (object key in _data.Keys)
					{
						if (key != null)
						{
							CacheCollectionElement element = _data[key] as CacheCollectionElement;
							if (element != null)
							{
								element.IsCollectable = true;
							}
						}	
					}

					_data.Clear();
				}
			}
			finally
			{
				_rwl.ReleaseWriterLock();
			}
		}
		/// <summary>Compresses the <see cref="Cache"/>, removing references to collected items (if any).</summary>
		/// <returns>The count of live items in the <see cref="Cache"/>.</returns>
		private int Purge()
		{
			int liveItemCount = 0;

			_rwl.AcquireWriterLock(LOCK_TIMEOUT_MSECS);
			try
			{
				// if no items in Cache, we're done...
				if (_data.Count > 0)
				{
					// check to see if the data for any elements has been collected
					ArrayList collected = new ArrayList();
					
					foreach (object key in _data.Keys)
					{
						if (key != null)
						{
							CacheCollectionElement element = _data[key] as CacheCollectionElement;
							if (element != null)
							{
								if (element.IsCollected)
								{
									collected.Add(key);
								}
							}
						}
					}

					// remove collected elements from the cache
					if (collected.Count > 0)
					{
						foreach (object key in collected)
						{
							_data.Remove(key);	
						}
					}

					// the Cache item count should now only refer to live, accessible items...
					liveItemCount = _data.Count;
				}
			}
			finally
			{
				_rwl.ReleaseWriterLock();
			}

			return liveItemCount;
		}
		/// <summary>Returns a string representation of the contents of the <see cref="Cache"/> object.</summary>
		/// <returns>A string representation of the contents of the cache and the number of items in the cache.</returns>
		public override string ToString()
		{
			StringBuilder	sb = new StringBuilder();
			int				cacheItemCount = Purge();

			if (cacheItemCount > 0)
			{
				sb.AppendFormat("Total items in cache = {0:d}, contents:", cacheItemCount);

				// manually iterate over enumerator because our implementation of GetEnumerator() first calls Purge() which
				// we've already done at this point...
				CacheEnumerator enumerator = new CacheEnumerator(_data.GetEnumerator());
				try
				{
					while (enumerator.MoveNext()) 
					{
						sb.AppendFormat("\r\n{0}", enumerator.Current);
					}
				}
				finally
				{
					enumerator.Dispose();
				}
			}
			else
			{
				sb.Append("Cache is empty.");
			}

			return sb.ToString();
		}
		#endregion
	}
}
