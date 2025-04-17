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
		public static readonly int limit = 4444;

		public static Graph graph { get; set; } = null;
		public static ConcurrentQueue<KeyValuePair<string, int>> urlQueue { get; } = new ConcurrentQueue<KeyValuePair<string, int>>();

		public static string origin { private get; set; } = String.Empty;
		public static RobotsFile robotsFile { private get; set; } = null;
		public static HtmlWeb web { private get; set; } = null;

		private static int pdfSkipCounter = 0;
		private static readonly int pdfSkipLimit = 44;
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
			Console.WriteLine($"Thread finished. Total found: {foundCount.value}\n");
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

			Thread saver = new Thread(() => {
				if (currentUrl.Key.Substring(currentUrl.Key.Length - 4) == ".pdf")
				{
					try
					{
						using (var client = new WebClient()) client.DownloadFile(currentUrl.Key, MakeFileName(currentUrl.Value, currentUrl.Key));
					} 
					catch { }
				}
				else
				{
					using (FileStream fs = File.OpenWrite(MakeFileName(currentUrl.Value, currentUrl.Key)))
					{
						currentDocument.Save(fs);
					}
				}
			});
			saver.Start();

			if (links is null) return;

			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", String.Empty);
				if (url.StartsWith('/')) url = origin + url.Substring(1);
				if (!url.StartsWith(origin)) continue;
				if (Regex.Matches(url, "pdf").Count > 1) continue; // save some more interesting pdfs, but generally skip those in major pdf dump folders
				if (Regex.Matches(url, "pdf").Count == 1) // also allow only 1 in pdfSkipLimit pdfs (apparently there are way too many of them)
				{
					++pdfSkipCounter; // no lock here because the precision isn't that important here
					if (pdfSkipCounter >= pdfSkipLimit) pdfSkipCounter = 0;
					else continue;
				}

				int index = url.IndexOf('?');
				if (index != -1) url = url.Substring(0, index);
				index = url.IndexOf('#');
				if (index != -1) url = url.Substring(0, index);

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

			saver.Join();
			Console.WriteLine($"Finished crawling page {currentUrl.Value}");
		}

		private string MakeFileName(int index, string currentUrl)
		{
			string suffix = "";
			if (!(currentUrl.Substring(currentUrl.Length - 5, 5) == ".html") && !(currentUrl.Substring(currentUrl.Length - 4, 4) == ".pdf")) suffix = ".html";
			return Program.pagesPath + $"{index}_{currentUrl.Replace(':', '-').Replace('/', '_').Replace('\\', '_')}{suffix}";
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
