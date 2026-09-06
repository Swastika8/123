// 2-D: University Result Processing System — Quick Sort
// Sort student records by user-selected parameter: CGPA, total marks, or subject marks.
// Compile: g++ -O2 -std=c++17 -o university 2D_university_results_quicksort.cpp
// Run:     ./university
// Input:   student_results.csv (auto-generated with 1,000,000 rows if missing)
// Format:  student_id,name,cgpa,total_marks,subject_marks

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;

struct StudentResult {
    long id;
    string name;
    double cgpa;
    int totalMarks;
    int subjectMarks; // marks in a specific selected subject
};

// ---------- CSV I/O ----------
vector<StudentResult> readCSV(const string& filename) {
    vector<StudentResult> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, cgpaStr, totalStr, subjStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, cgpaStr, ',');
        getline(ss, totalStr, ',');
        getline(ss, subjStr, ',');
        data.push_back({stol(idStr), name, stod(cgpaStr), stoi(totalStr), stoi(subjStr)});
    }
    return data;
}

void writeCSV(const string& filename, const vector<StudentResult>& data) {
    ofstream file(filename);
    for (auto& s : data)
        file << s.id << "," << s.name << "," << s.cgpa << "," << s.totalMarks << "," << s.subjectMarks << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_real_distribution<double> cgpa(4.0, 10.0);
    uniform_int_distribution<int> total(200, 600), subj(20, 100);
    for (int i = 1; i <= n; i++)
        file << i << ",Student_" << i << "," << cgpa(rng) << "," << total(rng) << "," << subj(rng) << "\n";
}

// ---------- Quick Sort (randomized pivot, tail-call optimized) ----------
// field: 1 = cgpa, 2 = total_marks, 3 = subject_marks
double getKey(const StudentResult& s, int field) {
    if (field == 1) return s.cgpa;
    if (field == 2) return s.totalMarks;
    return s.subjectMarks;
}

int partition(vector<StudentResult>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
    uniform_int_distribution<int> dist(low, high);
    swap(arr[dist(rng)], arr[high]);
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

void quickSort(vector<StudentResult>& arr, int low, int high, int field, bool ascending, mt19937& rng) {
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
    const string inputFile = "student_results.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample student records...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<StudentResult> students = readCSV(inputFile);
    cout << "Loaded " << students.size() << " records.\n";

    cout << "\nSort by: 1) CGPA  2) Total Marks  3) Subject Marks\nEnter choice: ";
    int field; cin >> field;
    cout << "Order: 1) Ascending  2) Descending (merit list = Descending)\nEnter choice: ";
    int order; cin >> order;
    bool ascending = (order == 1);

    mt19937 rng(random_device{}());
    auto start = chrono::high_resolution_clock::now();
    quickSort(students, 0, (int)students.size() - 1, field, ascending, rng);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Quick Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("student_results_sorted.csv", students);
    cout << "Sorted data written to student_results_sorted.csv\n";


    return 0;
}
