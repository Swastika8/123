// 1-C: Library Management System — Merge Sort by return_deadline
// Compile: g++ -O2 -o library 1C_library_management_merge_sort.cpp
// Run:     ./library
// Input:   library_books.csv (auto-generated with 1,000,000 rows if missing)
// Format:  book_id,title,return_deadline ("YYYY-MM-DD")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct Book {
    long id;
    string title;
    string returnDeadline; // ISO date "YYYY-MM-DD" sorts correctly as string
};

// ---------- CSV I/O ----------
vector<Book> readCSV(const string& filename) {
    vector<Book> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, title, deadline;
        getline(ss, idStr, ',');
        getline(ss, title, ',');
        getline(ss, deadline, ',');
        data.push_back({stol(idStr), title, deadline});
    }
    return data;
}

void writeCSV(const string& filename, const vector<Book>& data) {
    ofstream file(filename);
    for (auto& b : data)
        file << b.id << "," << b.title << "," << b.returnDeadline << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_int_distribution<int> yr(2025, 2026), mo(1, 12), da(1, 28);
    for (int i = 1; i <= n; i++) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", yr(rng), mo(rng), da(rng));
        file << i << ",Book_Title_" << i << "," << buf << "\n";
    }
}

// ---------- Merge Sort ----------
void merge(vector<Book>& arr, int l, int m, int r) {
    vector<Book> left(arr.begin() + l, arr.begin() + m + 1);
    vector<Book> right(arr.begin() + m + 1, arr.begin() + r + 1);

    size_t i = 0, j = 0;
    int k = l;
    while (i < left.size() && j < right.size())
        arr[k++] = (left[i].returnDeadline <= right[j].returnDeadline) ? left[i++] : right[j++];
    while (i < left.size()) arr[k++] = left[i++];
    while (j < right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<Book>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

int main() {
    const string inputFile = "library_books.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample book records...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<Book> books = readCSV(inputFile);
    cout << "Loaded " << books.size() << " records.\n";

    auto start = chrono::high_resolution_clock::now();
    mergeSort(books, 0, (int)books.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Merge Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("library_books_sorted.csv", books);
    cout << "Sorted data written to library_books_sorted.csv\n";

  

    return 0;
}
