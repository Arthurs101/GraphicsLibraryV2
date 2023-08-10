import random
import ml

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
