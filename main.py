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

rend.glLookAt(camPos=(0,0,0),eyePos=(0,0,-1))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(0,-1.5,-3),rotate=(0,45,0),scale=(10,10,10))
# Se renderiza la escena
rend.glRender()

# Se crea el FrameBuffer con la escena renderizada
rend.glFinish("medium.bmp")

rend.glClearColor(0, 0, 0)
rend.glClear()
rend.objects = []

rend.glLookAt(camPos=(0,-1,0),eyePos=(0,0,-1))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(0,-1.5,-3),rotate=(0,45,0),scale=(10,10,10))
# Se renderiza la escena
rend.glRender()

# Se crea el FrameBuffer con la escena renderizada
rend.glFinish("low.bmp")

rend.glClearColor(0, 0, 0)
rend.glClear()
rend.objects = []

rend.glLookAt(camPos=(0,5,0),eyePos=(0,0,-4))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(0,-1.5,-3),rotate=(0,45,0),scale=(10,10,10))
# Se renderiza la escena
rend.glRender()

# Se crea el FrameBuffer con la escena renderizada
rend.glFinish("high.bmp")

rend.glClearColor(0, 0, 0)
rend.glClear()
rend.objects = []

rend.glLookAt(camPos=(0,0,0),eyePos=(0,0,-1))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(0,-1.5,-3),rotate=(0,45,0),scale=(10,10,10))
rend.glRender()
rend.glFinish("dutch.bmp")