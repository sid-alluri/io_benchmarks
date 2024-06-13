#include <iostream>
#include <cuda_runtime.h>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;
using namespace std::chrono;

const vector<size_t> DATA_SIZES = {10 * 1024 * 1024, 50 * 1024 * 1024, 100 * 1024 * 1024, 250 * 1024 * 1024, 500 * 1024 * 1024, 1024 * 1024 * 1024};

struct BenchmarkResult
{
    size_t dataSize;
    double h2dLatency; // Host to Device latency in milliseconds
    double d2hLatency; // Device to Host latency in milliseconds
};

void benchmarkMemoryTransfer(size_t dataSize, vector<BenchmarkResult> &results)
{
    // Allocate host memory
    vector<char> h_data(dataSize, 'a');
    char *d_data;

    // Allocate device memory
    cudaMalloc(&d_data, dataSize);

    // Measure host to device transfer time
    auto start = high_resolution_clock::now();
    cudaMemcpy(d_data, h_data.data(), dataSize, cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    double h2dLatency = elapsed.count();
    cout << "Host to Device Transfer Latency for " << dataSize / (1024 * 1024) << " MB: " << h2dLatency << " ms" << endl;

    // Measure device to host transfer time
    start = high_resolution_clock::now();
    cudaMemcpy(h_data.data(), d_data, dataSize, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();
    end = high_resolution_clock::now();
    elapsed = duration_cast<milliseconds>(end - start);
    double d2hLatency = elapsed.count();
    cout << "Device to Host Transfer Latency for " << dataSize / (1024 * 1024) << " MB: " << d2hLatency << " ms" << endl;

    // Free device memory
    cudaFree(d_data);

    // Store the result
    results.push_back({dataSize, h2dLatency, d2hLatency});
}

void writeResultsToCSV(const vector<BenchmarkResult> &results, const string &fileName)
{
    ofstream outfile(fileName);

    if (!outfile)
    {
        cerr << "Failed to open CSV file for writing: " << fileName << endl;
        return;
    }

    outfile << "Data Size (MB),Host to Device Latency (ms),Device to Host Latency (ms)" << endl;

    for (const auto &result : results)
    {
        outfile << result.dataSize / (1024 * 1024) << ","
                << result.h2dLatency << ","
                << result.d2hLatency << endl;
    }

    outfile.close();
}

int main()
{
    vector<BenchmarkResult> results;

    for (size_t dataSize : DATA_SIZES)
    {
        benchmarkMemoryTransfer(dataSize, results);
    }

    // Write results to CSV
    writeResultsToCSV(results, "cuda_memory_transfer_benchmark_results.csv");

    return 0;
}
