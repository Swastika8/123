// 4-C: Robot Navigation in Smart Factory — Dijkstra's Algorithm
// Finds the shortest travel path for a robot from a charging station (S)
// to an assembly point (D), supports dynamic obstacle/congestion updates,
// and exports a Graphviz .dot file visualizing the optimal path.
//
// Compile: g++ -O2 -std=c++17 -o robot_nav 4C_robot_navigation.cpp
// Run:     ./robot_nav   (interactive manual input, no files)
// Visualize output: dot -Tpng robot_path.dot -o robot_path.png
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
    double weight; // travel time between junctions (accounts for obstacles/traffic)
};

int n; // number of factory-floor junctions (nodes numbered 1..n)
vector<Edge> edges;
vector<vector<int>> adj;
bool directed; // factory paths are typically bidirectional unless one-way conveyors etc.

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
        cout << "  No path found to junction " << dest << " (blocked/unreachable).\n";
        return;
    }
    vector<int> path = reconstructPath(dest);
    cout << "  Path: ";
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n  Total travel time: " << dist_[dest] << " (time units)\n";
}

void exportDOT(const string& filename, int source, int dest) {
    set<pair<int,int>> pathEdges;
    vector<int> path;
    bool hasPath = false;
    if (dest != -1 && dist_[dest] != INF) {
        path = reconstructPath(dest);
        for (size_t i = 0; i + 1 < path.size(); i++)
            pathEdges.insert({min(path[i], path[i+1]), max(path[i], path[i+1])});
        hasPath = true;
    }

    ofstream f(filename);
    f << (directed ? "digraph" : "graph") << " FactoryFloor {\n";
    f << "  node [shape=circle, style=filled, fillcolor=lightgray];\n";
    f << "  " << source << " [fillcolor=gold, label=\"" << source << "\\n(Charge)\"];\n";
    if (dest != -1) f << "  " << dest << " [fillcolor=lightgreen, label=\"" << dest << "\\n(Assembly)\"];\n";
    string linkOp = directed ? " -> " : " -- ";
    for (auto& e : edges) {
        bool onPath = hasPath && pathEdges.count({min(e.u, e.v), max(e.u, e.v)}) > 0;
        f << "  " << e.u << linkOp << e.v
          << " [label=\"" << e.weight << "\""
          << (onPath ? ", color=red, penwidth=3" : ", color=gray") << "];\n";
    }
    f << "}\n";
    f.close();
    cout << "  Visualization exported to " << filename
         << " (render with: dot -Tpng " << filename << " -o path.png)\n";
}

int main() {
    cout << "=== Robot Navigation in Smart Factory (Dijkstra) ===\n";
    cout << "Number of factory junctions (nodes, numbered 1..n): ";
    cin >> n;
    cout << "Is the path network directed? (1 = yes, 0 = no, typically 0): ";
    int d; cin >> d;
    directed = (d == 1);

    int m;
    cout << "Number of paths between junctions (edges): ";
    cin >> m;
    for (int i = 0; i < m; i++) {
        int u, v; double w;
        cout << "  Path " << (i + 1) << " -- from junction, to junction, travel time: ";
        cin >> u >> v >> w;
        edges.push_back({u, v, w});
    }
    buildAdjacency();

    int source, dest;
    cout << "Enter charging station node S (start): ";
    cin >> source;
    cout << "Enter assembly point node D (destination): ";
    cin >> dest;

    dijkstra(source);

    cout << "\n--- Shortest Path from Charging Station " << source
         << " to Assembly Point " << dest << " ---\n";
    printPath(dest);

    while (true) {
        cout << "\nOptions: [1] Query another destination  [2] Simulate obstacle/congestion"
                " (update edge)  [3] Export visualization  [0] Exit\nChoice: ";
        int choice; cin >> choice;
        if (choice == 0) break;

        if (choice == 1) {
            cout << "Enter destination junction: ";
            cin >> dest;
            printPath(dest);
        } else if (choice == 2) {
            int u, v; double w;
            cout << "Enter path to update (from, to, new travel time -- use a very\n"
                    "large number, e.g. 999999, to represent a fully blocked path): ";
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
                cout << "  Path not found.\n";
                continue;
            }
            cout << "  Obstacle/congestion applied. Recomputing route...\n";
            dijkstra(source);
            cout << "  Route updated.\n";
        } else if (choice == 3) {
            exportDOT("robot_path.dot", source, dest);
        }
    }

    return 0;
}
