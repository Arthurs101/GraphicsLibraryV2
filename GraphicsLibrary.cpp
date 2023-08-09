#include "VernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1920; int height = 1080;
    vgImage Img(width, height);
    Img.vgLoad3dObject("Bug.obj", { (float)width / 2  , (float)height/2-200 ,0 }, { 800,800,800 }, {0,0,90});
    Img.Render3DObjects();
    Img.vgWriteBMP("Output.bmp");
    return 0;
}