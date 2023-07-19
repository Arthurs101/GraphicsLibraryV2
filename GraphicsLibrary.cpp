#include "vernuliGL.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1920;
    int height = 1920;
    generateBMP(width, height);
    clearAllImage(0, 0, 0);
    Load3DObjects("Bug.obj", { (float)width / 2,(float)height / 2,1 }, {800,800,1}, {0,0,0});
    Render3DObjects();
    writeBMP("Bug.bmp");
    return 0;
}