// 1-D: Flight Reservation System — Merge Sort by departure_time
// Compile: g++ -O2 -o flights 1D_flight_reservation_merge_sort.cpp
// Run:     ./flights
// Input:   flight_bookings.csv (auto-generated with 1,000,000 rows if missing)
// Format:  booking_id,passenger,flight_no,departure_time ("YYYY-MM-DD HH:MM:SS")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct Booking {
    long id;
    string passenger;
    string flightNo;
    string departureTime; // ISO format sorts chronologically as string
};

// ---------- CSV I/O ----------
vector<Booking> readCSV(const string& filename) {
    vector<Booking> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, passenger, flightNo, dep;
        getline(ss, idStr, ',');
        getline(ss, passenger, ',');
        getline(ss, flightNo, ',');
        getline(ss, dep, ',');
        data.push_back({stol(idStr), passenger, flightNo, dep});
    }
    return data;
}

void writeCSV(const string& filename, const vector<Booking>& data) {
    ofstream file(filename);
    for (auto& b : data)
        file << b.id << "," << b.passenger << "," << b.flightNo << "," << b.departureTime << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_int_distribution<int> yr(2026, 2027), mo(1, 12), da(1, 28), hr(0, 23), mi(0, 59);
    uniform_int_distribution<int> fno(100, 999);
    for (int i = 1; i <= n; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:00",
                 yr(rng), mo(rng), da(rng), hr(rng), mi(rng));
        file << i << ",Passenger_" << i << ",FL" << fno(rng) << "," << buf << "\n";
    }
}

// ---------- Merge Sort ----------
void merge(vector<Booking>& arr, int l, int m, int r) {
    vector<Booking> left(arr.begin() + l, arr.begin() + m + 1);
    vector<Booking> right(arr.begin() + m + 1, arr.begin() + r + 1);

    size_t i = 0, j = 0;
    int k = l;
    while (i < left.size() && j < right.size())
        arr[k++] = (left[i].departureTime <= right[j].departureTime) ? left[i++] : right[j++];
    while (i < left.size()) arr[k++] = left[i++];
    while (j < right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<Booking>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

int main() {
    const string inputFile = "flight_bookings.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample bookings...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<Booking> bookings = readCSV(inputFile);
    cout << "Loaded " << bookings.size() << " records.\n";

    auto start = chrono::high_resolution_clock::now();
    mergeSort(bookings, 0, (int)bookings.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Merge Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("flight_bookings_sorted.csv", bookings);
    cout << "Sorted data written to flight_bookings_sorted.csv\n";

    return 0;
}
