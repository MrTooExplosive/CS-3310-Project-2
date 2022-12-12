#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <limits>
#include <utility>
#include <fstream>
#include <chrono>
using namespace std;

vector<vector<int>> genGraph(unsigned);
pair<vector<int>,vector<int>> dijkstra(vector<vector<int>>&,unsigned);
vector<pair<vector<int>, vector<int>>> allDijkstra(vector<vector<int>>&);
pair<vector<vector<int>>,vector<vector<int>>> floydWarshall(vector<vector<int>>&);

int main()
{
	ofstream fout("timing.csv");
	srand(time(NULL));
	vector<vector<int>> adjMat(3, vector<int>(3));
	adjMat[0][0] = 0;
	adjMat[0][1] = 51;
	adjMat[0][2] = 32;
	adjMat[1][0] = -1;
	adjMat[1][1] = 0;
	adjMat[1][2] = 11;
	adjMat[2][0] = 39;
	adjMat[2][1] = -1;
	adjMat[2][2] = 0;
	cout << "Adjacency Matrix:\n\nNodes\t";
	for (int i = 0; i < 3; i++)
		cout << i << '\t';
	cout << "\n\n";
	for (int i = 0; i < 3; i++)
	{
		cout << i << '\t';
		for (int j = 0; j < 3; j++)
			cout << adjMat[i][j] << '\t';
		cout << '\n';
	}
	cout << "\n\n\n";

	cout << "Dijkstra's Algorithm:\n";
	auto ans = allDijkstra(adjMat);
	for (int i = 0; i < 3; i++)
	{
		cout << i << ":\t";
		for (int j = 0; j < adjMat.size(); j++)
			cout << ans[i].first[j] << '\t';
		cout << '\n';
	}

	cout << "\n\nFloyd-Warshall Algorithm:\n";
	auto ans2 = floydWarshall(adjMat);
	for (int i = 0; i < adjMat.size(); i++)
	{
		cout << i << ":\t";
		for (int j = 0; j < adjMat.size(); j++)
			cout << ans2.first[i][j] << '\t';
		cout << '\n';
	}

	unsigned long long n = 10;
	while (true)
	{
		auto graph = genGraph(n);
		auto start1 = chrono::high_resolution_clock::now();
		for (int i = 0; i < 5; i++)
			allDijkstra(graph);
		auto stop1 = chrono::high_resolution_clock::now();
		auto start2 = chrono::high_resolution_clock::now();
		for (int i = 0; i < 5; i++)
			floydWarshall(graph);
		auto stop2 = chrono::high_resolution_clock::now();

		auto dur1 = chrono::duration_cast<chrono::microseconds>(stop1 - start1);
		auto dur2 = chrono::duration_cast<chrono::microseconds>(stop2 - start2);

		fout << n << ',' << dur1.count() / 5 << ',' << dur2.count() / 5 << '\n';
		fout.flush();
		cout << "Finished graph with " << n << " nodes.\n";
		n += 10;
	}
	fout.close();
	return 0;
}

pair<vector<vector<int>>,vector<vector<int>>> floydWarshall(vector<vector<int>>& adj)
{
	vector<vector<vector<int>>> D(adj.size(), vector<vector<int>>(adj.size(), vector<int>(adj.size())));
	vector<vector<vector<int>>> P(adj.size(), vector<vector<int>>(adj.size(), vector<int>(adj.size())));
	vector<vector<int>> baseP(adj.size(), vector<int>(adj.size()));
	for (int i = 0; i < adj.size(); i++)
	{
		for (int j = 0; j < adj.size(); j++)
		{
			if (i != j && adj[i][j] > 0)
				baseP[i][j] = i;
			else
				baseP[i][j] = -1;
		}
	}
	for (int u = 0; u < adj.size(); u++)
	{
		for (int v = 0; v < adj.size(); v++)
		{
			D[0][u][v] = adj[u][v];
			P[0][u][v] = baseP[u][v];
			if (adj[u][v] < 0)
			{
				if (adj[u][0] > -1 && adj[0][v] > -1)
				{
					D[0][u][v] = adj[u][0] + adj[0][v];
					P[0][u][v] = baseP[0][v];
				}
			}
			else
			{
				if (adj[u][0] > -1 && adj[0][v] > -1 && adj[u][v] > adj[u][0] + adj[0][v])
				{
					D[0][u][v] = adj[u][0] + adj[0][v];
					P[0][u][v] = baseP[0][v];
				}
			}
		}
	}
	for (int k = 1; k < adj.size(); k++)
	{
		for (int u = 0; u < adj.size(); u++)
		{
			for (int v = 0; v < adj.size(); v++)
			{
				D[k][u][v] = D[k - 1][u][v];
				P[k][u][v] = P[k - 1][u][v];
				if (D[k - 1][u][v] < 0)
				{
					if (D[k - 1][u][k] > -1 && D[k - 1][k][v] > -1)
					{
						D[k][u][v] = D[k - 1][u][k] + D[k - 1][k][v];
						P[k][u][v] = P[k - 1][k][v];
					}
				}
				else
				{
					if (D[k - 1][u][k] > -1 && D[k - 1][k][v] > -1 && D[k - 1][u][v] > D[k - 1][u][k] + D[k - 1][k][v])
					{
						D[k][u][v] = D[k - 1][u][k] + D[k - 1][k][v];
						P[k][u][v] = P[k - 1][k][v];
					}
				}
			}
		}
	}
	return make_pair(D[adj.size()-1], P[adj.size() - 1]);
}
vector<pair<vector<int>, vector<int>>> allDijkstra(vector<vector<int>>& adj)
{
	vector<pair<vector<int>, vector<int>>> ans(adj.size());
	for (int i = 0; i < adj.size(); i++)
		ans[i] = dijkstra(adj, i);
	return ans;
}
pair<vector<int>, vector<int>> dijkstra(vector<vector<int>>& adj, unsigned s)
{
	vector<int> dist(adj.size(), -1);
	vector<int> prev(adj.size(), s);
	vector<int> mark(adj.size(), 0);
	for (int i = 0; i < adj.size(); i++)
		dist[i] = adj[s][i];
	mark[s] = 1;
	for (int i = 0; i < adj.size() - 1; i++)
	{
		int min = INT_MAX;
		int minIndex = -1;
		for (int j = 0; j < adj.size(); j++)
		{
			if (mark[j])
				continue;
			if (dist[j] > 0 && dist[j] < min)
			{
				min = dist[j];
				minIndex = j;
			}
		}
		mark[minIndex] = 1;
		for (int j = 0; j < adj.size(); j++)
		{
			if (mark[j] || adj[minIndex][j] < 1)
				continue;
			if (dist[j] > dist[minIndex] + adj[minIndex][j] || (dist[j] < 1 && adj[minIndex][j] > 0))
			{
				dist[j] = dist[minIndex] + adj[minIndex][j];
				prev[j] = minIndex;
			}
		}
	}
	return make_pair(dist, prev);
}
vector<vector<int>> genGraph(unsigned n)
{
	vector<vector<int>>  adj(n, vector<int>(n));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i == j)
				adj[i][j] = 0;
			else
			{
				do
					adj[i][j] = rand() % 100 - 50;
				while (!adj[i][j]);
			}
		}
		if ((*max_element(adj[i].begin(), adj[i].end())) < 1)
		{
			int x = i;
			int val = rand() % 100 + 1;
			do
				x = rand() % n;
			while (x == i);
			adj[i][x] = val;
		}
	}
	for (int i = 0; i < n; i++)
	{
		bool available = false;
		for (int j = 0; j < n && !available; j++)
		{
			if (adj[j][i] > 0)
				available = true;
		}
		if (!available)
		{
			int x;
			int val = rand() % 100 + 1;
			do
				x = rand() % n;
			while (x == i);
			adj[x][i] = val;
		}
	}
	return adj;
}
