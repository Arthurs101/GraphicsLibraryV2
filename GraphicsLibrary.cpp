#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1920; int height = 1080;
    vgImage Img(width, height);
    Img.vgLoad3dObject("Bug.obj", { (float) width/2 , (float) height/2,0}, {800,800,400}, {90,0,0});
    Img.Render3DObjects();
    Img.vgWriteBMP("vg.bmp");
    return 0;
}