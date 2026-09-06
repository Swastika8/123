// 1-E: Banking Transactions — Merge Sort by transaction_amount
// Compile: g++ -O2 -o banking 1E_banking_transactions_merge_sort.cpp
// Run:     ./banking
// Input:   bank_transactions.csv (auto-generated with 1,000,000 rows if missing)
// Format:  transaction_id,account_no,amount,type

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>

using namespace std;

struct BankTransaction {
    long id;
    string accountNo;
    double amount;
    string type; // "CREDIT" or "DEBIT"
};

// ---------- CSV I/O ----------
vector<BankTransaction> readCSV(const string& filename) {
    vector<BankTransaction> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, acc, amtStr, type;
        getline(ss, idStr, ',');
        getline(ss, acc, ',');
        getline(ss, amtStr, ',');
        getline(ss, type, ',');
        data.push_back({stol(idStr), acc, stod(amtStr), type});
    }
    return data;
}

void writeCSV(const string& filename, const vector<BankTransaction>& data) {
    ofstream file(filename);
    for (auto& t : data)
        file << t.id << "," << t.accountNo << "," << t.amount << "," << t.type << "\n";
}

void generateSampleData(const string& filename, int n) {
    ofstream file(filename);
    mt19937 rng(42);
    uniform_real_distribution<double> amt(1.0, 1000000.0);
    uniform_int_distribution<int> acctSuffix(100000, 999999);
    for (int i = 1; i <= n; i++) {
        string type = (i % 2 == 0) ? "CREDIT" : "DEBIT";
        file << i << ",ACC" << acctSuffix(rng) << "," << amt(rng) << "," << type << "\n";
    }
}

// ---------- Merge Sort ----------
void merge(vector<BankTransaction>& arr, int l, int m, int r) {
    vector<BankTransaction> left(arr.begin() + l, arr.begin() + m + 1);
    vector<BankTransaction> right(arr.begin() + m + 1, arr.begin() + r + 1);

    size_t i = 0, j = 0;
    int k = l;
    while (i < left.size() && j < right.size())
        arr[k++] = (left[i].amount <= right[j].amount) ? left[i++] : right[j++];
    while (i < left.size()) arr[k++] = left[i++];
    while (j < right.size()) arr[k++] = right[j++];
}

void mergeSort(vector<BankTransaction>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

int main() {
    const string inputFile = "bank_transactions.csv";
    const int SAMPLE_SIZE = 1000000;

    ifstream test(inputFile);
    if (!test.good()) {
        cout << "No input file found. Generating " << SAMPLE_SIZE << " sample bank transactions...\n";
        generateSampleData(inputFile, SAMPLE_SIZE);
    }
    test.close();

    cout << "Reading data...\n";
    vector<BankTransaction> transactions = readCSV(inputFile);
    cout << "Loaded " << transactions.size() << " records.\n";

    auto start = chrono::high_resolution_clock::now();
    mergeSort(transactions, 0, (int)transactions.size() - 1);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;
    cout << "Merge Sort completed in " << elapsed.count() << " seconds.\n";

    writeCSV("bank_transactions_sorted.csv", transactions);
    cout << "Sorted data written to bank_transactions_sorted.csv\n";

    return 0;
}
