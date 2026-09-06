// 2-A: E-Commerce Product Catalog Optimization (Shoppy) — Quick Sort
// Sort products by user-selected parameter: price, rating, or sales volume.
// Compile: g++ -O2 -std=c++17 -o shoppy 2A_ecommerce_catalog_quicksort.cpp
// Run:     ./shoppy
// Input:   products.csv (auto-generated with 1,000,000 rows if missing)
// Format:  product_id,name,price,rating,sales_volume

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

struct Product {
    long id;
    string name;
    double price;
    double rating;
    long salesVolume;
};

// ---------- CSV I/O ----------
vector<Product> readCSV(const string& filename) {
    vector<Product> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, priceStr, ratingStr, salesStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        getline(ss, ratingStr, ',');
        getline(ss, salesStr, ',');
        data.push_back({stol(idStr), name, stod(priceStr), stod(ratingStr), stol(salesStr)});
    }
    return data;
}

void writeCSV(const string& filename, const vector<Product>& data) {
    ofstream file(filename);
    for (auto& p : data)
        file << p.id << "," << p.name << "," << p.price << "," << p.rating << "," << p.salesVolume << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_real_distribution<double> price(5.0, 20000.0);
    uniform_real_distribution<double> rating(1.0, 5.0);
    uniform_int_distribution<long> sales(0, 500000);
    for (int i = 1; i <= n; i++)
        file << i << ",Product_" << i << "," << price(rng) << "," << rating(rng) << "," << sales(rng) << "\n";
}

// ---------- Quick Sort (randomized pivot, tail-call optimized) ----------
// key selector: 1 = price, 2 = rating, 3 = sales volume
double getKey(const Product& p, int field) {
    if (field == 1) return p.price;
    if (field == 2) return p.rating;
    return (double)p.salesVolume;
}

int partition(vector<Product>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    uniform_int_distribution<int> dist(low, high);
    int randIdx = dist(rng);
    swap(arr[randIdx], arr[high]); // move random pivot to end
    double pivot = getKey(arr[high], field);
    int i = low - 1;
    for (int j = low; j < high; j++) {
        bool cond = ascending ? (getKey(arr[j], field) <= pivot) : (getKey(arr[j], field) >= pivot);
        if (cond) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<Product>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    // recurse on smaller side, loop on larger side -> bounds stack depth to O(log n)
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
    const string inputFile = "products.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample products...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<Product> products = readCSV(inputFile);
    cout << "Loaded " << products.size() << " records.\n";

    cout << "\nSort by: 1) Price  2) Rating  3) Sales Volume\nEnter choice: ";
    int field; cin >> field;
    cout << "Order: 1) Ascending  2) Descending\nEnter choice: ";
    int order; cin >> order;
    bool ascending = (order == 1);

    mt19937 rng(random_device{}());
    auto start = chrono::high_resolution_clock::now();
    quickSort(products, 0, (int)products.size() - 1, field, ascending, rng);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Quick Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("products_sorted.csv", products);
    cout << "Sorted data written to products_sorted.csv\n";

    return 0;
}
