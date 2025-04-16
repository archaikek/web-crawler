using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using HtmlAgilityPack;
using TurnerSoftware.RobotsExclusionTools;

namespace web_crawler
{
	internal class Crawler
	{
		private static LockedInt foundCount = new LockedInt(1);
		public static readonly int limit = 34;

		public static Graph graph { get; set; } = null;
		public static ConcurrentQueue<KeyValuePair<string, int>> urlQueue { get; } = new ConcurrentQueue<KeyValuePair<string, int>>();

		public static string origin { private get; set; } = String.Empty;
		public static RobotsFile robotsFile { private get; set; } = null;
		public static HtmlWeb web { private get; set; } = null;
		public Crawler() 
		{
			if (graph is null)
			{
				graph = new Graph(limit);
				for (int i = 0;  i < limit; ++i) graph[i] = new HashSet<int>();
			}
		}

		public void CrawlLoop()
		{
			while (urlQueue.Count > 0)
			{
				CrawlNext();
			}
		}

		public void CrawlNext()
		{
			KeyValuePair<string, int> currentUrl;
			if (!urlQueue.TryDequeue(out currentUrl)) return;

			//if (foundUrls.ContainsKey(currentUrl.Key)) return;

			var foundUrls = graph.Nodes;

			foundUrls.GetOrAdd(currentUrl.Key, currentUrl.Value);
			Console.WriteLine($"Crawling \'{currentUrl.Key}\' [{currentUrl.Value}]...");

			var currentDocument = web.TryLoad(currentUrl.Key);
			var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");
			if (links is null) return;

			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", String.Empty);
				if (!url.StartsWith(origin)) continue;
				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawer2221")) continue;

				lock (foundCount)
				{
					if (foundUrls.ContainsKey(url))
					{
						graph[currentUrl.Value].Add(foundUrls[url]);
						continue;
					}
					else
					{
						graph[currentUrl.Value].Add(foundCount.value);
					}
					if (foundCount.value == limit) continue;

					var newUrl = new KeyValuePair<string, int>(url, foundCount.value++);
					foundUrls.TryAdd(newUrl.Key, newUrl.Value);
					urlQueue.Enqueue(newUrl);
				}
			}
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
