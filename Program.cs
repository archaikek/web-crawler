using System;
using HtmlAgilityPack;
using TurnerSoftware.RobotsExclusionTools;
using System.Collections;
using System.Collections.Concurrent;
using System.Numerics;
using System.Xml.Xsl;
using System.Runtime.CompilerServices;
using System.Text;

namespace web_crawler
{
	internal class Program
	{
		public static string pagesPath = "../../../pages/";
		public static string graphFilename = "../graph.in";
		public static string timeFilename = "../times.txt";
		public static string cacheFilename = "../cache.txt";
		static void Main(string[] args)
		{
			if (!Directory.Exists(pagesPath))
			{
				Directory.CreateDirectory(pagesPath);
			}
			if (File.Exists(pagesPath + cacheFilename))
			{
				LoadCache(Crawler.destinationCache);
			}

			int threadCount = 1;
			try { threadCount = Int32.Parse(args[0]); } catch { threadCount = 12; }
			Console.WriteLine($"Running with {threadCount} threads.");

			Console.WriteLine("Please provide a starting URL.");
			//var origin = "https://pg.edu.pl/";
			var origin = "https://www.u-tokyo.ac.jp/";
			//var origin = Console.ReadLine();

			var watch = System.Diagnostics.Stopwatch.StartNew();

			var robotsFileParser = new RobotsFileParser();
			Task<RobotsFile> robotsFileTask = robotsFileParser.FromUriAsync(new Uri($"{origin}/robots.txt"));
			var robotsFile = robotsFileTask.Result;

			Crawler[] crawlers = new Crawler[threadCount];
			crawlers[0] = new Crawler(origin, robotsFile);

			var document = crawlers[0].Web.TryLoad(origin);

			Crawler.urlQueue.Enqueue(new KeyValuePair<string, int>(origin, 0));

			// run this a few times so that there are actual links in the queue

			while (Crawler.urlQueue.Count < threadCount * 2 && Crawler.urlQueue.Count > 0) crawlers[0].CrawlNext();

			//threadCount = 1; // uncomment this to simulate a single-threaded crawler
			Task[] tasks = new Task[threadCount - 1];
			for (int i = 1; i < threadCount; ++i)
			{
				var crawler = crawlers[i] = new Crawler(origin, robotsFile);
				tasks[i - 1] = Task.Factory.StartNew(() =>  crawler.CrawlLoop(), TaskCreationOptions.LongRunning);
			}
			crawlers[0].CrawlLoop();

			Task.WaitAll(tasks);

			watch.Stop();
			var timeElapsed = watch.ElapsedMilliseconds;

			//Console.WriteLine(Crawler.graph);
			Thread cacheSaver = new Thread(() => {
				SaveCache(Crawler.destinationCache);
			});
			cacheSaver.Start();
			File.WriteAllText(pagesPath + graphFilename, $"{Crawler.graph.Nodes.Count}\n" + Crawler.graph.ToString());

			File.AppendAllText(pagesPath + timeFilename, $"{DateTime.Now}: Gathered {Crawler.graph.Nodes.Count} pages in {timeElapsed} ms using {threadCount} threads.\n");
			cacheSaver.Join();
		}

		private static void LoadCache(ConcurrentDictionary<string, string> cache)
		{
			const Int32 bufferSize = 512;
			using (var fileStream = File.OpenRead(pagesPath + cacheFilename))
			{
				using (var streamReader = new StreamReader(fileStream, Encoding.UTF8, true, bufferSize))
				{
					String line;
					while ((line = streamReader.ReadLine()) is not null)
					{
						var urls = line.Split(' ');
						if (urls.Length != 2) continue;
						if (urls[1] == "null") urls[1] = null;
						cache[urls[0]] = urls[1];
					}
				}
			}
		}
		private static void SaveCache(ConcurrentDictionary<string, string> cache)
		{
			const Int32 bufferSize = 512;
			using (var fileStream = File.OpenWrite(pagesPath + cacheFilename))
			{
				using (var streamWriter = new StreamWriter(fileStream, Encoding.UTF8, bufferSize))
				{
					foreach (var entry in cache)
					{
						streamWriter.WriteLine($"{entry.Key} {(entry.Value is null ? "null" : entry.Value)}");
					}
				}
			}
		}
	}
}