// 2-B: Online Job Portal Sorting System (Joboffer) — Quick Sort
// Sort job listings by user-selected parameter: salary, posting date, or company rating.
// Compile: g++ -O2 -std=c++17 -o joboffer 2B_job_portal_quicksort.cpp
// Run:     ./joboffer
// Input:   jobs.csv (auto-generated with 1,000,000 rows if missing)
// Format:  job_id,title,company,salary,posting_date,company_rating

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

struct Job {
    long id;
    string title;
    string company;
    double salary;
    string postingDate; // ISO "YYYY-MM-DD"
    double companyRating;
};

// ---------- CSV I/O ----------
vector<Job> readCSV(const string& filename) {
    vector<Job> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, title, company, salaryStr, date, ratingStr;
        getline(ss, idStr, ',');
        getline(ss, title, ',');
        getline(ss, company, ',');
        getline(ss, salaryStr, ',');
        getline(ss, date, ',');
        getline(ss, ratingStr, ',');
        data.push_back({stol(idStr), title, company, stod(salaryStr), date, stod(ratingStr)});
    }
    return data;
}

void writeCSV(const string& filename, const vector<Job>& data) {
    ofstream file(filename);
    for (auto& j : data)
        file << j.id << "," << j.title << "," << j.company << "," << j.salary << ","
             << j.postingDate << "," << j.companyRating << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_real_distribution<double> salary(300000, 4000000); // annual, in local currency units
    uniform_real_distribution<double> rating(1.0, 5.0);
    uniform_int_distribution<int> yr(2025, 2026), mo(1, 12), da(1, 28);
    for (int i = 1; i <= n; i++) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", yr(rng), mo(rng), da(rng));
        file << i << ",Job_Title_" << i << ",Company_" << (i % 20000) << ","
             << salary(rng) << "," << buf << "," << rating(rng) << "\n";
    }
}

// ---------- Quick Sort (randomized pivot, tail-call optimized) ----------
// field: 1 = salary, 2 = posting_date, 3 = company_rating
// postingDate compares as string (ISO format); salary/rating compare numerically
struct KeyView {
    double numeric;
    const string* text;
};

KeyView getKey(const Job& j, int field) {
    if (field == 1) return {j.salary, nullptr};
    if (field == 3) return {j.companyRating, nullptr};
    return {0.0, &j.postingDate};
}

bool lessOrEqual(const Job& a, const Job& b, int field, bool ascending) {
    if (field == 2) {
        bool cmp = a.postingDate <= b.postingDate;
        return ascending ? cmp : !cmp || a.postingDate == b.postingDate;
    }
    double ka = getKey(a, field).numeric, kb = getKey(b, field).numeric;
    return ascending ? (ka <= kb) : (ka >= kb);
}

int partition(vector<Job>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    uniform_int_distribution<int> dist(low, high);
    swap(arr[dist(rng)], arr[high]);
    Job pivot = arr[high];
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

void quickSort(vector<Job>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
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
    const string inputFile = "jobs.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample job listings...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<Job> jobs = readCSV(inputFile);
    cout << "Loaded " << jobs.size() << " records.\n";

    cout << "\nSort by: 1) Salary  2) Posting Date  3) Company Rating\nEnter choice: ";
    int field; cin >> field;
    cout << "Order: 1) Ascending  2) Descending\nEnter choice: ";
    int order; cin >> order;
    bool ascending = (order == 1);

    mt19937 rng(random_device{}());
    auto start = chrono::high_resolution_clock::now();
    quickSort(jobs, 0, (int)jobs.size() - 1, field, ascending, rng);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Quick Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("jobs_sorted.csv", jobs);
    cout << "Sorted data written to jobs_sorted.csv\n";

    return 0;
}
