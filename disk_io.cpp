#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>

using namespace std;
using namespace std::chrono;

const vector<size_t> FILE_SIZES = {10 * 1024 * 1024, 50 * 1024 * 1024, 100 * 1024 * 1024}; // 10 MB, 50 MB, 100 MB
const vector<size_t> BUFFER_SIZES = {1 * 1024, 4 * 1024, 16 * 1024};                       // 1 KB, 4 KB, 16 KB
const string FILE_NAME_PREFIX = "test_file_";

struct BenchmarkResult
{
    size_t fileSize;
    size_t bufferSize;
    double writeLatency; // in milliseconds
    double readLatency;  // in milliseconds
};

void writeTestFile(size_t fileSize, size_t bufferSize, const string &fileName)
{
    ofstream outfile(fileName, ios::binary | ios::trunc);
    vector<char> buffer(bufferSize, 'a');

    if (!outfile)
    {
        cerr << "Failed to open file for writing: " << fileName << endl;
        return;
    }

    for (size_t i = 0; i < fileSize / bufferSize; ++i)
    {
        outfile.write(buffer.data(), bufferSize);
    }

    outfile.close();
}

void readTestFile(size_t bufferSize, const string &fileName)
{
    ifstream infile(fileName, ios::binary);
    vector<char> buffer(bufferSize);

    if (!infile)
    {
        cerr << "Failed to open file for reading: " << fileName << endl;
        return;
    }

    while (infile.read(buffer.data(), bufferSize))
    {
        // Simulate processing of read data
    }

    infile.close();
}

double benchmarkWrite(size_t fileSize, size_t bufferSize, const string &fileName)
{
    vector<char> buffer(bufferSize, 'a');
    ofstream outfile(fileName, ios::binary | ios::trunc);

    if (!outfile)
    {
        cerr << "Failed to open file for writing: " << fileName << endl;
        return -1;
    }

    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < fileSize / bufferSize; ++i)
    {
        outfile.write(buffer.data(), bufferSize);
    }
    auto end = high_resolution_clock::now();

    outfile.close();

    auto elapsed = duration_cast<milliseconds>(end - start);
    return elapsed.count();
}

double benchmarkRead(size_t bufferSize, const string &fileName)
{
    vector<char> buffer(bufferSize);
    ifstream infile(fileName, ios::binary);

    if (!infile)
    {
        cerr << "Failed to open file for reading: " << fileName << endl;
        return -1;
    }

    auto start = high_resolution_clock::now();
    while (infile.read(buffer.data(), bufferSize))
    {
        // Simulate processing of read data
    }
    auto end = high_resolution_clock::now();

    infile.close();

    auto elapsed = duration_cast<milliseconds>(end - start);
    return elapsed.count();
}

void writeResultsToCSV(const vector<BenchmarkResult> &results, const string &fileName)
{
    ofstream outfile(fileName);

    if (!outfile)
    {
        cerr << "Failed to open CSV file for writing: " << fileName << endl;
        return;
    }

    outfile << "File Size (MB),Buffer Size (KB),Write Latency (ms),Read Latency (ms)" << endl;

    for (const auto &result : results)
    {
        outfile << result.fileSize / (1024 * 1024) << ","
                << result.bufferSize / 1024 << ","
                << result.writeLatency << ","
                << result.readLatency << endl;
    }

    outfile.close();
}

int main()
{
    vector<BenchmarkResult> results;

    for (size_t fileSize : FILE_SIZES)
    {
        for (size_t bufferSize : BUFFER_SIZES)
        {
            string fileName = FILE_NAME_PREFIX + to_string(fileSize) + "_" + to_string(bufferSize) + ".bin";

            // Write test file
            writeTestFile(fileSize, bufferSize, fileName);

            // Benchmark write
            double writeLatency = benchmarkWrite(fileSize, bufferSize, fileName);

            // Benchmark read
            double readLatency = benchmarkRead(bufferSize, fileName);

            // Collect results
            results.push_back({fileSize, bufferSize, writeLatency, readLatency});

            // Clean up test file
            remove(fileName.c_str());
        }
    }

    // Write results to CSV
    writeResultsToCSV(results, "disk_benchmark_results.csv");

    return 0;
}
