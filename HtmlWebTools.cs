using HtmlAgilityPack;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace web_crawler
{
	internal static class HtmlWebTools
	{
		private static readonly int maxAttempts = 50;
		private static readonly int attemptTimeout = 200;
		public static HtmlDocument TryLoad(this HtmlWeb web, string url)
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
	}
}
