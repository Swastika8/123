// 4-A: Campus Wi-Fi Network Optimization — Dijkstra's Algorithm
// Finds minimum-latency paths from main server (S) to department nodes,
// supports dynamic congestion updates, and exports a Graphviz .dot file
// for visualizing the optimal routes.
//
// Compile: g++ -O2 -std=c++17 -o campus_wifi 4A_campus_wifi_dijkstra.cpp
// Run:     ./campus_wifi   (interactive manual input, no files)
// Visualize output: dot -Tpng campus_routes.dot -o campus_routes.png
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
    double weight; // latency in ms
};

int n; // number of routers/access points (nodes numbered 1..n)
vector<Edge> edges;
vector<vector<int>> adj; // adjacency: node -> list of edge indices
bool directed;

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
        if (d > dist_[u]) continue; // stale entry, skip

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
        cout << "  No route to node " << dest << " (unreachable).\n";
        return;
    }
    vector<int> path = reconstructPath(dest);
    cout << "  Route: ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n  Total latency: " << dist_[dest] << " ms\n";
}

// exports a graphviz DOT file highlighting the shortest path to `highlightDest`
void exportDOT(const string& filename, int source, int highlightDest) {
    set<pair<int,int>> pathEdges;
    if (highlightDest != -1 && dist_[highlightDest] != INF) {
        vector<int> path = reconstructPath(highlightDest);
        for (size_t i = 0; i + 1 < path.size(); i++) {
            pathEdges.insert({min(path[i], path[i+1]), max(path[i], path[i+1])});
        }
    }

    ofstream f(filename);
    f << (directed ? "digraph" : "graph") << " CampusWiFi {\n";
    f << "  node [shape=circle, style=filled, fillcolor=lightgray];\n";
    f << "  " << source << " [fillcolor=gold, label=\"" << source << "\\n(Server)\"];\n";
    string linkOp = directed ? " -> " : " -- ";
    for (auto& e : edges) {
        bool onPath = pathEdges.count({min(e.u, e.v), max(e.u, e.v)}) > 0;
        f << "  " << e.u << linkOp << e.v
          << " [label=\"" << e.weight << "ms\""
          << (onPath ? ", color=red, penwidth=3" : ", color=gray") << "];\n";
    }
    f << "}\n";
    f.close();
    cout << "  Visualization exported to " << filename
         << " (render with: dot -Tpng " << filename << " -o routes.png)\n";
}

int main() {
    cout << "=== Campus Wi-Fi Network Optimization (Dijkstra) ===\n";
    cout << "Number of routers/access points (nodes, numbered 1..n): ";
    cin >> n;
    cout << "Is the network directed? (1 = yes, 0 = no, typically 0 for cables): ";
    int d; cin >> d;
    directed = (d == 1);

    int m;
    cout << "Number of cable connections (edges): ";
    cin >> m;
    for (int i = 0; i < m; i++) {
        int u, v; double w;
        cout << "  Edge " << (i + 1) << " -- from node, to node, latency(ms): ";
        cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    buildAdjacency();

    int source;
    cout << "Enter main server node S: ";
    cin >> source;

    dijkstra(source);

    cout << "\n--- Shortest Latency Paths from Server " << source << " ---\n";
    for (int i = 1; i <= n; i++) {
        if (i == source) continue;
        cout << "To node " << i << ":\n";
        printPath(i);
    }

    // interactive queries + dynamic congestion updates
    while (true) {
        cout << "\nOptions: [1] Query path to a node  [2] Simulate congestion (update edge latency)"
                "  [3] Export visualization  [0] Exit\nChoice: ";
        int choice; cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            int dest;
            cout << "Enter destination department node: ";
            cin >> dest;
            printPath(dest);
        } else if (choice == 2) {
            int u, v; double w;
            cout << "Enter edge to update (from, to, new latency ms): ";
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
                cout << "  Edge not found.\n";
                continue;
            }
            cout << "  Congestion applied. Recomputing shortest paths...\n";
            dijkstra(source); // full recompute; O((V+E) log V), fast enough for periodic congestion events
            cout << "  Paths updated.\n";
        } else if (choice == 3) {
            int dest;
            cout << "Highlight route to which node (-1 for none): ";
            cin >> dest;
            exportDOT("campus_routes.dot", source, dest);
        }
    }

    return 0;
}
