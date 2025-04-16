using System;
using HtmlAgilityPack;
using TurnerSoftware.RobotsExclusionTools;
using System.Collections;
using System.Collections.Concurrent;
using System.Numerics;

namespace web_crawler
{
	internal class Program
	{
		private static ConcurrentQueue<KeyValuePair<string, int>> urlQueue = new ConcurrentQueue<KeyValuePair<string, int>>();
		private static ConcurrentDictionary<string, int> foundUrls = new ConcurrentDictionary<string, int>();
		
		private static LockedInt found = new LockedInt(1);
		private static readonly int limit = 34;

		private static readonly int maxAttempts = 50;
		private static readonly int attemptTimeout = 200;

		private static HashSet<int>[] graph = new HashSet<int>[limit];
		static void Main(string[] args)
		{
			for (int i = 0; i < limit; ++i) graph[i] = new HashSet<int>();

			int threadCount = Int32.Parse(args[0]);
			Console.WriteLine($"Running with {threadCount} threads.");

			Console.WriteLine("Please provide a starting URL.");
			var origin = "https://pg.edu.pl";
			//var origin = Console.ReadLine();

			var robotsFileParser = new RobotsFileParser();
			Task<RobotsFile> robotsFileTask = robotsFileParser.FromUriAsync(new Uri($"{origin}/robots.txt"));
			var robotsFile = robotsFileTask.Result;

			var web = new HtmlWeb();
			var document = TryLoad(web, origin);

			urlQueue.Enqueue(new KeyValuePair<string, int>(origin, 0));

			// run this once so that there are actual links in the queue
			CrawlNext(origin, robotsFile, web);

			//threadCount = 1; // uncomment this to simulate a single-threaded crawler
			Task[] tasks = new Task[threadCount - 1];
			for (int i = 1; i < threadCount; ++i)
			{
				tasks[i - 1] = Task.Run(() => CrawlLoop(origin, robotsFile, web));
			}
			CrawlLoop(origin, robotsFile, web);
			Task.WaitAll(tasks);

			Console.WriteLine(foundUrls.Count);
			Console.WriteLine(GraphToString(graph));
			Console.WriteLine(NodesToString(foundUrls));
		}
		private static void CrawlLoop(string origin, RobotsFile robotsFile, HtmlWeb web)
		{
			while (urlQueue.Count > 0)
			{
				CrawlNext(origin, robotsFile, web);
			}
		}

		private static void CrawlNext(string origin, RobotsFile robotsFile, HtmlWeb web)
		{
			KeyValuePair<string, int> currentUrl;
			if (!urlQueue.TryDequeue(out currentUrl)) return;

			//if (foundUrls.ContainsKey(currentUrl.Key)) return;

			foundUrls.GetOrAdd(currentUrl.Key, currentUrl.Value);
			Console.WriteLine($"Crawling \'{currentUrl.Key}\' [{currentUrl.Value}]...");

			var currentDocument = TryLoad(web, currentUrl.Key);
			var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");
			if (links is null) return;

			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", String.Empty);
				if (!url.StartsWith(origin)) continue;
				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawer2221")) continue;

				lock (found)
				{
					if (foundUrls.ContainsKey(url))
					{
						graph[currentUrl.Value].Add(foundUrls[url]);
						continue;
					}
					else
					{
						graph[currentUrl.Value].Add(found.value);
					}
					if (found.value == limit) continue;

					var newUrl = new KeyValuePair<string, int>(url, found.value++);
					foundUrls.TryAdd(newUrl.Key, newUrl.Value);
					urlQueue.Enqueue(newUrl);
				}
			}
		}


		private static string GraphToString(HashSet<int>[] graph)
		{
			string result = "";
			for (int i = 0; i < limit; ++i)
			{
				result += $"{i}\n";
				var destinations = graph[i].ToArray();
				for (int j = 0 ; j < destinations.Length; ++j)
				{
					result += $"{destinations[j]} ";
				}
				result += "\n";
			}
			return result;
		}
		private static string NodesToString(ConcurrentDictionary<string, int> foundUrls)
		{
			string result = "";
			foreach (var url in foundUrls)
			{
				result += $"{url.Value} {url.Key}\n";
			}
			return result;
		}
		private static HtmlDocument TryLoad(HtmlWeb web, string url)
		{
			int attempts = 0;
			HtmlDocument result = null;
			while (true)
			{
				try
				{
					result = web.Load(url);
					break;
				}
				catch (Exception e)
				{
					if (attempts == maxAttempts) throw e;
					attempts++;
					Thread.Sleep(attemptTimeout);
				}
			}
			return result;
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