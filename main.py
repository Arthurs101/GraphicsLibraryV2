from gl import Renderer
import shaders

# El tamaño del FrameBuffer
width = 1000
height = 1000

# Se crea el renderizador
rend = Renderer(width, height)

# Le damos los shaders que se utilizarás
rend.vertexShader = shaders.vertexShader
rend.fragmentShader = shaders.fragmentShader
rend.experimentalShader = shaders.experimentalShader
rend.glLookAt(camPos=(0,0,0),eyePos=(0,0,-1))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(0,-1.5,-3),rotate=(0,45,0),scale=(10,10,10))
rend.glRender()
rend.glFinish("dutch.bmp")