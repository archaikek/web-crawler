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

			var links = document.DocumentNode.SelectNodes("//a[@href]");
			foreach (var link in links)
			{
				var url = link.GetAttributeValue("href", string.Empty);
				Console.WriteLine($"Found URL: {url}");
			}
		}
	}
}