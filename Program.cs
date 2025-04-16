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
		static void Main(string[] args)
		{
			int threadCount = Int32.Parse(args[0]);
			Console.WriteLine($"Running with {threadCount} threads.");

			Console.WriteLine("Please provide a starting URL.");
			var origin = "https://pg.edu.pl";
			//var origin = Console.ReadLine();

			var robotsFileParser = new RobotsFileParser();
			Task<RobotsFile> robotsFileTask = robotsFileParser.FromUriAsync(new Uri($"{origin}/robots.txt"));
			var robotsFile = robotsFileTask.Result;

			var web = new HtmlWeb();
			var document = web.TryLoad(origin);

			Crawler.origin = origin;
			Crawler.robotsFile = robotsFile;
			Crawler.web = web;

			Crawler.urlQueue.Enqueue(new KeyValuePair<string, int>(origin, 0));

			// run this once so that there are actual links in the queue
			Crawler[] crawlers = new Crawler[threadCount];
			crawlers[0] = new Crawler();
			crawlers[0].CrawlNext();

			//threadCount = 1; // uncomment this to simulate a single-threaded crawler
			Task[] tasks = new Task[threadCount - 1];
			for (int i = 1; i < threadCount; ++i)
			{
				var crawler = crawlers[i] = new Crawler();
				tasks[i - 1] = Task.Run(() => crawler.CrawlLoop());
			}
			crawlers[0].CrawlLoop();
			Task.WaitAll(tasks);

			//Console.WriteLine(Crawler.graph);

			if (!Directory.Exists(pagesPath))
			{
				Directory.CreateDirectory(pagesPath);
			}
			File.WriteAllText(pagesPath + graphFilename, $"{Crawler.graph.Nodes.Count}\n" + Crawler.graph.ToString());
		}
	}
}