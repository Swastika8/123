// 4-B: Flight Route Planner — Dijkstra's Algorithm
// Finds the shortest total travel time from Source City (S) to Destination
// City (D), supports dynamic delay updates, and exports a Graphviz .dot
// file visualizing the quickest route.
//
// Compile: g++ -O2 -std=c++17 -o flight_planner 4B_flight_route_planner.cpp
// Run:     ./flight_planner   (interactive manual input, no files)
// Visualize output: dot -Tpng flight_route.dot -o flight_route.png
//                    (requires Graphviz installed: https://graphviz.org)

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <fstream>
#include <set>
#include <algorithm>

using namespace std;
const double INF = numeric_limits<double>::infinity();

struct Edge {
    int u, v;
    double weight; // total travel time (mins), including layovers/delays
};

int n; // number of airports (nodes numbered 1..n)
vector<Edge> edges;
vector<vector<int>> adj;
bool directed; // flights are naturally directed (A->B travel time may differ from B->A)

vector<double> dist_;
vector<int> parent_;

void buildAdjacency() {
    adj.assign(n + 1, {});
    for (int i = 0; i < (int)edges.size(); i++) {
        adj[edges[i].u].push_back(i);
        if (!directed) adj[edges[i].v].push_back(i);
    }
}

void dijkstra(int source) {
    dist_.assign(n + 1, INF);
    parent_.assign(n + 1, -1);
    dist_[source] = 0;

    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<>> pq;
    pq.push({0.0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist_[u]) continue;

        for (int idx : adj[u]) {
            Edge& e = edges[idx];
            int nxt = (e.u == u) ? e.v : e.u;
            double nd = dist_[u] + e.weight;
            if (nd < dist_[nxt]) {
                dist_[nxt] = nd;
                parent_[nxt] = u;
                pq.push({nd, nxt});
            }
        }
    }
}

vector<int> reconstructPath(int dest) {
    vector<int> path;
    if (dist_[dest] == INF) return path;
    for (int cur = dest; cur != -1; cur = parent_[cur]) path.push_back(cur);
    reverse(path.begin(), path.end());
    return path;
}

void printPath(int dest) {
    if (dist_[dest] == INF) {
        cout << "  No route found to airport " << dest << ".\n";
        return;
    }
    vector<int> path = reconstructPath(dest);
    cout << "  Route: ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n  Total travel time: " << dist_[dest] << " minutes\n";
}

void exportDOT(const string& filename, int source, int dest) {
    set<pair<int,int>> pathEdges;
    bool directedPath = false;
    vector<int> path;
    if (dest != -1 && dist_[dest] != INF) {
        path = reconstructPath(dest);
        for (size_t i = 0; i + 1 < path.size(); i++)
            pathEdges.insert({path[i], path[i+1]});
        directedPath = true;
    }

    ofstream f(filename);
    f << (directed ? "digraph" : "graph") << " FlightRoutes {\n";
    f << "  node [shape=box, style=filled, fillcolor=lightgray];\n";
    f << "  " << source << " [fillcolor=gold];\n";
    if (dest != -1) f << "  " << dest << " [fillcolor=lightgreen];\n";
    string linkOp = directed ? " -> " : " -- ";
    for (auto& e : edges) {
        bool onPath = directedPath && pathEdges.count({e.u, e.v}) > 0;
        f << "  " << e.u << linkOp << e.v
          << " [label=\"" << e.weight << "min\""
          << (onPath ? ", color=red, penwidth=3" : ", color=gray") << "];\n";
    }
    f << "}\n";
    f.close();
    cout << "  Visualization exported to " << filename
         << " (render with: dot -Tpng " << filename << " -o route.png)\n";
}

int main() {
    cout << "=== Flight Route Planner (Dijkstra) ===\n";
    cout << "Number of airports (nodes, numbered 1..n): ";
    cin >> n;
    cout << "Is the flight network directed? (1 = yes -- recommended, 0 = no): ";
    int d; cin >> d;
    directed = (d == 1);

    int m;
    cout << "Number of direct flight connections (edges): ";
    cin >> m;
    for (int i = 0; i < m; i++) {
        int u, v; double w;
        cout << "  Flight " << (i + 1) << " -- from airport, to airport, travel time(min): ";
        cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    buildAdjacency();

    int source, dest;
    cout << "Enter Source City airport S: ";
    cin >> source;
    cout << "Enter Destination City airport D: ";
    cin >> dest;

    dijkstra(source);

    cout << "\n--- Shortest Travel Time from " << source << " to " << dest << " ---\n";
    printPath(dest);

    while (true) {
        cout << "\nOptions: [1] Query another destination  [2] Simulate flight delay (update edge)"
                "  [3] Export visualization  [0] Exit\nChoice: ";
        int choice; cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            cout << "Enter destination airport: ";
            cin >> dest;
            printPath(dest);
        } else if (choice == 2) {
            int u, v; double w;
            cout << "Enter flight to delay (from, to, new travel time min): ";
            cin >> u >> v >> w;
            bool found = false;
            for (auto& e : edges) {
                if ((e.u == u && e.v == v) || (!directed && e.u == v && e.v == u)) {
                    e.weight = w;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "  Flight connection not found.\n";
                continue;
            }
            cout << "  Delay applied. Recomputing shortest routes...\n";
            dijkstra(source);
            cout << "  Routes updated.\n";
        } else if (choice == 3) {
            exportDOT("flight_route.dot", source, dest);
        }
    }

    return 0;
}
