using System;
using HtmlAgilityPack;
using TurnerSoftware.RobotsExclusionTools;
using System.Collections.Concurrent;

namespace WebCrawler
{
	internal class Program
	{
		static ConcurrentQueue<KeyValuePair<string, int>> urlQueue = new ConcurrentQueue<KeyValuePair<string, int>>();
		static ConcurrentDictionary<string, int> foundUrls = new ConcurrentDictionary<string, int>();
		static LockedInt found = new LockedInt(1);
		static readonly int limit = 334;
		static void Main(string[] args)
		{
			int threadCount = Int32.Parse(args[0]);
			Console.WriteLine($"Running with {threadCount + 1} threads.");

			Console.WriteLine("Please provide a starting URL.");
			var origin = "https://pg.edu.pl";
			//var origin = Console.ReadLine();

			var robotsFileParser = new RobotsFileParser();
			Task<RobotsFile> robotsFileTask = robotsFileParser.FromUriAsync(new Uri($"{origin}/robots.txt"));
			var robotsFile = robotsFileTask.Result;

			var web = new HtmlWeb();
			var document = web.Load(origin);

			urlQueue.Enqueue(new KeyValuePair<string, int>(origin, 0));

			// run this once so that there are actual links in the queue
			CrawlNext(origin, robotsFile, web);

			//threadCount = 0; // uncomment this to simulated a single-threaded crawler
			for (int i = 0; i < threadCount; ++i)
			{
				Task.Run(() => CrawlLoop(origin, robotsFile, web));
			}
			CrawlLoop(origin, robotsFile, web);

			Console.WriteLine(foundUrls.Count);
		}
		static void CrawlLoop(string origin, RobotsFile robotsFile, HtmlWeb web)
		{
			while (urlQueue.Count > 0)
			{
				CrawlNext(origin, robotsFile, web);
			}
		}

		static void CrawlNext(string origin, RobotsFile robotsFile, HtmlWeb web)
		{
			KeyValuePair<string, int> currentUrl;
			if (!urlQueue.TryDequeue(out currentUrl)) return;

			//if (foundUrls.ContainsKey(currentUrl.Key)) return;

			foundUrls.GetOrAdd(currentUrl.Key, currentUrl.Value);
			Console.WriteLine($"Crawling \'{currentUrl.Key}\' [{currentUrl.Value}]...");

			var currentDocument = web.Load(currentUrl.Key);
			var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");
			if (links is null) return;

			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", String.Empty);
				if (!url.StartsWith(origin)) continue;
				if (foundUrls.ContainsKey(url)) continue;
				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawer2221")) continue;
				lock (found)
				{
					if (found.value == limit) return;
					if (foundUrls.ContainsKey(url)) continue;
					var newUrl = new KeyValuePair<string, int>(url, found.value++);
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