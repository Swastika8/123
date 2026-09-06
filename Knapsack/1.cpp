// 3-A: Cloud Resource Allocation — Fractional Knapsack (with divisible/indivisible tasks)
// Compile: g++ -O2 -std=c++17 -o cloud_alloc 3A_cloud_resource_allocation.cpp
// Run:     ./cloud_alloc   (interactive manual input, no files)

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Task {
    string name;
    double wi;         // computing units required
    double vi;         // revenue if executed
    bool divisible;    // true = can be partially executed
    double ratio;       // vi / wi
    double allocated = 0;      // units actually allocated
    double revenueEarned = 0;  // revenue actually earned
};

int main() {
    double W;
    int n;

    cout << "=== Cloud Resource Allocation (Fractional Knapsack) ===\n";
    cout << "Enter total available computing power (W units): ";
    cin >> W;
    cout << "Enter number of tasks: ";
    cin >> n;

    vector<Task> tasks(n);
    for (int i = 0; i < n; i++) {
        cout << "\nTask " << (i + 1) << ":\n";
        cout << "  Name (no spaces): ";
        cin >> tasks[i].name;
        cout << "  Computational requirement wi (units): ";
        cin >> tasks[i].wi;
        cout << "  Revenue vi: ";
        cin >> tasks[i].vi;
        cout << "  Type (1 = Divisible, 0 = Indivisible): ";
        int t; cin >> t;
        tasks[i].divisible = (t == 1);
        tasks[i].ratio = tasks[i].vi / tasks[i].wi;
    }

    // Greedy: prioritize highest revenue-to-resource ratio
    sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.ratio > b.ratio;
    });

    double remaining = W;
    double totalRevenue = 0;

    for (auto& t : tasks) {
        if (remaining <= 0) break; // no capacity left, remaining tasks get 0

        if (t.wi <= remaining) {
            // fits fully -- execute completely regardless of divisibility
            t.allocated = t.wi;
            t.revenueEarned = t.vi;
            remaining -= t.wi;
        } else if (t.divisible) {
            // doesn't fit fully but CAN be split -- take the fraction that fits
            double fraction = remaining / t.wi;
            t.allocated = remaining;
            t.revenueEarned = t.vi * fraction;
            remaining = 0;
        } else {
            // indivisible and doesn't fit -- must skip entirely, move on
            // (a smaller lower-ratio task later might still fit the remaining space)
            t.allocated = 0;
            t.revenueEarned = 0;
        }
        totalRevenue += t.revenueEarned;
    }

    cout << "\n--- Allocation Result (sorted by revenue/resource ratio) ---\n";
    cout << left << setw(12) << "Task" << setw(10) << "Req(wi)" << setw(10) << "Rev(vi)"
         << setw(8) << "Ratio" << setw(12) << "Allocated" << setw(10) << "Earned" << "\n";
    for (auto& t : tasks) {
        cout << left << setw(12) << t.name << setw(10) << t.wi << setw(10) << t.vi
             << setw(8) << fixed << setprecision(2) << t.ratio
             << setw(12) << t.allocated << setw(10) << t.revenueEarned << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nTotal computing power used: " << (W - remaining) << " / " << W << " units\n";
    cout << "Total revenue generated:    " << totalRevenue << "\n";

    return 0;
}
