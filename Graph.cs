using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
using System.Collections.Concurrent;
using HtmlAgilityPack;
using System.Security.Cryptography.X509Certificates;

namespace web_crawler
{
	internal class Graph
	{
		private HashSet<int>[] edges { get; set; } = null;
		private ConcurrentDictionary<string, int> nodes { get; set; } = null;

		public HashSet<int>[] Edges 
		{ 
			get { return edges; }
			set { edges = value; }
		}
		public ConcurrentDictionary<string, int> Nodes
		{
			get { return nodes; }
			private set { nodes = value; }
		}

		public HashSet<int> this[int node]
		{
			get { return edges[node]; }
			set { edges[node] = value; }
		}
		public int this[int node, int index]
		{
			get { return edges[node].ElementAt(index); }
			set { edges[node].Add(value); }
		}

		public Graph() { }
		public Graph(int maxNodeCount)
		{
			edges = new HashSet<int>[maxNodeCount];
			for (int i = 0; i < maxNodeCount; ++i) edges[i] = new HashSet<int>();
			nodes = new ConcurrentDictionary<string, int>();
		}
		public Graph(HashSet<int>[] edges, ConcurrentDictionary<string, int> nodes)
		{
			Edges = edges;
			Nodes = nodes;
		}


		public override string ToString()
		{
			return EdgesToString() + NodesToString();
		}
		private string EdgesToString()
		{
			string result = "";
			for (int i = 0; i < edges.Length; ++i)
			{
				var neighbours = Edges[i];
				result += $"{i} {neighbours.Count()}\n";
				foreach (var neighbour in neighbours)
				{
					result += $"{neighbour} ";
				}
				result += "\n";
			}
			return result;
		}
		private string NodesToString()
		{
			var NodeList = Nodes.ToList();
			NodeList.Sort((a, b) => { return a.Value == b.Value ? 0 : a.Value < b.Value ? -1 : 1; });

			string result = "";
			foreach (var node in NodeList)
			{
				result += $"{node.Value} {node.Key}\n";
			}
			return result;
		}
	}
}
