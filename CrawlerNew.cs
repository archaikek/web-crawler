using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using HtmlAgilityPack;
using TurnerSoftware.RobotsExclusionTools;
using System.Security;
using System.Net;
using System.Text.RegularExpressions;
using System.Reflection;
using System.Security.Cryptography.X509Certificates;
using System.Web;
using Microsoft.Win32.SafeHandles;

namespace web_crawler
{
	internal class CrawlerNew
	{
		private static readonly int limit = 111;

		public static ConcurrentDictionary<string, string> destinationCache = new ConcurrentDictionary<string, string>();

		public static Graph graph = new Graph(limit);
		public static ConcurrentQueue<QueuedRequest> remainingRequests = new ConcurrentQueue<QueuedRequest>();

		private static Mutex indexMutex = new Mutex(false);
		private static int index = 0;


		private string origin { get; set; } = String.Empty;
		private RobotsFile robotsFile { get; set; } = null;
		private HtmlWeb web { get; set; } = null;
		private HttpClient client { get; set; } = null;

		public CrawlerNew(string _origin, RobotsFile _robotsFile)
		{
			origin = _origin;
			robotsFile = _robotsFile;
			web = new HtmlWeb();
			web.OverrideEncoding = System.Text.Encoding.UTF8;
			web.PreRequest += request =>
			{
				request.CookieContainer = new System.Net.CookieContainer();
				return true;
			};
			client = new HttpClient();
			client.Timeout = new TimeSpan(0, 0, 20);
		}

		public void CrawlLoop()
		{
			while (index < limit)
			{
				CrawlNext();
			}
		}
		public async void CrawlNext()
		{
			//if (remainingRequests.Count() > 0) Console.WriteLine($"[{Thread.CurrentThread.ManagedThreadId}] Queue state: {remainingRequests.Count()}, Index: {index}/{limit}, Found: {found.HashSet.Count()}");
			//else Console.Write(".");
			QueuedRequest request;
			if (!remainingRequests.TryDequeue(out request)) // the queue is empty for some reason
			{
				//Thread.Sleep(50); // do not clog the access to the queue with repeated attempts to access
				// actually maybe let them spam it a bit
				return; 
			}

			string currentUrl = NormalizeUrl(request.url);
			//if (blacklist.Contains(currentUrl))
			//{
			//	Console.WriteLine($"? {currentUrl} BLACKLISTED");
			//	return; // the url entered the blacklist in the meantime
			//}
			HtmlDocument currentDocument = request.document;
			if (currentDocument is null) // failed to load the document
			{
				//blacklist.Add(currentUrl);
				return;
			}

			int currentIndex = GetNextIndex();
			if (currentIndex >= limit) return; // the limit was reached somewhere in the meantime
			graph.Nodes[currentUrl] = currentIndex;
			Console.WriteLine($"Crawling {currentUrl}... [{currentIndex}]");

			Thread saver = new Thread(() => {
				using (FileStream fs = File.OpenWrite(MakeFileName(currentIndex, currentUrl)))
				{
					currentDocument.Save(fs);
				}
			});
			saver.Start();

			var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");

			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", String.Empty);
				if (string.IsNullOrEmpty(url)) continue; // something's wrong

				url = NormalizeUrl(url);
				if (string.IsNullOrEmpty(url)) continue; // something's wrong only now somehow
				
				url = NormalizeUrl(CheckFinalDestination(url));
				if (url is null) continue;

				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawler2221")) continue;  // Robots not allowe
				if (graph.Nodes.ContainsKey(url)) continue; // URL already found and saved

				HtmlDocument doc = web.TryLoad(url);
				if (doc.DocumentNode.SelectNodes("//a[@href]").Count == 0)
				{
					destinationCache[url] = null;
					continue;
				}
				int index = GetNextIndex();

				remainingRequests.Enqueue(new QueuedRequest(url, doc));
			}
			saver.Join();
			Console.WriteLine($"Finished crawling {currentIndex}.");
		}

		private int GetNextIndex()
		{
			int result;
			indexMutex.WaitOne();
			result = index++;
			indexMutex.ReleaseMutex();
			return result;
		}
		private string NormalizeUrl(string url)
		{
			string result = url;
			if (!result.StartsWith(origin)) result = origin + (result.StartsWith('/') ? result.Substring(1) : result);

			int index = result.IndexOf('?');
			if (index != -1) result = result.Substring(0, index);
			index = result.IndexOf('#');
			if (index != -1) result = result.Substring(0, index);

			return result;
		}


		public static string MakeFileName(int index, string currentUrl)
		{
			string suffix = currentUrl.EndsWith(".html") ? "" : ".html"; // add .html to documents that don't have it
			foreach (char c in System.IO.Path.GetInvalidFileNameChars()) currentUrl = currentUrl.Replace(c, '_');
			return Program.pagesPath + $"{index}_{currentUrl}{suffix}";
		}
		private string CheckFinalDestination(string url)
		{
			if (destinationCache.ContainsKey(url)) return destinationCache[url];

			client.BaseAddress = new Uri(url);
			HttpResponseMessage response;
			try
			{
				response = client.GetAsync(client.BaseAddress).Result;
			}
			catch (Exception ex)
			{
				return destinationCache[url] = null;
			}

			destinationCache[url] = null;
			if (response is null) return null;
			if (response.StatusCode != HttpStatusCode.OK) return null;
			if (!response.RequestMessage.RequestUri.ToString().StartsWith(origin)) return null;
			if (response.Content.Headers.ContentType.MediaType != "text/html") return null;

			return destinationCache[url] = response.RequestMessage.RequestUri.ToString();
		}

		public class QueuedRequest
		{
			public string url;
			public HtmlDocument document;
			public QueuedRequest(string _url, HtmlDocument _document)
			{
				url = _url;
				document = _document;
			}
		}
		public class ConcurrentHashset<T> : HashSet<T>
		{
			private Mutex mutex;
			private HashSet<T> hashSet;
			public HashSet<T> HashSet { get { return hashSet; } }
			public ConcurrentHashset()
			{
				mutex = new Mutex(false);
				hashSet = new HashSet<T>();
			}

			public new bool Add(T item)
			{
				bool result;
				mutex.WaitOne();
				result = hashSet.Add(item);
				mutex.ReleaseMutex();
				return result;
			}
			public new bool Contains(T item)
			{
				bool result;
				mutex.WaitOne();
				result = hashSet.Contains(item);
				mutex.ReleaseMutex();
				return result;
			}
		}
	}
}
