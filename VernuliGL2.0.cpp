#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "vernuliShaders.cpp"
#include "ObjectParser.h"
#include "LinealAlgebra.h"




//estructuras
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    Pixel(unsigned char r, unsigned char g, unsigned char b) {
        red = r;
        green = g;
        blue = b;
    }
    Pixel(std::vector<int> rgb) {
        red = rgb[0];
        green = rgb[1];
        blue = rgb[2];
    }
};
struct Image {
    std::vector<std::vector<Pixel>> pixels;
    int width;
    int height;
    Image(int w, int h) : pixels(w, std::vector<Pixel>(h, { 0, 0, 0 })), width(w), height(h) {}
};

//constantes y variables globales

# define PI 3.14159265
class vgImage {
    int width;
    int height;
    Image imgData;
    std::vector<ObjParser> objects;
    public:
        vgImage(int w, int h) : width(w), height(h), imgData(w, h) {}
        /*
        Funcion para escribir el archivo BMP
        */
        void vgWriteBMP(const std::string& filename) {
            std::ofstream file(filename, std::ios::binary);

            // Header
            file.put('B');
            file.put('M');
            int fileSize = 14 + 40 + (width * height * 3);
            file.write(reinterpret_cast<char*>(&fileSize), sizeof(int));
            int reserved = 0;
            file.write(reinterpret_cast<char*>(&reserved), sizeof(int));
            int dataOffset = 14 + 40;
            file.write(reinterpret_cast<char*>(&dataOffset), sizeof(int));

            // InfoHeader
            int infoHeaderSize = 40;
            file.write(reinterpret_cast<char*>(&infoHeaderSize), sizeof(int));
            file.write(reinterpret_cast<char*>(&width), sizeof(int));
            file.write(reinterpret_cast<char*>(&height), sizeof(int));
            short planes = 1;
            file.write(reinterpret_cast<char*>(&planes), sizeof(short));
            short bitsPerPixel = 24;
            file.write(reinterpret_cast<char*>(&bitsPerPixel), sizeof(short));
            int compression = 0;
            file.write(reinterpret_cast<char*>(&compression), sizeof(int));
            int imageSize = width * height * 3;
            file.write(reinterpret_cast<char*>(&imageSize), sizeof(int));
            int horizontalResolution = 0;
            file.write(reinterpret_cast<char*>(&horizontalResolution), sizeof(int));
            int verticalResolution = 0;
            file.write(reinterpret_cast<char*>(&verticalResolution), sizeof(int));
            int colors = 0;
            file.write(reinterpret_cast<char*>(&colors), sizeof(int));
            int importantColors = 0;
            file.write(reinterpret_cast<char*>(&importantColors), sizeof(int));

            // Color table
            for (int y = 0; y < height; y++) { // BMP files are stored upside down
                for (int x = 0; x < width; x++) {
                    file.put(imgData.pixels[x][y].blue);
                    file.put(imgData.pixels[x][y].green);
                    file.put(imgData.pixels[x][y].red);
                }
            }

            file.close();
        }
        
        /*colorear ub pixel*/
        void vgPoint(int x, int y, Pixel clr = { 255,255,255 }) {
            if (0 <= x && x < width && 0 <= y && y < height) {
                imgData.pixels[x][y].red = clr.red;
                imgData.pixels[x][y].green = clr.green;
                imgData.pixels[x][y].blue = clr.blue;
            }
        }
        
        /*
        Dibujar linea
        */
        void vgCreateLine(std::vector<float> v0, std::vector<float> v1, Pixel clr = { 255,255,255 }) {
            int x0 = static_cast<int>(v0[0]);
            int x1 = static_cast<int>(v1[0]);
            int y0 = static_cast<int>(v0[1]);
            int y1 = static_cast<int>(v1[1]);

            // Si el punto 0 es igual al punto 1, solo dibuja un punto
            if (x0 == x1 && y0 == y1) {
                vgPoint(x0, y0, clr);
                return;
            }

            int dy = std::abs(y1 - y0);
            int dx = std::abs(x1 - x0);

            bool steep = dy > dx;

            // Si la línea tiene pendiente mayor a 1 o menor a -1
            // intercambiamos las x por las y, y se dibuja la línea
            // de manera vertical en lugar de horizontal
            if (steep) {
                std::swap(x0, y0);
                std::swap(x1, y1);
            }

            // Si el punto inicial en x es mayor que el punto final en x,
            // intercambiamos los puntos para siempre dibujar de
            // izquierda a derecha
            if (x0 > x1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            dy = std::abs(y1 - y0);
            dx = std::abs(x1 - x0);

            float offset = 0;
            float limit = 0.5;
            float m = static_cast<float>(dy) / dx;
            int y = y0;

            for (int x = x0; x <= x1; x++) {
                if (steep) {
                    // Dibujar de manera vertical
                    vgPoint(y, x, clr);
                }
                else {
                    vgPoint(x, y, clr);
                }

                offset += m;
                if (offset >= limit) {
                    if (y0 < y1) {
                        y += 1;
                    }
                    else {
                        y -= 1;
                    }
                    limit += 1;
                }
            }
        };
        
        void vgCreatePoligon(std::vector<std::vector<float>> vertices, bool fill = false, Pixel clr = { 255,255,255 }){
            if (vertices.size() < 2) {
                std::cout << "Se requieren al menos dos coordenadas para combinar." << std::endl;
                return;
            };
            
            std::vector<std::vector<float>> combined = vertices;
            // Combina el último punto con el primer punto
            std::vector<float> lastPoint = combined.back();
            std::vector<float> firstPoint = combined.front();
            vgCreateLine(lastPoint, firstPoint,clr);

            // Combina cada par de coordenadas consecutivas
            for (size_t i = 0; i < combined.size() - 1; ++i) {
                std::vector<float> currentPoint = combined[i];
                std::vector<float> nextPoint = combined[i + 1];
                vgCreateLine(currentPoint, nextPoint, clr);
            }
            if (fill) {
                //obtener los limites x, y de los vertices que conforman el poligono
                float minX = std::numeric_limits<float>::max();
                float maxX = std::numeric_limits<float>::min();
                for (const auto& vertex : vertices) {
                    minX = std::min(minX, vertex[0]);
                    maxX = std::max(maxX, vertex[0]);
                }

                float minY = std::numeric_limits<float>::max();
                float maxY = std::numeric_limits<float>::min();
                for (const auto& vertex : vertices) {
                    minY = std::min(minY, vertex[1]);
                    maxY = std::max(maxY, vertex[1]);
                }

                int vertexS = vertices.size();

                for (float y = minY; y <= maxY; y++) {
                    std::vector<float> intersecciones = {};
                    //calcular las intersecciones de los vertices
                    for (int i = 0; i < vertexS; i++) {
                        int k = (i + 1) % vertexS;
                        std::vector<float> V0 = vertices[i];
                        std::vector<float> V1 = vertices[k];
                        //verificar alguna interseccion del vertice con la fila Y actual
                        if (V0[1] < y and V1[1] >= y or V1[1] < y and V0[1] >= y) {
                            float iP = V0[0] + (y - V0[1]) / (V1[1] - V0[1]) * (V1[0] - V0[0]);
                            intersecciones.push_back(iP);
                        }
                    }
                    //ordenar las intersecciones
                    std::sort(intersecciones.begin(), intersecciones.end());
                    //pintar cada interseccion 
                    for (int i = 0; i < intersecciones.size(); i += 2) {
                        float x0 = intersecciones[i];
                        float x1 = std::min(intersecciones[i + 1], maxX);
                        for (int x = x0; x <= x1; x += 1) {
                            vgPoint(x, y, clr);
                        }
                    }
                }
            }
        }
        
        /*cargar objetos 3d*/
        void vgLoad3dObject(const std::string& filename, const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }) {
            objects.push_back(ObjParser(filename, transformobj, scaleobj, rotationobj));
        }

        

        void Render3DObjects(char PRIMTYPE = 't') {
            for (int i = 0; i < objects.size(); i++) {
                std::vector<std::vector<float> > M = Generate3DObjectMatrix(objects[i].transform, objects[i].scale,objects[i].rotation);
                std::vector<std::vector<float>> shaded_vertices = {};
                int faces_ = objects[i].faces.size();
                for (int face = 0; face < faces_; face++) {
                    std::vector<float> v0 = objects[i].vertices[objects[i].faces[face][0][0] - 1];
                    std::vector<float> v1 = objects[i].vertices[objects[i].faces[face][1][0] - 1];
                    std::vector<float> v2 = objects[i].vertices[objects[i].faces[face][2][0] - 1];
                    if (objects[i].faces[face].size() == 4) {
                        std::vector<float> v3 = objects[i].vertices[objects[i].faces[face][3][0] - 1];;
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

        private:
            void Assemble(std::vector<std::vector<float>>& vertices, int thickness = 1, char PRIMTYPE = 't') {
                switch (PRIMTYPE)
                {
                case 't':
                    for (int vtx = 0; vtx < vertices.size(); vtx += 3) {
                        std::vector<std::vector<float>> triangle = { {vertices[vtx], vertices[vtx + 1], vertices[vtx + 2]} };
                        vgCreatePoligon(triangle, false ,Pixel(fragmentShader()));
                        //vgTriangle_bc(vertices[vtx], vertices[vtx + 1], vertices[vtx + 2]);
                    };
                    break;

                default:
                    std::runtime_error("Unknown PRIMTYPE, try; p for point, t for triangles , l: line , s: square");
                    break;
                }

            }

            std::vector<std::vector<float> > Generate3DObjectMatrix(const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }) {
               
                std::vector<std::vector<float> > Mt = { {1,0,0,transformobj[0]},
                                                      {0,1,0,transformobj[1]},
                                                      {0,0,1,transformobj[2]},
                                                      {0,0,0,1} };
                std::vector<std::vector<float> > Ms = { {scaleobj[0],0,0,0},
                                                      {0,scaleobj[1],0,0},
                                                      {0,0,scaleobj[2],0},
                                                      {0,0,0,1} };
                std::vector<std::vector<float> > Mr = Generate3DAnglesMatrix(rotationobj[0], rotationobj[1], rotationobj[2]);
                std::vector<std::vector<float> > tmp = multiplyMatrices(Mt, Mr);
                return multiplyMatrices(tmp,Ms);
            };

            std::vector<std::vector<float> > Generate3DAnglesMatrix(float pitch , float yaw , float roll) {
                pitch *= PI/180;
                roll +=  PI / 180;
                yaw += PI / 180;
                std::vector<std::vector<float> > Rx = { { 1,0,0,0 }, 
                                                        { 0,(float)cos(pitch),(float)-sin(pitch),0}, 
                                                        {0,(float)sin(pitch),(float)cos(pitch),0}, 
                                                        {0,0,0,1} };
                std::vector<std::vector<float> > Ry = { { (float)cos(roll) , 0, (float)sin(roll), 0 },
                                                      { 0        , 1, 0       , 0 },
                                                      { (float)-sin(roll), 0, (float)cos(roll), 0 },
                                                      { 0        , 0, 0       , 1 }};
                std::vector<std::vector<float> > Rz = { { (float)cos(yaw),-(float)sin(yaw),0,0 }, 
                                                        { (float)sin(yaw),(float)cos(yaw),0,0 },
                                                        { 0,0,1,0 }, 
                                                        { 0,0,0,1 } };
                return multiplyMatrices(multiplyMatrices(Rx, Ry), Rz);
            }

            // Función para calcular el área de un triángulo formado por tres puntos
            float area(const std::vector<float>& A, const std::vector<float>& B, const std::vector<float>& C) {
                return std::abs((A[0] * B[1] + B[0] * C[1] + C[0] * A[1]) - (A[1] * B[0] + B[1] * C[0] + C[1] * A[0])) / 2.0f;
            }

            // Función para verificar si un punto P está dentro de un triángulo ABC utilizando coordenadas baricéntricas
            bool isInsideTriangle(const std::vector<float>& A, const std::vector<float>& B, const std::vector<float>& C, const std::vector<float>& P) {
                float areaABC = area(A, B, C);
                float areaPBC = area(P, B, C);
                float areaAPC = area(A, P, C);
                float areaABP = area(A, B, P);

                // Calcular las coordenadas baricéntricas (u, v, w) del punto P
                float u = areaPBC / areaABC;
                float v = areaAPC / areaABC;
                float w = areaABP / areaABC;

                return (0 <= u && u <= 1 && 0 <= v && v <= 1 && 0 <= w && w <= 1);
            }

            // Función para pintar el triángulo utilizando colores dados para cada vértice
            void vgTriangle_bc(std::vector<float> V0, std::vector<float> V1, std::vector<float> V2) {
                int minX = std::round(std::min({ V0[0], V1[0], V2[0] }));
                int maxX = std::round(std::max({ V0[0], V1[0], V2[0] }));
                int minY = std::round(std::min({ V0[1], V1[1], V2[1] }));
                int maxY = std::round(std::max({ V0[1], V1[1], V2[1] }));

                std::vector<float> colorV0 = { 1.0f, 0.0f, 0.0f };
                std::vector<float> colorV1 = { 0.0f, 1.0f, 0.0f };
                std::vector<float> colorV2 = { 0.0f, 0.0f, 1.0f };

                for (int y = minY; y <= maxY; ++y) {
                    for (int x = minX; x <= maxX; ++x) {
                        std::vector<float> P = { static_cast<float>(x), static_cast<float>(y) };
                        if (isInsideTriangle(V0, V1, V2, P)) {
                            // Pintar el pixel (x, y) con el color interpolado
                            // en función de las coordenadas baricéntricas
                            float u = area(P, V1, V2) / area(V0, V1, V2);
                            float v = area(V0, P, V2) / area(V0, V1, V2);
                            float w = area(V0, V1, P) / area(V0, V1, V2);

                            std::vector<float> pixelColor(3);
                            for (int i = 0; i < 3; ++i) {
                                pixelColor[i] = u * colorV0[i] + v * colorV1[i] + w * colorV2[i];
                            }
                            vgPoint(x, y, { (unsigned char) pixelColor[0],(unsigned char) pixelColor[1],(unsigned char) pixelColor[2] });
                        }
                    }
                }
            }

            

};
