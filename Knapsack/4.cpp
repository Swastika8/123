// 3-D: Network Bandwidth Allocation — Fractional Knapsack (with divisible/indivisible services)
// Compile: g++ -O2 -std=c++17 -o bandwidth_alloc 3D_network_bandwidth_allocation.cpp
// Run:     ./bandwidth_alloc   (interactive manual input, no files)

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Application {
    string name;
    double wi;         // bandwidth required, Mbps
    double vi;         // utility value / importance
    bool divisible;    // true = can be partially allocated
    double ratio;       // vi / wi (utility-to-bandwidth ratio)
    double allocated = 0;      // Mbps actually allocated
    double utilityEarned = 0;  // utility actually earned
};

int main() {
    double W;
    int n;

    cout << "=== Network Bandwidth Allocation (Fractional Knapsack) ===\n";
    cout << "Enter total available bandwidth W (Mbps): ";
    cin >> W;
    cout << "Enter number of applications: ";
    cin >> n;

    vector<Application> apps(n);
    for (int i = 0; i < n; i++) {
        cout << "\nApplication " << (i + 1) << ":\n";
        cout << "  Name (no spaces): ";
        cin >> apps[i].name;
        cout << "  Bandwidth requirement wi (Mbps): ";
        cin >> apps[i].wi;
        cout << "  Utility value vi: ";
        cin >> apps[i].vi;
        cout << "  Type (1 = Divisible, 0 = Indivisible): ";
        int t; cin >> t;
        apps[i].divisible = (t == 1);
        apps[i].ratio = apps[i].vi / apps[i].wi;
    }

    // Greedy: prioritize highest utility-to-bandwidth ratio
    sort(apps.begin(), apps.end(), [](const Application& a, const Application& b) {
        return a.ratio > b.ratio;
    });

    double remaining = W;
    double totalUtility = 0;

    for (auto& app : apps) {
        if (remaining <= 0) break;

        if (app.wi <= remaining) {
            app.allocated = app.wi;
            app.utilityEarned = app.vi;
            remaining -= app.wi;
        } else if (app.divisible) {
            double fraction = remaining / app.wi;
            app.allocated = remaining;
            app.utilityEarned = app.vi * fraction;
            remaining = 0;
        } else {
            // indivisible service (e.g. video call, gaming session) doesn't fit -- skip
            app.allocated = 0;
            app.utilityEarned = 0;
        }
        totalUtility += app.utilityEarned;
    }

    cout << "\n--- Allocation Result (sorted by utility/bandwidth ratio) ---\n";
    cout << left << setw(14) << "Application" << setw(10) << "BW(wi)" << setw(10) << "Util(vi)"
         << setw(8) << "Ratio" << setw(14) << "Allocated" << setw(10) << "Earned" << "\n";
    for (auto& app : apps) {
        cout << left << setw(14) << app.name << setw(10) << app.wi << setw(10) << app.vi
             << setw(8) << fixed << setprecision(2) << app.ratio
             << setw(14) << app.allocated << setw(10) << app.utilityEarned << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nTotal bandwidth allocated: " << (W - remaining) << " / " << W << " Mbps\n";
    cout << "Total utility achieved:    " << totalUtility << "\n";


    return 0;
}
