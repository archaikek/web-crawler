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
		public static string pagesPath = "../../../pages/";
		public static string graphFilename = "graph.in";
		public static string timeFilename = "times.txt";
		static void Main(string[] args)
		{
			if (!Directory.Exists(pagesPath))
			{
				Directory.CreateDirectory(pagesPath);
			}

			int threadCount = 1;
			try { threadCount = Int32.Parse(args[0]); } catch { threadCount = 12; }
			Console.WriteLine($"Running with {threadCount} threads.");

			Console.WriteLine("Please provide a starting URL.");
			//var origin = "https://pg.edu.pl/";
			var origin = "https://www.tus.ac.jp/";
			//var origin = Console.ReadLine();

			var watch = System.Diagnostics.Stopwatch.StartNew();

			var robotsFileParser = new RobotsFileParser();
			Task<RobotsFile> robotsFileTask = robotsFileParser.FromUriAsync(new Uri($"{origin}/robots.txt"));
			var robotsFile = robotsFileTask.Result;

			var web = new HtmlWeb();
			web.OverrideEncoding = System.Text.Encoding.UTF8;
			var document = web.TryLoad(origin);

			Crawler.origin = origin;
			Crawler.robotsFile = robotsFile;
			Crawler.web = web;

			Crawler.urlQueue.Enqueue(new KeyValuePair<string, int>(origin, 0));

			// run this a few times so that there are actual links in the queue
			Crawler[] crawlers = new Crawler[threadCount];
			crawlers[0] = new Crawler();
			while (Crawler.urlQueue.Count < threadCount * 2 && Crawler.urlQueue.Count > 0) crawlers[0].CrawlNext();

			//threadCount = 1; // uncomment this to simulate a single-threaded crawler
			Task[] tasks = new Task[threadCount - 1];
			for (int i = 1; i < threadCount; ++i)
			{
				var crawler = crawlers[i] = new Crawler();
				tasks[i - 1] = Task.Factory.StartNew(() =>  crawler.CrawlLoop(), TaskCreationOptions.LongRunning);
			}
			crawlers[0].CrawlLoop();

			Task.WaitAll(tasks);

			watch.Stop();
			var timeElapsed = watch.ElapsedMilliseconds;

			//Console.WriteLine(Crawler.graph);
			File.WriteAllText(pagesPath + graphFilename, $"{Crawler.graph.Nodes.Count}\n" + Crawler.graph.ToString());

			File.AppendAllText(pagesPath + timeFilename, $"{DateTime.Now}: Gathered {Crawler.graph.Nodes.Count} pages in {timeElapsed} ms using {threadCount} threads.\n");
		}
	}
}