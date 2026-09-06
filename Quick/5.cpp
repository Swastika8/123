// 2-E: Online Travel Booking Optimization (Airgo) — Quick Sort
// Sort travel schedules by user-selected parameter: departure time, price, or duration.
// Compile: g++ -O2 -std=c++17 -o airgo 2E_travel_booking_quicksort.cpp
// Run:     ./airgo
// Input:   travel_schedules.csv (auto-generated with 1,000,000 rows if missing)
// Format:  schedule_id,mode,departure_time,price,duration_minutes

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

struct TravelSchedule {
    long id;
    string mode; // "Flight" or "Train"
    string departureTime; // ISO "YYYY-MM-DD HH:MM:SS"
    double price;
    int durationMinutes;
};

// ---------- CSV I/O ----------
vector<TravelSchedule> readCSV(const string& filename) {
    vector<TravelSchedule> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, mode, dep, priceStr, durStr;
        getline(ss, idStr, ',');
        getline(ss, mode, ',');
        getline(ss, dep, ',');
        getline(ss, priceStr, ',');
        getline(ss, durStr, ',');
        data.push_back({stol(idStr), mode, dep, stod(priceStr), stoi(durStr)});
    }
    return data;
}

void writeCSV(const string& filename, const vector<TravelSchedule>& data) {
    ofstream file(filename);
    for (auto& t : data)
        file << t.id << "," << t.mode << "," << t.departureTime << "," << t.price << "," << t.durationMinutes << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_int_distribution<int> yr(2026, 2027), mo(1, 12), da(1, 28), hr(0, 23), mi(0, 59);
    uniform_real_distribution<double> price(500.0, 25000.0);
    uniform_int_distribution<int> dur(30, 1440);
    for (int i = 1; i <= n; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:00", yr(rng), mo(rng), da(rng), hr(rng), mi(rng));
        string mode = (i % 2 == 0) ? "Flight" : "Train";
        file << i << "," << mode << "," << buf << "," << price(rng) << "," << dur(rng) << "\n";
    }
}

// ---------- Quick Sort (randomized pivot, tail-call optimized) ----------
// field: 1 = departure_time, 2 = price, 3 = duration
bool lessOrEqual(const TravelSchedule& a, const TravelSchedule& b, int field, bool ascending) {
    if (field == 1) {
        bool cmp = a.departureTime <= b.departureTime;
        return ascending ? cmp : (a.departureTime >= b.departureTime);
    }
    double ka = (field == 2) ? a.price : a.durationMinutes;
    double kb = (field == 2) ? b.price : b.durationMinutes;
    return ascending ? (ka <= kb) : (ka >= kb);
}

int partition(vector<TravelSchedule>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    uniform_int_distribution<int> dist(low, high);
    swap(arr[dist(rng)], arr[high]);
    TravelSchedule pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (lessOrEqual(arr[j], pivot, field, ascending)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<TravelSchedule>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    while (low < high) {
        int pi = partition(arr, low, high, field, ascending, rng);
        if (pi - low < high - pi) {
            quickSort(arr, low, pi - 1, field, ascending, rng);
            low = pi + 1;
        } else {
            quickSort(arr, pi + 1, high, field, ascending, rng);
            high = pi - 1;
        }
    }
}

int main() {
    const string inputFile = "travel_schedules.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample travel schedules...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<TravelSchedule> schedules = readCSV(inputFile);
    cout << "Loaded " << schedules.size() << " records.\n";

    cout << "\nSort by: 1) Departure Time  2) Price  3) Duration\nEnter choice: ";
    int field; cin >> field;
    cout << "Order: 1) Ascending  2) Descending\nEnter choice: ";
    int order; cin >> order;
    bool ascending = (order == 1);

    mt19937 rng(random_device{}());
    auto start = chrono::high_resolution_clock::now();
    quickSort(schedules, 0, (int)schedules.size() - 1, field, ascending, rng);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Quick Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("travel_schedules_sorted.csv", schedules);
    cout << "Sorted data written to travel_schedules_sorted.csv\n";

   

    return 0;
}
