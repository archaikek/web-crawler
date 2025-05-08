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
using System.Reflection;
using System.Security.Cryptography.X509Certificates;
using System.Web;
using Microsoft.Win32.SafeHandles;
using System.Xml.XPath;
using System.Security.Principal;
using System.Xml.Linq;

namespace web_crawler
{
	internal class CrawlerNew
	{
		private static readonly int limit = 4444;
		private static readonly int maxQueueSize = 222;
		private static int index = 0;
		private static Mutex indexMutex = new Mutex(false), foundMutex = new Mutex(false);

		public static ConcurrentQueue<QueuedRequest> remaining = new ConcurrentQueue<QueuedRequest>();
		public static ConcurrentQueue<QueuedNode> foundNodes = new ConcurrentQueue<QueuedNode>();
		public static HashSet<string> found = new HashSet<string>();
		public static Graph graph = new Graph(limit);

		private string origin { get; set; } = string.Empty;
		private RobotsFile robotsFile { get; set; } = null;
		private HtmlWeb web { get; set; } = null;
		public CrawlerNew(string _origin, RobotsFile _robotsFile)
		{
			origin = _origin;
			robotsFile = _robotsFile;
			web = new HtmlWeb();
			web.OverrideEncoding = Encoding.UTF8;
			web.PreRequest += request =>
			{
				request.CookieContainer = new System.Net.CookieContainer();
				return true;
			};
		}

		public void CrawlLoop()
		{
			while (index < limit || remaining.Count > 0)
			{
				CrawlNext();
			}
			Console.WriteLine($"Thread {Thread.CurrentThread.ManagedThreadId} starts making connections.");
			while (foundNodes.Count > 0)
			{
				QueuedNode node;
				if (!foundNodes.TryDequeue(out node))
				{
					Thread.Sleep(50);
					continue;
				}
				MakeConnections(node);
			}
			Console.WriteLine($"Thread {Thread.CurrentThread.ManagedThreadId} finished work.");
		}
		public void MakeConnections(QueuedNode node)
		{
			var urls = node.urls;
			foreach (string _url in urls)
			{
				string url = _url;
				if (string.IsNullOrEmpty(url)) continue; // bad link

				url = NormalizeUrl(url);
				if (string.IsNullOrEmpty(url)) continue; // link leading outside the domain

				if (graph.Nodes.ContainsKey(url))
				{
					graph[node.id].Add(graph.Nodes[url]);
				}
			}
		}
		public void CrawlNext()
		{
			QueuedRequest request;
			if (!remaining.TryDequeue(out request))
			{
				Thread.Sleep(50);
				return;
			}
			if (index >= limit)
			{
				remaining.Clear();
				return;
			}

			HtmlDocument document;
			try { document = request.document.Result; }
			catch (Exception e) { request.document.Dispose(); return; }

			request.document.Dispose();

			if (document is null) return;

			var links = document.DocumentNode.SelectNodes("//a[@href]");
			if (links is null) return;

			var urls = links.Select(link => link.GetAttributeValue("href", String.Empty)).Distinct().ToArray();
			if (urls.Length <= 4) return; // won't be able to reasonably hold all neighbours

			int currentIndex = GetNextIndex();
			if (currentIndex >= limit) return; // the graph is full

			graph.Nodes[request.url] = currentIndex;
			foundNodes.Enqueue(new QueuedNode(currentIndex, urls));

			Console.WriteLine($"Crawling \"{request.url}\"... [{currentIndex}]\n\tQueue size = {remaining.Count}, found size = {found.Count}");

			Thread saver = new Thread(() =>
			{
				using (FileStream fs = File.OpenWrite(MakeFileName(currentIndex, request.url)))
				{
					document.Save(fs);
				}
			});
			saver.Start();

			foreach (string _url in urls)
			{
				if (index >= limit) break;
				if (remaining.Count >= 10 + (limit - index) / 4) continue; // too many nodes to process
				string url = _url;
				if (string.IsNullOrEmpty(url)) continue; // bad link

				url = NormalizeUrl(url);
				if (string.IsNullOrEmpty(url)) continue; // link leading outside the domain
				if (IsFound(url)) continue; // link already in the queue, don't add it again
				if (!robotsFile.IsAllowedAccess(new Uri(url), "TestCrawler2221")); // robots not allowed

				remaining.Enqueue(new QueuedRequest(url, web.LoadFromWebAsync(url)));
			}

			saver.Join();
			Console.WriteLine($"Finished crawling {currentIndex}");
		}


		private string? NormalizeUrl(string url)
		{
			string result = url;
			if (result.StartsWith("http") && !result.StartsWith(origin)) return null;
			if (!result.StartsWith(origin)) result = origin + (result.StartsWith('/') ? result.Substring(1) : result);

			int index = result.IndexOf('?');
			if (index != -1) result = result.Substring(0, index);
			index = result.IndexOf('#');
			if (index != -1) result = result.Substring(0, index);

			if (!result.StartsWith(origin)) return null;

			return result;
		}
		private bool IsFound(string url)
		{
			bool result;
			foundMutex.WaitOne();
			result = found.Contains(url);
			found.Add(url);
			foundMutex.ReleaseMutex();
			return result;
		}
		private int GetNextIndex()
		{
			int result;
			indexMutex.WaitOne();
			result = index++;
			indexMutex.ReleaseMutex();
			return result;
		}
		private string MakeFileName(int index, string currentUrl)
		{
			string suffix = currentUrl.EndsWith(".html") ? "" : ".html"; // add .html to documents that don't have it
			foreach (char c in System.IO.Path.GetInvalidFileNameChars()) currentUrl = currentUrl.Replace(c, '_');
			return Program.pagesPath + $"{index}_{currentUrl}{suffix}";
		}
		public class QueuedRequest
		{
			public string url;
			public Task<HtmlDocument> document;
			public QueuedRequest(string _url, Task<HtmlDocument> _document)
			{
				url = _url;
				document = _document;
			}
		}
		public class QueuedNode
		{
			public int id = -1;
			public string[] urls = null;
			public QueuedNode(int _id, string[] _urls)
			{
				id = _id;
				urls = _urls;
			}
		}
	}
}
