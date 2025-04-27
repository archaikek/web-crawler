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

namespace web_crawler
{
	internal class Crawler
	{
		private static LockedInt foundCount = new LockedInt(1);
		private static Semaphore sem = new Semaphore(1, 1);
		private static Mutex mut = new Mutex(false);
		public static ConcurrentDictionary<string, string> destinationCache = new ConcurrentDictionary<string, string>();
		public static readonly int limit = 4444;

		public static Graph graph { get; set; } = null;
		public static ConcurrentQueue<KeyValuePair<string, int>> urlQueue { get; } = new ConcurrentQueue<KeyValuePair<string, int>>();

		private string origin { get; set; } = String.Empty;
		private RobotsFile robotsFile { get; set; } = null;
		private HtmlWeb web { get; set; } = null;
		public HtmlWeb Web { get { return web; } }

		public Crawler()
		{
			web = new HtmlWeb();
			web.OverrideEncoding = System.Text.Encoding.UTF8;

			if (graph is null)
			{
				graph = new Graph(limit);
				for (int i = 0; i < limit; ++i) graph[i] = new HashSet<int>();
			}
		}
		public Crawler(string _origin, RobotsFile _robotsFile)
		{
			web = new HtmlWeb();
			web.OverrideEncoding = System.Text.Encoding.UTF8;

			if (graph is null)
			{
				graph = new Graph(limit);
				for (int i = 0; i < limit; ++i) graph[i] = new HashSet<int>();
			}

			origin = _origin;
			robotsFile = _robotsFile;
		}

		public void CrawlLoop()
		{
			while (urlQueue.Count > 0)
			{
				CrawlNext();
			}
			Console.WriteLine($"Thread finished. Total found: {foundCount.value}\n");
		}

		public void CrawlNext()
		{
			KeyValuePair<string, int> currentUrl;
			if (!urlQueue.TryDequeue(out currentUrl)) return;

			var currentDocument = web.TryLoad(currentUrl.Key);
			var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");

			var foundUrls = graph.Nodes;

			foundUrls.GetOrAdd(currentUrl.Key, currentUrl.Value);
			Console.WriteLine($"Crawling \'{currentUrl.Key}\' [{currentUrl.Value}]...");

			Thread saver = new Thread(() => {
				KeyValuePair<string, int> temp = new KeyValuePair<string, int>(currentUrl.Key, currentUrl.Value);
				using (FileStream fs = File.OpenWrite(MakeFileName(temp.Value, temp.Key)))
				{
					currentDocument.Save(fs);
				}
			});
			saver.Start();

			if (links is null) return;

			foreach (var link in links)
			{
				//if (foundCount.value >= limit) break;

				var url = link.GetAttributeValue("href", String.Empty);
				if (url.StartsWith('/')) url = origin + url.Substring(1);
				// if the page does not belong to the origin domain
				if (!url.StartsWith(origin)) continue;
				// if the page is a html (this will obviously fail if a location without an extension leads to anything other than an html, hopefully it's not an issue)
				//if (Regex.Matches(url.Substring(url.Length - 5), ".").Count >= 1 && !url.EndsWith(".html")) continue;

				int index = url.IndexOf('?');
				if (index != -1) url = url.Substring(0, index);
				index = url.IndexOf('#');
				if (index != -1) url = url.Substring(0, index);

				url = CheckFinalDestination(url);
				if (url is null) continue;

				index = url.IndexOf('?'); // do this again for good measure
				if (index != -1) url = url.Substring(0, index);
				index = url.IndexOf('#');
				if (index != -1) url = url.Substring(0, index);

				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawer2221")) continue;

				mut.WaitOne();
				{
					if (foundUrls.ContainsKey(url))
					{
						graph[currentUrl.Value].Add(foundUrls[url]);
						mut.ReleaseMutex();
						continue;
					}
					else if (foundCount.value >= limit)
					{
						mut.ReleaseMutex();
						continue;
					}
					else
					{
						graph[currentUrl.Value].Add(foundCount.value);
					}

					var newUrl = new KeyValuePair<string, int>(url, foundCount.value++);
					foundUrls.TryAdd(newUrl.Key, newUrl.Value);
					urlQueue.Enqueue(newUrl);
				}
				mut.ReleaseMutex();
			}

			saver.Join();
			Console.WriteLine($"Finished crawling page {currentUrl.Value}");
		}

		private string MakeFileName(int index, string currentUrl)
		{
			string suffix = currentUrl.EndsWith(".html") ? "" : ".html"; // add .html to documents that don't have it
			return Program.pagesPath + $"{index}_{currentUrl.Replace(':', '-').Replace('/', '_').Replace('\\', '_')}{suffix}";
		}
		private string CheckFinalDestination(string url)
		{
			if (destinationCache.ContainsKey(url)) return destinationCache[url];

			WebRequest request = WebRequest.Create(url);
			WebResponse response;
			try
			{
				response = request.GetResponse();
			}
			catch (WebException ex)
			{
				destinationCache[url] = null;
				return null;
			}
			destinationCache[url] = null;
			if (response is null) return null;
			if (!response.ResponseUri.ToString().StartsWith(origin)) return null;
			if (response.ContentType != "text/html") return null;

			return destinationCache[url] = response.ResponseUri.ToString();
		}

		internal class LockedInt
		{
			public int value;

			public LockedInt(int value)
			{
				this.value = value;
			}
		}
	}
}
