// 2-C: Hospital Patient Management System — Quick Sort
// Sort patients by user-selected parameter: severity level, admission date, or age.
// Compile: g++ -O2 -std=c++17 -o hospital_qs 2C_hospital_management_quicksort.cpp
// Run:     ./hospital_qs
// Input:   hospital_patients.csv (auto-generated with 1,000,000 rows if missing)
// Format:  patient_id,name,severity_level,admission_date,age
// severity_level: 1 (critical) - 5 (routine) -- lower number = higher priority

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

struct PatientRec {
    long id;
    string name;
    int severityLevel;
    string admissionDate; // ISO "YYYY-MM-DD"
    int age;
};

// ---------- CSV I/O ----------
vector<PatientRec> readCSV(const string& filename) {
    vector<PatientRec> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, sevStr, date, ageStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, sevStr, ',');
        getline(ss, date, ',');
        getline(ss, ageStr, ',');
        data.push_back({stol(idStr), name, stoi(sevStr), date, stoi(ageStr)});
    }
    return data;
}

void writeCSV(const string& filename, const vector<PatientRec>& data) {
    ofstream file(filename);
    for (auto& p : data)
        file << p.id << "," << p.name << "," << p.severityLevel << "," << p.admissionDate << "," << p.age << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_int_distribution<int> sev(1, 5), age(0, 95);
    uniform_int_distribution<int> yr(2025, 2026), mo(1, 12), da(1, 28);
    for (int i = 1; i <= n; i++) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", yr(rng), mo(rng), da(rng));
        file << i << ",Patient_" << i << "," << sev(rng) << "," << buf << "," << age(rng) << "\n";
    }
}

// ---------- Quick Sort (randomized pivot, tail-call optimized) ----------
// field: 1 = severity_level, 2 = admission_date, 3 = age
bool lessOrEqual(const PatientRec& a, const PatientRec& b, int field, bool ascending) {
    if (field == 2) {
        bool cmp = a.admissionDate <= b.admissionDate;
        return ascending ? cmp : (a.admissionDate >= b.admissionDate);
    }
    double ka = (field == 1) ? a.severityLevel : a.age;
    double kb = (field == 1) ? b.severityLevel : b.age;
    return ascending ? (ka <= kb) : (ka >= kb);
}

int partition(vector<PatientRec>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    uniform_int_distribution<int> dist(low, high);
    swap(arr[dist(rng)], arr[high]);
    PatientRec pivot = arr[high];
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

void quickSort(vector<PatientRec>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
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
    const string inputFile = "hospital_patients.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample patient records...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<PatientRec> patients = readCSV(inputFile);
    cout << "Loaded " << patients.size() << " records.\n";

    cout << "\nSort by: 1) Severity Level  2) Admission Date  3) Age\nEnter choice: ";
    int field; cin >> field;
    cout << "Order: 1) Ascending  2) Descending\n";
    cout << "(For severity, Ascending = most critical first)\nEnter choice: ";
    int order; cin >> order;
    bool ascending = (order == 1);

    mt19937 rng(random_device{}());
    auto start = chrono::high_resolution_clock::now();
    quickSort(patients, 0, (int)patients.size() - 1, field, ascending, rng);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Quick Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("hospital_patients_sorted.csv", patients);
    cout << "Sorted data written to hospital_patients_sorted.csv\n";


    return 0;
}
