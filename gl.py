import ml
from math import pi, sin, cos, tan
from obj import Obj
from texture import Texture
from ml import barycentricCoords
import struct

def char(c):
    # 1 byte
    return struct.pack('=c', c.encode('ascii'))

def word(w):
    # 2 bytes
    return struct.pack('=h', w)

def dword(d):
    # 4 bytes
    return struct.pack('=l', d)

def color(r, g, b):
    return bytes([int(b * 255),
                  int(g * 255),
                  int(r * 255)])
POINTS = 0
LINES = 1
TRIANGLES = 2
QUADS = 3


class Model(object):
    def __init__(self, filename, translate=(0, 0, 0), rotate=(0, 0, 0), scale=(1, 1, 1)):
        model = Obj(filename)

        self.vertices = model.vertices
        self.texcoords = model.texcoords
        self.normals = model.normals
        self.faces = model.faces

        self.translate = translate
        self.rotate = rotate
        self.scale = scale

    def LoadTexture(self, textureName):
        self.texture = Texture(textureName)


class Renderer(object):
    def __init__(self, width, height):

        self.width = width
        self.height = height

        self.glClearColor(0, 0, 0)
        self.glClear()

        self.glColor(1, 1, 1)

        self.objects = []

        self.vertexShader = None
        self.fragmentShader = None

        self.primitiveType = TRIANGLES

        self.activeTexture = None

        self.glViewport(0, 0, self.width, self.height)
        self.glCamMatrix()
        self.glProjectionMatrix()


    def glClearColor(self, r, g, b):
        # Establecer el color de fondo
        self.clearColor = color(r, g, b)

    def glColor(self, r, g, b):
        # Establecer el color default de rederización.
        self.currColor = color(r, g, b)

    def glClear(self):
        # Se crea la tabla de pixeles del tamaño width*height.
        # Se le asigna a cada pixel el color de fondo.
        self.pixels = [[self.clearColor for y in range(self.height)]
                       for x in range(self.width)]

        # Se crea otra tabla para el Z Buffer. Aquí se guarda la profundidad
        # de cada pixel, con el valor máximo de profundidad inicial.
        self.zbuffer = [[float('inf') for y in range(self.height)]
                        for x in range(self.width)]

    def glPoint(self, x, y, clr=None):
        # Si el valor de X y Y está dentro del ancho y alto del framebuffer,
        # dibujar el punto en la posición (x,y) del FrameBuffer.
        if (0 <= x < self.width) and (0 <= y < self.height):
            self.pixels[x][y] = clr or self.currColor

    def glTriangle(self, A, B, C, vtA, vtB, vtC):
        # Rederización de un triángulo usando coordenadas baricéntricas.
        # Se reciben los vertices A, B y C y las coordenadas de
        # textura vtA, vtB y vtC

        # Bounding box
        minX = round(min(A[0], B[0], C[0]))
        maxX = round(max(A[0], B[0], C[0]))
        minY = round(min(A[1], B[1], C[1]))
        maxY = round(max(A[1], B[1], C[1]))

        # Para cada pixel dentro del bounding box
        for x in range(minX, maxX + 1):
            for y in range(minY, maxY + 1):
                # Si el pixel está dentro del FrameBuffer
                if (0 <= x < self.width) and (0 <= y < self.height):

                    P = (x, y)
                    bCoords = barycentricCoords(A, B, C, P)

                    # Si se obtienen coordenadas baricéntricas válidas para este punto
                    if bCoords != None:

                        u, v, w = bCoords

                        # Se calcula el valor Z para este punto usando las coordenadas baricéntricas
                        z = u * A[2] + v * B[2] + w * C[2]

                        # Si el valor de Z para este punto es menor que el valor guardado en el Z Buffer
                        if z < self.zbuffer[x][y]:

                            # Guardamos este valor de Z en el Z Buffer
                            self.zbuffer[x][y] = z

                            # Calcular las UVs del pixel usando las coordenadas baricéntricas.
                            uvs = (u * vtA[0] + v * vtB[0] + w * vtC[0],
                                   u * vtA[1] + v * vtB[1] + w * vtC[1])

                            # Si contamos un Fragment Shader, obtener el color de ahí.
                            # Sino, usar el color preestablecido.
                            if self.fragmentShader != None:
                                # Mandar los parámetros necesarios al shader
                                colorP = self.fragmentShader(texCoords=uvs,
                                                             texture=self.activeTexture)

                                self.glPoint(x, y, color(colorP[0], colorP[1], colorP[2]))

                            else:
                                self.glPoint(x, y)

    def glViewport(self, x, y, width, height):
        self.vpX = x
        self.vpY = y
        self.vpWidth = width
        self.vpHeight = height
        self.vpMatrix = [[self.vpWidth / 2, 0, 0, self.vpX + self.vpWidth / 2],
                                   [0, self.vpHeight / 2, 0, self.vpY + self.vpHeight / 2],
                                   [0, 0, 0.5, 0.5],
                                   [0, 0, 0, 1]]

    def glCamMatrix(self, translate=(0, 0, 0), rotate=(0, 0, 0)):
        # Crea una matriz de camara
        self.camMatrix = self.glModelMatrix(translate, rotate)
        # La matriz de vista es igual a la inversa de la camara
        self.viewMatrix = ml.matriz_inversa(self.camMatrix)

    def glLookAt(self, camPos=(0, 0, 0), eyePos=(0, 0, 0)):
        worldUp = (0, 1, 0)
        forward = ml.subtract_arrays(camPos, eyePos)
        forward = ml.normalizar_vector(forward)

        rigth = ml.producto_cruz(worldUp, forward)
        rigth = ml.normalizar_vector(rigth)

        up = ml.producto_cruz(forward, rigth)
        up = ml.normalizar_vector(up)

        self.camMatrix = [[rigth[0], up[0], forward[0], camPos[0]],
                                    [rigth[1], up[1], forward[1], camPos[1]],
                                    [rigth[2], up[2], forward[2], camPos[2]],
                                    [0, 0, 0, 1]]

        self.viewMatrix = ml.matriz_inversa(self.camMatrix)

    def glProjectionMatrix(self, fov=60, n=0.1, f=1000):
        aspectRatio = self.vpWidth / self.vpHeight
        t = tan((fov * pi / 180) / 2) * n
        r = t * aspectRatio

        self.projectionMatrix = [[n / r, 0, 0, 0],
                                           [0, n / t, 0, 0],
                                           [0, 0, -(f + n) / (f - n), -2 * f * n / (f - n)],
                                           [0, 0, -1, 0]]

    def glPrimitiveAssembly(self, tVerts, tTexCoords):

        # Esta función construye las primitivas de acuerdo con la
        # opción de primitiva actual. De momento solo hay para triángulos

        primitives = []

        if self.primitiveType == TRIANGLES:
            for i in range(0, len(tVerts), 3):
                # Un triángulo contará con las posiciones de sus vértices y
                # y sus UVs, seguidos uno tras otro.

                triangle = []
                # Verts
                triangle.append(tVerts[i])
                triangle.append(tVerts[i + 1])
                triangle.append(tVerts[i + 2])

                # TexCoords
                triangle.append(tTexCoords[i])
                triangle.append(tTexCoords[i + 1])
                triangle.append(tTexCoords[i + 2])

                primitives.append(triangle)

        return primitives

    def glModelMatrix(self, translate=(0, 0, 0), rotate=(0, 0, 0), scale=(1, 1, 1)):

        # Matriz de traslación
        translation = [[1, 0, 0, translate[0]],
                                 [0, 1, 0, translate[1]],
                                 [0, 0, 1, translate[2]],
                                 [0, 0, 0, 1]]

        # Matrix de rotación
        rotMat = self.glRotationMatrix(rotate[0], rotate[1], rotate[2])

        # Matriz de escala
        scaleMat = [[scale[0], 0, 0, 0],
                              [0, scale[1], 0, 0],
                              [0, 0, scale[2], 0],
                              [0, 0, 0, 1]]

        # Se multiplican las tres para obtener la matriz del objeto final
        return ml.multiplicar_matrices(ml.multiplicar_matrices(translation,rotMat),scaleMat)

    def glRotationMatrix(self, pitch=0, yaw=0, roll=0):

        # Convertir a radianes
        pitch *= pi / 180
        yaw *= pi / 180
        roll *= pi / 180

        # Creamos la matriz de rotación para cada eje.
        pitchMat = [[1, 0, 0, 0],
                              [0, cos(pitch), -sin(pitch), 0],
                              [0, sin(pitch), cos(pitch), 0],
                              [0, 0, 0, 1]]

        yawMat = [[cos(yaw), 0, sin(yaw), 0],
                            [0, 1, 0, 0],
                            [-sin(yaw), 0, cos(yaw), 0],
                            [0, 0, 0, 1]]

        rollMat = [[cos(roll), -sin(roll), 0, 0],
                             [sin(roll), cos(roll), 0, 0],
                             [0, 0, 1, 0],
                             [0, 0, 0, 1]]

        # Se multiplican las tres matrices para obtener la matriz de rotación final
        return ml.multiplicar_matrices(ml.multiplicar_matrices(pitchMat,yawMat),rollMat)

    def glLine(self, v0, v1, clr=None):
        # Bresenham line algorith
        # y = m*x + b

        x0 = int(v0[0])
        x1 = int(v1[0])
        y0 = int(v0[1])
        y1 = int(v1[1])

        # Si el punto 0 es igual al punto 1, solo dibujar un punto
        if x0 == x1 and y0 == y1:
            self.glPoint(x0, y0)
            return

        dy = abs(y1 - y0)
        dx = abs(x1 - x0)

        steep = dy > dx

        # Si la linea tiene pendiente mayor a 1 o menor a -1
        # intercambiamos las x por las y, y se dibuja la linea
        # de manera vertical en vez de horizontal
        if steep:
            x0, y0 = y0, x0
            x1, y1 = y1, x1

        # Si el punto inicial en X es mayor que el punto final en X,
        # intercambiamos los puntos para siempre dibujar de
        # izquierda a derecha
        if x0 > x1:
            x0, x1 = x1, x0
            y0, y1 = y1, y0

        dy = abs(y1 - y0)
        dx = abs(x1 - x0)

        offset = 0
        limit = 0.5
        m = dy / dx
        y = y0

        for x in range(x0, x1 + 1):
            if steep:
                # Dibujar de manera vertical
                self.glPoint(y, x, clr or self.currColor)
            else:
                # Dibujar de manera horizontal
                self.glPoint(x, y, clr or self.currColor)

            offset += m

            if offset >= limit:
                if y0 < y1:
                    y += 1
                else:
                    y -= 1

                limit += 1

    def glLoadModel(self, filename, textureName, translate=(0, 0, 0), rotate=(0, 0, 0), scale=(1, 1, 1)):
        # Se crea el modelo y le asignamos su textura
        model = Model(filename, translate, rotate, scale)
        model.LoadTexture(textureName)

        # Se agrega el modelo al listado de objetos
        self.objects.append(model)

    def glRender(self):

        # Esta función está encargada de renderizar todo a la tabla de pixeles

        transformedVerts = []
        texCoords = []

        # Para cada modelo en nuestro listado de objetos
        for model in self.objects:

            # Establecemos la textura y la matriz del modelo
            self.activeTexture = model.texture
            mMat = self.glModelMatrix(model.translate, model.rotate, model.scale)

            # Para cada cara del modelo
            for face in model.faces:
                # Revisamos cuantos vértices tiene esta cara. Si tiene cuatro
                # vértices, hay que crear un segundo triángulo por cara
                vertCount = len(face)

                # Obtenemos los vértices de la cara actual.
                v0 = model.vertices[face[0][0] - 1]
                v1 = model.vertices[face[1][0] - 1]
                v2 = model.vertices[face[2][0] - 1]
                if vertCount == 4:
                    v3 = model.vertices[face[3][0] - 1]

                # Si contamos con un Vertex Shader, se manda cada vértice
                # al mismo para transformarlos. Recordar pasar las matrices
                # necesarias para usarlas dentro del shader.
                if self.vertexShader:
                    v0 = self.vertexShader(v0, modelMatrix=mMat, viewMatrix=self.viewMatrix,
                                           projectionMatrix=self.projectionMatrix, vpMatrix=self.vpMatrix)
                    v1 = self.vertexShader(v1, modelMatrix=mMat, viewMatrix=self.viewMatrix,
                                           projectionMatrix=self.projectionMatrix, vpMatrix=self.vpMatrix)
                    v2 = self.vertexShader(v2, modelMatrix=mMat, viewMatrix=self.viewMatrix,
                                           projectionMatrix=self.projectionMatrix, vpMatrix=self.vpMatrix)

                    if vertCount == 4:
                        v3 = self.vertexShader(v3, modelMatrix=mMat, viewMatrix=self.viewMatrix,
                                               projectionMatrix=self.projectionMatrix, vpMatrix=self.vpMatrix)

                # Agregar cada vértice transformado al listado de vértices.
                transformedVerts.append(v0)
                transformedVerts.append(v1)
                transformedVerts.append(v2)
                if vertCount == 4:
                    transformedVerts.append(v0)
                    transformedVerts.append(v2)
                    transformedVerts.append(v3)

                # Obtenemos las coordenadas de textura de la cara actual
                vt0 = model.texcoords[face[0][1] - 1]
                vt1 = model.texcoords[face[1][1] - 1]
                vt2 = model.texcoords[face[2][1] - 1]
                if vertCount == 4:
                    vt3 = model.texcoords[face[3][1] - 1]

                # Agregamos las coordenadas de textura al listado de coordenadas de textura.
                texCoords.append(vt0)
                texCoords.append(vt1)
                texCoords.append(vt2)
                if vertCount == 4:
                    texCoords.append(vt0)
                    texCoords.append(vt2)
                    texCoords.append(vt3)

        # Creamos las primitivas
        primitives = self.glPrimitiveAssembly(transformedVerts, texCoords)

        # Para cada primitiva
        for prim in primitives:
            if self.primitiveType == TRIANGLES:
                self.glTriangle(prim[0], prim[1], prim[2],
                                prim[3], prim[4], prim[5])

    def glFinish(self, filename):
        # Esta función crea una textura BMP de 24 bits y la rellena
        # con la tabla de pixeles. Este será nuestro FrameBuffer final.

        with open(filename, "wb") as file:
            # Header
            file.write(char("B"))
            file.write(char("M"))
            file.write(dword(14 + 40 + (self.width * self.height * 3)))
            file.write(dword(0))
            file.write(dword(14 + 40))

            # InfoHeader
            file.write(dword(40))
            file.write(dword(self.width))
            file.write(dword(self.height))
            file.write(word(1))
            file.write(word(24))
            file.write(dword(0))
            file.write(dword(self.width * self.height * 3))
            file.write(dword(0))
            file.write(dword(0))
            file.write(dword(0))
            file.write(dword(0))

            # Color table
            for y in range(self.height):
                for x in range(self.width):
                    file.write(self.pixels[x][y])