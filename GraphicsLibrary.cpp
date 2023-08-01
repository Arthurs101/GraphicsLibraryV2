#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 800; int height = 800;
    vgImage Img(width, height);
    //Img.vgLoad3dObject("ALLOSAUR.OBJ", { (float) width/2 , 0 ,0}, {800,800,800}, {0,0,45});
    //Img.Render3DObjects();
    Img.drawTriangle({0,0,0}, {799,0,0}, {400,799,0});
    Img.vgCreatePoligon({ { 0,0,0 }, { 799,0,0 }, { 400,799,0 } }, false, {0,0,255});
    Img.vgWriteBMP("Baricentric-Test.bmp");
    return 0;
}