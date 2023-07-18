#include "vernuliGL.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1024;
    int height = 1024;
    generateBMP(width, height);
    clearAllImage(0, 0, 0);
    Load3DObjects("model.obj", { (float)width / 2,(float)height/ 2,1 }, {400,400,200});
    Render3DObjects();
    writeBMP("example.bmp");
    return 0;
}