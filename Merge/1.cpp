// 1-A: Hospital Patient Records — Merge Sort by admission_time
// Compile: g++ -O2 -o hospital 1A_hospital_patient_merge_sort.cpp
// Run:     ./hospital
// Input:   patients.csv (auto-generated with 1,000,000 rows if missing)
// Format:  patient_id,name,admission_time   (admission_time = "YYYY-MM-DD HH:MM:SS")

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct Patient {
    long id;
    string name;
    string admissionTime; // ISO format -> lexicographic compare == chronological compare
};

// ---------- CSV I/O ----------
vector<Patient> readCSV(const string& filename) {
    vector<Patient> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, time;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, time, ',');
        data.push_back({stol(idStr), name, time});
    }
    return data;
}

void writeCSV(const string& filename, const vector<Patient>& data) {
    ofstream file(filename);
    for (auto& p : data)
        file << p.id << "," << p.name << "," << p.admissionTime << "\n";
}

// generates dummy data so the program is runnable end-to-end
void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_int_distribution<int> yr(2023, 2026), mo(1, 12), da(1, 28), hr(0, 23), mi(0, 59), se(0, 59);
    for (int i = 1; i <= n; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 yr(rng), mo(rng), da(rng), hr(rng), mi(rng), se(rng));
        file << i << ",Patient_" << i << "," << buf << "\n";
    }
}

// ---------- Merge Sort ----------
void merge(vector<Patient>& arr, int l, int m, int r) {
    vector<Patient> left(arr.begin() + l, arr.begin() + m + 1);
    vector<Patient> right(arr.begin() + m + 1, arr.begin() + r + 1);

    size_t i = 0, j = 0;
    int k = l;
    while (i < left.size() && j < right.size())
        arr[k++] = (left[i].admissionTime <= right[j].admissionTime) ? left[i++] : right[j++];
    while (i < left.size()) arr[k++] = left[i++];
    while (j < right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<Patient>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

int main() {
    const string inputFile = "patients.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample patient records...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<Patient> patients = readCSV(inputFile);
    cout << "Loaded " << patients.size() << " records.\n";

    auto start = chrono::high_resolution_clock::now();
    mergeSort(patients, 0, (int)patients.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Merge Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("patients_sorted.csv", patients);
    cout << "Sorted data written to patients_sorted.csv\n";


    return 0;
}
