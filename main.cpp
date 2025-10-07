#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <limits>

#define STATUS_OK 0
#define STATUS_NULL_PTR 1
#define STATUS_ZERO_SIZE 2

int find_max_sse(const float* arr, float* max_val, size_t* max_idx, size_t size) {
    if (!arr || !max_val || !max_idx) return STATUS_NULL_PTR;
    if (size == 0) return STATUS_ZERO_SIZE;

    size_t i = 0;
    float current_max = arr[0];
    size_t current_idx = 0;

    __m128 max_vec = _mm_set1_ps(arr[0]);
    alignas(16) float temp[4];

    for (i = 0; i + 3 < size; i += 4) {
        __m128 v = _mm_loadu_ps(&arr[i]);
        max_vec = _mm_max_ps(max_vec, v);
    }

    _mm_store_ps(temp, max_vec);
    for (int j = 0; j < 4; ++j) {
        if (temp[j] > current_max) current_max = temp[j];
    }

    for (; i < size; ++i) {
        if (arr[i] > current_max) current_max = arr[i];
    }

    for (size_t k = 0; k < size; ++k) {
        if (arr[k] == current_max) {
            current_idx = k;
            break;
        }
    }

    *max_val = current_max;
    *max_idx = current_idx;

    return STATUS_OK;
}

std::vector<float> parse_line(const std::string &line) {
    std::vector<float> result;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ';')) {
        item.erase(0, item.find_first_not_of(" \t\r\n"));
        item.erase(item.find_last_not_of(" \t\r\n") + 1);
        if (!item.empty()) {
            std::replace(item.begin(), item.end(), ',', '.');
            try { result.push_back(std::stof(item)); }
            catch (...) { std::cerr << "Warning: invalid number \"" << item << "\" skipped\n"; }
        }
    }
    return result;
}

int main() {
    std::ifstream file("input_one_array.txt");
    if (!file.is_open()) { std::cerr << "Cannot open file\n"; return 1; }

    size_t size;
    file >> size;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string line;
    std::getline(file, line);

    auto arr = parse_line(line);
    if (arr.size() != size) { std::cerr << "Array size mismatch\n"; return 1; }

    float max_val;
    size_t max_idx;
    int status = find_max_sse(arr.data(), &max_val, &max_idx, size);
    if (status != STATUS_OK) { std::cerr << "Error: " << status << "\n"; return 1; }

    std::cout << "Max value: " << max_val << ", Index: " << max_idx << "\n";
    return 0;
}
