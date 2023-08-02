#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 6000; int height = 4000;
    vgImage Img(width, height);
    Img.vgLoad3dObject("ALLOSAUR.OBJ", { (float) width/2 + 400 , 0 ,0}, {800,800,800}, {0,0,45});
    Img.Render3DObjects();
    Img.vgWriteBMP("Baricentric-Test.bmp");
    return 0;
}