using System;
using HtmlAgilityPack;

namespace WebCrawler
{
	internal class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine("Please provide a starting URL.");
			var origin = Console.ReadLine();

			var web = new HtmlWeb();
			var document = web.Load(origin);
			
			Console.WriteLine("Page loaded successfully");
		}
	}
}