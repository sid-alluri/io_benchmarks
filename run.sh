g++ -o disk_benchmark disk_io.cpp
./disk_benchmark

nvcc -o gpu_benchmark gpu_io.cu
./gpu_benchmark
