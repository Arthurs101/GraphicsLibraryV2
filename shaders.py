import random
from ml import *

def vertexShader(vertex, **kwargs):
    # El Vertex Shader se lleva a cabo por cada vértice

    modelMatrix = kwargs["modelMatrix"]
    viewMatrix = kwargs["viewMatrix"]
    projectionMatrix = kwargs["projectionMatrix"]
    vpMatrix = kwargs["vpMatrix"]

    vt = [vertex[0],
          vertex[1],
          vertex[2],
          1]

    vt = ml.multiplicar_matriz_vector(ml.multiplicar_matrices(ml.multiplicar_matrices(ml.multiplicar_matrices(vpMatrix,projectionMatrix),viewMatrix),modelMatrix),vt)

    vt = [vt[0] / vt[3],
          vt[1] / vt[3],
          vt[2] / vt[3]]

    return vt


def fragmentShader(**kwargs):
    # El Fragment Shader se lleva a cabo por cada pixel
    # que se renderizará en la pantalla.

    texCoords = kwargs["texCoords"]
    texture = kwargs["texture"]

    if texture != None:
        color = texture.getColor(texCoords[0], texCoords[1])
    else:
        color = (1, 1, 1)

    return color


def experimentalShader(**kwargs): #shader para pintado de pixel en escala blanco y negro
    texCoords = kwargs["texCoords"]
    texture = kwargs["texture"]
    lightCoords = kwargs["lightCoords"]
    A = kwargs["A"]
    B = kwargs["B"]
    C = kwargs["C"]
    normal = calculate_face_normal(A, B, C)
    intensity = dot_product(normal , lightCoords)
    intensity = max(intensity,0.0) # garantizara siempre que sea mayor a 0

    if texture != None:
        _color = texture.getColor(texCoords[0], texCoords[1])
        color = (_color[0] * intensity , _color[1] * intensity , _color[2] * intensity )
    else:
        color = (0, 0, 0)
    return color