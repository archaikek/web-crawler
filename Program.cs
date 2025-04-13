using System;
using HtmlAgilityPack;

namespace WebCrawler
{
	internal class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine("Please provide a starting URL.");
			var origin = "https://example.com";
			//var origin = Console.ReadLine();

			var web = new HtmlWeb();
			var document = web.Load(origin);

			var urlQueue = new Queue<string>();
			var visitedUrls = new HashSet<string>();

			urlQueue.Enqueue(origin);

			while (urlQueue.Count > 0)
			{
				var currentUrl = urlQueue.Dequeue();
				if (visitedUrls.Contains(currentUrl)) continue;

				visitedUrls.Add(currentUrl);
				Console.WriteLine($"Crawling \'{currentUrl}\'...");

				var currentDocument = web.Load(currentUrl);
				var links = currentDocument.DocumentNode.SelectNodes("//a[@href]");
				if (links is null) continue;

				foreach (var link in links)
				{
					var url = link.GetAttributeValue("href", string.Empty);
					if (visitedUrls.Contains(url)) continue;
					urlQueue.Enqueue(url);
				}
			}
		}
	}
}