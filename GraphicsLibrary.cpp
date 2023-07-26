#include "vernuliGL.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 800;
    int height = 500;
    generateBMP(width, height);
    clearAllImage(0, 0, 0);
    vector<vector<float>> points;
    points.push_back({ 165, 380 });
    points.push_back({ 185, 360 });
    points.push_back({ 180, 330 });
    points.push_back({ 207, 345 });
    points.push_back({ 233, 330 });
    points.push_back({ 230, 360 });
    points.push_back({ 250, 380 });
    points.push_back({ 220, 385 });
    points.push_back({ 205, 410 });
    points.push_back({ 193, 383 });
    std::vector<std::vector<float>> poligono2 = {
        {321.0f, 335.0f},
        {288.0f, 286.0f},
        {339.0f, 251.0f},
        {374.0f, 302.0f}
    };
    std::vector<std::vector<float>> poligono3 = {
    {377.0f, 249.0f},
    {411.0f, 197.0f},
    {436.0f, 249.0f}
    };
    std::vector<std::vector<float>> poligono4 = {
    {413.0f, 177.0f},
    {448.0f, 159.0f},
    {502.0f, 88.0f},
    {553.0f, 53.0f},
    {535.0f, 36.0f},
    {676.0f, 37.0f},
    {660.0f, 52.0f},
    {750.0f, 145.0f},
    {761.0f, 179.0f},
    {672.0f, 192.0f},
    {659.0f, 214.0f},
    {615.0f, 214.0f},
    {632.0f, 230.0f},
    {580.0f, 230.0f},
    {597.0f, 215.0f},
    {552.0f, 214.0f},
    {517.0f, 144.0f},
    {466.0f, 180.0f}
    };
    std::vector<std::vector<float>> poligono5 = {
    {682.0f, 175.0f},
    {708.0f, 120.0f},
    {735.0f, 148.0f},
    {739.0f, 170.0f}
    };


    DrawPoligon(points, 1, true, {0,0,255});
    DrawPoligon(poligono2, 1, true, { 0,255,255 });
    DrawPoligon(poligono3, 1, true, { 0,255,0 });
    DrawPoligon(poligono4 , 1, true, { 255,0,255 });
    DrawPoligon(poligono5, 1, true, { 0,0,0 });
    writeBMP("poligons-lab.bmp");
    return 0;
}