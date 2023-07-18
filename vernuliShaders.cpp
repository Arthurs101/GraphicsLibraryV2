
#include <vector>

std::vector<float> vertexShader(std::vector<float> vertex, std::vector<std::vector<float>> modelMatrix) {
    std::vector<float> vt = { vertex[0], vertex[1], vertex[2], 1 };

    for (int i = 0; i < 4; i++) {
        float sum = 0;
        for (int j = 0; j < 4; j++) {
            sum += modelMatrix[i][j] * vt[j];
        }
        vt[i] = sum;
    }

    vt[0] /= vt[3];
    vt[1] /= vt[3];
    vt[2] /= vt[3];

    return vt;
}