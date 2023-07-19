
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include "vernuliShaders.cpp"
#include "ObjectParser.h"
#include "LinealAlgebra.h"

/*
Estructura de los pixeles de la imagen, contiene los valores RGB
*/
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

//variable global de datos de la imagen
struct ImageData {
    int width;
    int height;
    Pixel* imageData;

};

//variables globales

ImageData imageBMP;
Pixel currentColor = { 255,255,255 };
std::vector<ObjParser> objects = {};
#pragma pack(push, 1);

using namespace std;

// Estructura que define la cabecera del archivo BMP
struct BMPHeader {
    uint16_t signature;      // Firma del archivo BMP ('BM')
    uint32_t fileSize;       // Tamaño total del archivo
    uint32_t reserved;       // Reservado (valor 0)
    uint32_t dataOffset;     // Desplazamiento a los datos de la imagen
    uint32_t headerSize;     // Tamaño de la cabecera (valor 40 para formato Windows)
    int32_t width;           // Ancho de la imagen en píxeles
    int32_t height;          // Alto de la imagen en píxeles
    uint16_t planes;         // Número de planos de color (valor 1)
    uint16_t bpp;            // Bits por píxel (usualmente 24 para formato sin compresión)
    uint32_t compression;    // Tipo de compresión (valor 0 para formato sin compresión)
    uint32_t dataSize;       // Tamaño de los datos de la imagen (valor 0 para formato sin compresión)
    int32_t hResolution;     // Resolución horizontal en píxeles por metro (valor 0 por defecto)
    int32_t vResolution;     // Resolución vertical en píxeles por metro (valor 0 por defecto)
    uint32_t colors;         // Número de colores en la paleta (valor 0 para formato sin paleta)
    uint32_t importantColors;// Número de colores importantes (valor 0 para todos)
};

#pragma pack(pop)



/*
    @ params : x , y , coordenadas x y del pixel
    @ brief  : calcula la posicion del array del pixel
    @ return : int de la posicion del pixel en el
*/

int getPixelIndex(int x, int y) {
    return y * imageBMP.width + x;
};

void setColor(int r, int g, int b) {
    currentColor.red = r; currentColor.green = g; currentColor.blue = b;
}

void setColorOnPixel(int x, int y, Pixel color) {
    int index = getPixelIndex(x, y);
    imageBMP.imageData[index].red = color.red;
    imageBMP.imageData[index].green = color.green;
    imageBMP.imageData[index].blue = color.blue;
}



/*
    @ params : r,g,b = valores red green blue , es decir el color a usar
    @ brief  : pinta todos los pixeles de un color indicado
*/

void clearAllImage(int r, int g, int b) {
    for (int y = 0; y < imageBMP.height; y++) {
        for (int x = 0; x < imageBMP.width; x++) {
            int pixelIndex = getPixelIndex(x, y);
            imageBMP.imageData[pixelIndex].red = r;
            imageBMP.imageData[pixelIndex].green = g;
            imageBMP.imageData[pixelIndex].blue = b;
        }
    }
}

/*
    @ params : width , height  = valores de ancho y alto de la imagen
    @ brief  : genera el array con los pixeles de la imagen
*/

void generateBMP(int width, int height) {
    imageBMP.height = height;
    imageBMP.width = width;
    imageBMP.imageData = new Pixel[height * width];
};

// Función para escribir un archivo BMP
void writeBMP(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        return;
    }

    // Cabecera del archivo BMP
    BMPHeader header;
    header.signature = 0x4D42;  // 'BM'
    header.fileSize = sizeof(BMPHeader) + imageBMP.width * imageBMP.height * 3;  // Tamaño del archivo BMP
    header.reserved = 0;
    header.dataOffset = sizeof(BMPHeader);
    header.headerSize = 40;
    header.width = imageBMP.width;
    header.height = imageBMP.height;
    header.planes = 1;
    header.bpp = 24;
    header.compression = 0;
    header.dataSize = 0;
    header.hResolution = 0;
    header.vResolution = 0;
    header.colors = 0;
    header.importantColors = 0;

    // Escribir la cabecera en el archivo
    file.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));

    // Escribir los datos de la imagen en el archivo
    const unsigned char* rawData = reinterpret_cast<const unsigned char*>(imageBMP.imageData);
    file.write(reinterpret_cast<const char*>(rawData), imageBMP.width * imageBMP.height * 3);

    file.close();
    std::cout << "Archivo BMP creado: " << filename << std::endl;
}


//Funciones de renderizado

void DrawLine(std::vector<float>& crd, std::vector<float>& crd1, int thickness) {
    float dx = abs(crd[0] - crd1[0]);
    float dy = abs(crd[1] - crd1[1]);
    float pivot = 0;
    float tolerance = 0.5;
    // determinar si es muy empinada
    bool steep = dy > dx ? true : false;
    if (steep) {
        std::reverse(crd.begin(), crd.end());
        std::reverse(crd1.begin(), crd1.end());
    }
    // en caso el mayor sea el de la derecha, voltearlos
    if (crd1[0] < crd[0]) {
        std::swap(crd1, crd);
    }
    dx = abs(crd[0] - crd1[0]);
    dy = abs(crd[1] - crd1[1]);
    // determinar si es vertical (evitar división entre 0)
    if (dx == 0) {
        int fixed = crd[0];
        for (int i = crd[1]; i <= crd1[1]; i++) {
            int index = getPixelIndex(fixed, i);
            if (index > 0) {
                imageBMP.imageData[index].red = currentColor.red;
                imageBMP.imageData[index].green = currentColor.green;
                imageBMP.imageData[index].blue = currentColor.blue;
            }
            
        }
    }
    else {
        float m = (dy + 0.0) / (dx + 0.0);
        int y0 = crd[1];
        int y1 = crd1[1];
        int y = y0;
        for (int i = crd[0]; i <= crd1[0]; i++) {
            if (steep) {
                int index = getPixelIndex(y, i);
                imageBMP.imageData[index].red = currentColor.red;
                imageBMP.imageData[index].green = currentColor.green;
                imageBMP.imageData[index].blue = currentColor.blue;
            }
            else {
                 int index = getPixelIndex(i, y);
                imageBMP.imageData[index].red = currentColor.red;
                imageBMP.imageData[index].green = currentColor.green;
                imageBMP.imageData[index].blue = currentColor.blue;
            }
            pivot += m;
            if (pivot >= tolerance) {
                if (y0 < y1) {
                    y += 1;
                }
                else {
                    y -= 1;
                }
                tolerance += 1;
            }
        }
    }
    if (thickness - 1 > 0) {
        std::vector<float> tmp = { crd[0] + 1, crd[1] + 1 };
        std::vector<float> tmp1 = { crd1[0] + 1, crd1[1] + 1 };
        DrawLine(tmp, tmp1, thickness - 1);
    }
}

void DrawLine(const std::vector<float>& crd, const std::vector<float>& crd1, int thickness) {
    std::vector<float> temp1 = crd;
    std::vector<float> temp2 = crd1;
    DrawLine(temp1, temp2, thickness);
}

void DrawPoligon(vector<vector<float>>& vertices, int thickness = 1) {
    // Verificar si hay al menos dos coordenadas para combinar
    if (vertices.size() < 2) {
        std::cout << "Se requieren al menos dos coordenadas para combinar." << std::endl;
        return;
    }

    std::vector<std::vector<float>> combined = vertices;
    // Combina el último punto con el primer punto
    std::vector<float> lastPoint = combined.back();
    std::vector<float> firstPoint = combined.front();
    DrawLine(lastPoint, firstPoint, thickness);

    // Combina cada par de coordenadas consecutivas
    for (size_t i = 0; i < combined.size() - 1; ++i) {
        std::vector<float> currentPoint = combined[i];
        std::vector<float> nextPoint = combined[i + 1];
        DrawLine(currentPoint, nextPoint, thickness);
    }
}

void DrawCircle(const std::vector<float>& center, int radius, int thickness = 1) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        // Simetría en los octantes del círculo
        int index;

        // Octante 1
        index = getPixelIndex(center[0] + x, center[1] + y);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 2
        index = getPixelIndex(center[0] + y, center[1] + x);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 3
        index = getPixelIndex(center[0] + y, center[1] - x);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 4
        index = getPixelIndex(center[0] + x, center[1] - y);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 5
        index = getPixelIndex(center[0] - x, center[1] - y);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 6
        index = getPixelIndex(center[0] - y, center[1] - x);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 7
        index = getPixelIndex(center[0] - y, center[1] + x);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        // Octante 8
        index = getPixelIndex(center[0] - x, center[1] + y);
        imageBMP.imageData[index].red = currentColor.red;
        imageBMP.imageData[index].green = currentColor.green;
        imageBMP.imageData[index].blue = currentColor.blue;

        if (d < 0) {
            d += 4 * x + 6;
        }
        else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    if (thickness - 1 > 0) {
        {
            DrawCircle(center, radius - 1, thickness - 1);
        }

    }
}

void Assemble(vector<vector<float>>& vertices, int thickness = 1, char PRIMTYPE = 't') {
    switch (PRIMTYPE)
    {
    case 't':
        for (int vtx = 0; vtx < vertices.size() ; vtx += 3) {
            vector<vector<float>> triangle = { {vertices[vtx], vertices[vtx + 1], vertices[vtx + 2]} };
            DrawPoligon(triangle, thickness);
        };
        break;

    default:
        std::runtime_error("Unknown PRIMTYPE, try; p for point, t for triangles , l: line , s: square");
        break;
    }

}

void Load3DObjects(const std::string& filename, const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }) {
    objects.push_back(ObjParser(filename, transformobj, scaleobj, rotationobj));
};

std::vector<std::vector<float> > Generate3DObjectMatrix(const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }) {
    std::vector<std::vector<float> > Rx = { { 1,0,0,0 }, { 0,cos(rotationobj[0]),-sin(rotationobj[0]),0 }, { 0,sin(rotationobj[0]),cos(rotationobj[0]),0 }, { 0,0,0,1 } };
    std::vector<std::vector<float> > Ry = { { cos(rotationobj[1]),0,sin(rotationobj[1]),0 }, { 0,1,0,0 }, { -sin(rotationobj[1]),0,cos(rotationobj[1]),0 }, { 0,0,0,1 } };
    std::vector<std::vector<float> > Rz = { { cos(rotationobj[2]),-sin(rotationobj[2]),0,0 }, { sin(rotationobj[2]),cos(rotationobj[2]),0,0 }, { 0,0,1,0 }, { 0,0,0,1 } };
    std::vector<std::vector<float> > Mt = { {1,0,0,transformobj[0]},
                                          {0,1,0,transformobj[1]},
                                          {0,0,1,transformobj[2]},
                                          {0,0,0,1} };
    std::vector<std::vector<float> > Ms = { {scaleobj[0],0,0,0},
                                          {0,scaleobj[1],0,0},
                                          {0,0,scaleobj[2],0},
                                          {0,0,0,1} };
    std::vector<std::vector<float> > Mr = multiplyMatrices(multiplyMatrices(Rx, Ry), Rz);
    return multiplyMatrices(multiplyMatrices(Mt, Mr), Ms);
};


void Render3DObjects(char PRIMTYPE = 't') {
    for (int i = 0; i < objects.size(); i++) {
        std::vector<vector<float> > M = Generate3DObjectMatrix(objects[i].transform, objects[i].scale);
        std::vector<vector<float>> shaded_vertices = {};
        int faces_ = objects[i].faces.size();
        for (int face = 0; face < faces_ ; face ++) {
            vector<float> v0 = objects[i].vertices[objects[i].faces[face][0][0]-1];
            vector<float> v1 = objects[i].vertices[objects[i].faces[face][1][0]-1];
            vector<float> v2 = objects[i].vertices[objects[i].faces[face][2][0]-1];
            if (objects[i].faces[face].size() == 4) {
                vector<float> v3 = objects[i].vertices[objects[i].faces[face][3][0] - 1];;
                shaded_vertices.push_back(vertexShader(v0, M));
                shaded_vertices.push_back(vertexShader(v2, M));
                shaded_vertices.push_back(vertexShader(v3, M));
            }
            else {
                shaded_vertices.push_back(vertexShader(v0, M));
                shaded_vertices.push_back(vertexShader(v1, M));
                shaded_vertices.push_back(vertexShader(v2, M));
            }
        }
        Assemble(shaded_vertices);
    }
}
