#pragma once
#include <vector>
// Multiplicar matrices
std::vector<std::vector<float>> multiplyMatrices(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) {
    int n = a.size();
    int m = b[0].size();
    int p = b.size();

    std::vector<std::vector<float>> c(n, std::vector<float>(m));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < p; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    return c;
}

// Multiplicar matriz por vector
std::vector<float> multiplyMatrixByVector(const std::vector<std::vector<float>>& a, const std::vector<float>& b) {
    int n = a.size();
    int m = b.size();

    std::vector<float> c(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            c[i] += a[i][j] * b[j];
        }
    }

    return c;
}