// 3-B: Cargo Loading in Airline Industry — Fractional Knapsack (with divisible/indivisible items)
// Compile: g++ -O2 -std=c++17 -o cargo_load 3B_cargo_loading.cpp
// Run:     ./cargo_load   (interactive manual input, no files)

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Cargo {
    string name;
    double wi;         // weight in kg
    double vi;         // profit from transporting it
    bool divisible;    // true = can be loaded partially
    double ratio;       // vi / wi
    double allocated = 0;      // weight actually loaded
    double revenueEarned = 0;  // profit actually earned
};

int main() {
    double W;
    int n;

    cout << "=== Cargo Loading Optimization (Fractional Knapsack) ===\n";
    cout << "Enter plane's maximum weight capacity W (kg): ";
    cin >> W;
    cout << "Enter number of cargo items: ";
    cin >> n;

    vector<Cargo> items(n);
    for (int i = 0; i < n; i++) {
        cout << "\nCargo Item " << (i + 1) << ":\n";
        cout << "  Name (no spaces): ";
        cin >> items[i].name;
        cout << "  Weight wi (kg): ";
        cin >> items[i].wi;
        cout << "  Profit vi: ";
        cin >> items[i].vi;
        cout << "  Type (1 = Divisible, 0 = Indivisible): ";
        int t; cin >> t;
        items[i].divisible = (t == 1);
        items[i].ratio = items[i].vi / items[i].wi;
    }

    // Greedy: prioritize highest profit-to-weight ratio
    sort(items.begin(), items.end(), [](const Cargo& a, const Cargo& b) {
        return a.ratio > b.ratio;
    });

    double remaining = W;
    double totalRevenue = 0;

    for (auto& c : items) {
        if (remaining <= 0) break;

        if (c.wi <= remaining) {
            c.allocated = c.wi;
            c.revenueEarned = c.vi;
            remaining -= c.wi;
        } else if (c.divisible) {
            double fraction = remaining / c.wi;
            c.allocated = remaining;
            c.revenueEarned = c.vi * fraction;
            remaining = 0;
        } else {
            // indivisible item too heavy for remaining space -- skip, keep checking others
            c.allocated = 0;
            c.revenueEarned = 0;
        }
        totalRevenue += c.revenueEarned;
    }

    cout << "\n--- Loading Result (sorted by profit/weight ratio) ---\n";
    cout << left << setw(12) << "Item" << setw(10) << "Wt(wi)" << setw(10) << "Profit(vi)"
         << setw(8) << "Ratio" << setw(12) << "Loaded(kg)" << setw(10) << "Earned" << "\n";
    for (auto& c : items) {
        cout << left << setw(12) << c.name << setw(10) << c.wi << setw(10) << c.vi
             << setw(8) << fixed << setprecision(2) << c.ratio
             << setw(12) << c.allocated << setw(10) << c.revenueEarned << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nTotal weight loaded: " << (W - remaining) << " / " << W << " kg\n";
    cout << "Total revenue earned: " << totalRevenue << "\n";

    cout << "\n--- Complexity Analysis ---\n";
    cout << "Sorting by ratio: O(n log n). Greedy loading pass: O(n). Overall: O(n log n).\n";
    cout << "Space: O(n) for storing cargo items.\n";
    cout << "IMPORTANT CAVEAT: pure Fractional Knapsack is optimal only when every item\n";
    cout << "can be split. Mixing in indivisible cargo (machinery, cars) turns this into a\n";
    cout << "hybrid Fractional/0-1 Knapsack problem -- NP-hard in the general case. The\n";
    cout << "greedy-by-ratio approach here is the standard fast heuristic and performs well\n";
    cout << "in practice, but a guaranteed optimal loading plan when heavy indivisible items\n";
    cout << "are involved would require 0/1 Knapsack dynamic programming, O(n * W).\n";

    return 0;
}
