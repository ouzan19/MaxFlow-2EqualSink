/*
 * MaxFlow.cpp
 *
 *  Created on: Jan 21, 2014
 *      Author: ouzhan
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cfloat>
#include <utility>
#define s 0
#define c1 1
#define c2 2

using namespace std;

vector<vector<bool> > adj;
vector<vector<double> > cap;
vector<vector<double> > flow;
vector <int> sira;
int f = 0;
int size = 0;
double r,f1,f2;
int from,to;

void init() {

	ifstream file;
	int num_of_line = 0;
	file.open("hw4.inp");
	file >> size;
	f = (size += 4) - 1;
	file >> num_of_line;
	for (int i = 0; i < size; i++) {
		vector<bool> sub_adj;
		vector<double> sub_cap;
		vector<double> sub_flow;
		for (int j = 0; j < size; j++) {
			sub_adj.push_back(false);
			sub_cap.push_back(0);
			sub_flow.push_back(0);
		}
		adj.push_back(sub_adj);
		cap.push_back(sub_cap);
		flow.push_back(sub_flow);

	}
	for (int i = 0; i < num_of_line; i++) {
		int m, n;
		double c;
		file >> m;
		file >> n;
		file >> c;
		adj[m][n] = true;
		adj[n][m] = true;
		cap[m][n] = c;
		cap[n][m] = c;
		sira.push_back(m);
		sira.push_back(n);
	}

	adj[size - 1][1] = true;
	adj[1][size - 1] = true;
	cap[1][size - 1] = DBL_MAX;
	adj[size - 1][2] = true;
	adj[2][size - 1] = true;
	cap[2][size - 1] = DBL_MAX;

}

void print_flow() {
	ofstream myfile;
	  myfile.open ("hw4.out");


	int sira_size = sira.size();
	for (int i = 0; i < sira_size; i+=2) {
		if(flow[sira[i]][sira[i+1]] >= 0) myfile<< sira[i]<<" "<<sira[i+1]<<" "<<flow[sira[i]][sira[i+1]]<<endl;
		else myfile<< sira[i+1]<<" "<<sira[i]<<" "<<flow[sira[i+1]][sira[i]]<<endl;
	}
	 myfile.close();
}

vector<int> getAdj(int u, int k) {

	vector<int> E;
	for (int i = 0; i < k; i++)
		if (adj[u][i])
			E.push_back(i);
	return E;
}

double min(double x, double y) {
	return (x > y) ? y : x;
}

pair<double, vector<int> > BFS(int source, int si) {

	vector<int> P;
	vector<double> M;
	for (int i = 0; i < f + 1; i++) {
		P.push_back(-1);
	}
	P[source] = -2;
	for (int i = 0; i < f + 1; i++) {
		M.push_back(0);
	}
	M[source] = DBL_MAX;

	queue<int> Q;
	Q.push(source);
	int u = 0;
	while (!Q.empty()) {
		u = Q.front();
		Q.pop();
		vector<int> E = getAdj(u, f + 1);
		int temp_size = E.size();
		for (int i = 0; i < temp_size; i++) {
			int v = E[i];
			if (cap[u][v] - flow[u][v] > 0 and P[v] == -1) {
				P[v] = u;
				M[v] = min(M[u], cap[u][v] - flow[u][v]);
				if (v != si)
					Q.push(v);
				else return make_pair(M[si], P);
			}
		}
	}

	return make_pair(0, P);
}

double abs(double x) {
	return (x > 0) ? x : -x;
}

pair<double, vector<vector<double> > > EdmonsKarp(int source, int si,
		bool source_to_sink = true) {

	int max_flow = 0;
	while (r > 0 || source_to_sink) {
		pair<double, vector<int> > foo = BFS(source, si);

		double m = foo.first;
		if (m == 0)
			break;
		if (r > m)
			r -= m;
		else if (source_to_sink)
			r = 0;
		else {
			m = r;
			r = 0;
		}
		vector<int> P = foo.second;

		max_flow += m;
		int v = si;
		int u = 0;
		while (v != source) {
			u = P[v];
			flow[u][v] += m;
			flow[v][u] -= m;
			v = u;
		}

	}

	return make_pair(max_flow, flow);
}

double flow_c1() {
	double tempflow = 0;
	for (int i = 0; i < size - 1; i++)
		tempflow += flow[i][1];
	return tempflow;
}

double flow_c2() {
	double tempflow = 0;
	for (int i = 0; i < size - 1; i++)
		tempflow += flow[i][2];
	return tempflow;
}

bool EdmonsKarp_higher_to_source(){
	    f1 = flow_c1();
		f2 = flow_c2();

		if (f1 == f2)
			return true;
		else
			r = abs(f1 - f2) / 2;

		if (f1 > f2) {
			from = c1;
			to = c2;
		} else {
			from = c2;
			to = c1;
		}
		EdmonsKarp(from, s, false);
		r = abs(f1 - f2) / 2;
		f--;
		return false;
}

void EdmonsKarp_source_to_lower(){
	EdmonsKarp(s, to, false);
		if (r > 0)
			EdmonsKarp(from, s, false);
}

int main() {

	init();                                          // initialize flow
	EdmonsKarp(s, f);                                // apply EdmonsKarp, source to virtual sink
	bool finished = EdmonsKarp_higher_to_source();  // apply EdmonsKarp,  higher-flowed city to source
	if(finished)                                    // it returns whether two city equally flowed
		return 5;                                  // if so, return
	EdmonsKarp_source_to_lower();                   // if not, to equalize flows,
	                                                // give flow from source to lower-flowed city
	print_flow();

	return 0;
}


