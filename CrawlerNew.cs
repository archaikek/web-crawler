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
	internal class CrawlerNew
	{
		private static LockableInt PagesFound = new LockableInt(1);
		private static LockableInt crawlerCount = new LockableInt(0);
		private static readonly int limit = 4444;

		public static Graph graph { get; set; } = new Graph(limit);
		private static ConcurrentQueue<PageInfo> pendingPages = new ConcurrentQueue<PageInfo>();
		public static ConcurrentQueue<PageInfo> PendingPages { get { return pendingPages; } }
		private static HashSet<string> checkedUrls = new HashSet<string>();

		private string origin = String.Empty;
		private RobotsFile robotsFile = null;
		private HtmlWeb web = null;
		private int id = -1;

		public CrawlerNew() 
		{
			lock (crawlerCount) { id = crawlerCount.Value++; }
		}
		public CrawlerNew(string origin, RobotsFile robotsFile)
		{
			lock (crawlerCount) { id = crawlerCount.Value++; }
			this.origin = origin;
			this.robotsFile = robotsFile;
			web = new HtmlWeb();
			web.OverrideEncoding = Encoding.UTF8;
		}

		public void CrawlLoop()
		{
			while (true)
			{
				if (PagesFound.Value >= limit) return;
				CrawlNext();
			}
		}
		public void CrawlNext()
		{
			PageInfo page;
			if (!pendingPages.TryDequeue(out page)) // the queue is empty
			{
				Thread.Sleep(2221);
				return;
			}

				var links = page.document.DocumentNode.SelectNodes("//a[@href]");
			foreach (var link in links)
			{
				/* Run a few checks before trying to load a new page */
				var url = RemoveTagsAndGetArguments(link.GetAttributeValue("href", String.Empty));
				if (checkedUrls.Contains(url)) continue; // skip if the link has already been found before
				else checkedUrls.Add(url);

				if (url.StartsWith('/')) // if the page uses relative path links
				{
					url = origin + url; // prepend origin to it
					if (checkedUrls.Contains(url)) continue;
					else checkedUrls.Add(url);
				}

				if (!url.StartsWith(origin)) continue; // the link leads outside the main domain

				/* Load the document */
				var newDocument = web.TryLoad(url);
				var targetUrl = web.ResponseUri.ToString();
				if (checkedUrls.Contains(targetUrl)) continue;
				else checkedUrls.Add(targetUrl);
				if (!targetUrl.StartsWith(origin)) continue;
				if (newDocument is null || newDocument.DocumentNode.SelectSingleNode("//html") is null) // if the found document is not an HTML
				{
					continue;
				}

				/* Add the document to the graph and the queue */
				PageInfo newPage;
				lock (PagesFound)
				{
					if (PagesFound.Value >= limit) break;
					newPage = new PageInfo(newDocument, targetUrl, PagesFound.Value++);
				}
				Console.WriteLine($"[{id}] Found page {newPage.url} [{page.index} -> {newPage.index}].");

				Thread saver = new Thread(() => {
					PageInfo temp = new PageInfo(newPage.document, newPage.url, newPage.index);
					using (FileStream fs = File.OpenWrite(MakeFileName(temp.index, temp.url)))
					{
						newDocument.Save(fs);
					}
				});
				saver.Start();

				pendingPages.Enqueue(newPage);
				graph.Nodes.TryAdd(newPage.url, newPage.index);
				graph.Edges[page.index].Add(newPage.index);

				saver.Join();
				Console.WriteLine($"[{id}]\tFinished processing page {newPage.index}.");
			}
		}


		private string RemoveTagsAndGetArguments(string url)
		{
			string result = url;
			int index = result.IndexOf('?');
			if (index != -1) result = result.Substring(0, index);
			index = result.IndexOf('#');
			if (index != -1) result = result.Substring(0, index);

			return result;
		}
		private string MakeFileName(int index, string currentUrl)
		{
			string urlSuffix = currentUrl.Substring(currentUrl.Length - 5);
			string suffix = (Regex.Matches(urlSuffix, ".").Count >= 1 && !urlSuffix.Contains(".jp")) ? "" : ".html"; // add .html to documents that don't have it
			return Program.pagesPath + $"{index}_{currentUrl.Replace(':', '-').Replace('/', '_').Replace('\\', '_')}{suffix}";
		}

		private class LockableInt
		{
			public int Value;
			public LockableInt() { Value = 0; }
			public LockableInt(int value) { Value = value; }
		}
		internal class PageInfo
		{
			public HtmlDocument document = null;
			public string url = String.Empty;
			public int index = -1;

			public PageInfo() { }
			public PageInfo(HtmlDocument document, string url, int index)
			{
				this.document = document;
				this.url = url;
				this.index = index;
			}
		}
	}
}
