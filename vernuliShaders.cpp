#pragma once
#include <vector>
#include "LinealAlgebra.h"
#include "ObjectParser.h"
std::vector<float> vertexShader(std::vector<float> vertex, std::vector<std::vector<float>> modelMatrix) {
    std::vector<float> vt = { vertex[0], vertex[1], vertex[2], 1 };

    for (int i = 0; i < 4; i++) {
        float sum = 0;
        for (int j = 0; j < 4; j++) {
            sum += modelMatrix[i][j] * vt[j];
        }
        vt[i] = sum;
    }

std::vector<int> fragmentShader() {
    return { 255,0,0 };
}
std::vector<int> fragmentShader(std::vector<float> txtCrds , Texture txt) {
    if (txt.IsValid()) {
        std::vector<float> clr = txt.getColor(txtCrds[0], txtCrds[1]);
        return { (int)clr[0] , (int)clr[1] , (int)clr[2] };
    }
    else {
        return { 0,0,255 };
    };
}