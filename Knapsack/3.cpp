// 3-C: Advertisement Slot Allocation — Fractional Knapsack (with divisible/indivisible ads)
// Compile: g++ -O2 -std=c++17 -o ad_alloc 3C_ad_slot_allocation.cpp
// Run:     ./ad_alloc   (interactive manual input, no files)

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Ad {
    string name;
    double wi;         // time required, minutes
    double vi;         // revenue offered
    bool divisible;    // true = can be shown partially
    double ratio;       // vi / wi (revenue-to-time ratio)
    double allocated = 0;      // minutes actually shown
    double revenueEarned = 0;  // revenue actually earned
};

int main() {
    double W;
    int n;

    cout << "=== Advertisement Slot Allocation (Fractional Knapsack) ===\n";
    cout << "Enter total ad time available W (minutes): ";
    cin >> W;
    cout << "Enter number of ad requests: ";
    cin >> n;

    vector<Ad> ads(n);
    for (int i = 0; i < n; i++) {
        cout << "\nAd Request " << (i + 1) << ":\n";
        cout << "  Name (no spaces): ";
        cin >> ads[i].name;
        cout << "  Time requirement wi (minutes): ";
        cin >> ads[i].wi;
        cout << "  Revenue vi: ";
        cin >> ads[i].vi;
        cout << "  Type (1 = Divisible, 0 = Indivisible): ";
        int t; cin >> t;
        ads[i].divisible = (t == 1);
        ads[i].ratio = ads[i].vi / ads[i].wi;
    }

    // Greedy: prioritize highest revenue-to-time ratio
    sort(ads.begin(), ads.end(), [](const Ad& a, const Ad& b) {
        return a.ratio > b.ratio;
    });

    double remaining = W;
    double totalRevenue = 0;

    for (auto& a : ads) {
        if (remaining <= 0) break;

        if (a.wi <= remaining) {
            a.allocated = a.wi;
            a.revenueEarned = a.vi;
            remaining -= a.wi;
        } else if (a.divisible) {
            double fraction = remaining / a.wi;
            a.allocated = remaining;
            a.revenueEarned = a.vi * fraction;
            remaining = 0;
        } else {
            // indivisible ad doesn't fit in remaining slot time -- skip, keep checking others
            a.allocated = 0;
            a.revenueEarned = 0;
        }
        totalRevenue += a.revenueEarned;
    }

    cout << "\n--- Allocation Result (sorted by revenue/time ratio) ---\n";
    cout << left << setw(12) << "Ad" << setw(10) << "Time(wi)" << setw(10) << "Rev(vi)"
         << setw(8) << "Ratio" << setw(14) << "Shown(min)" << setw(10) << "Earned" << "\n";
    for (auto& a : ads) {
        cout << left << setw(12) << a.name << setw(10) << a.wi << setw(10) << a.vi
             << setw(8) << fixed << setprecision(2) << a.ratio
             << setw(14) << a.allocated << setw(10) << a.revenueEarned << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nTotal ad time used:    " << (W - remaining) << " / " << W << " minutes\n";
    cout << "Total revenue generated: " << totalRevenue << "\n";


    return 0;
}
