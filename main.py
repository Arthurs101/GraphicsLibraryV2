import shaders
from gl import Renderer, V2, color

width = 4000
height = 4000

rend = Renderer(width,height)

rend.vertexShader = shaders.vertexShader
rend.fragmentShader = shaders.fragmentShader

rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(800,height/2,0),rotate=(0,190,0),scale=(6000,6000,6000))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(800,800,0),rotate=(45,90,0),scale=(6000,6000,6000))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(width - 800,height/2,0),rotate=(0,0,0),scale=(6000,6000,6000))
rend.glLoadModel(filename="Yanfei.obj",textureName="Yanfei.bmp",translate=(width - 800,height/2- 200,0),rotate=(0,90,190),scale=(6000,6000,6000))
rend.glRender()

#triangle = [(100,100),(450,180),(250,500)]

#rend.glTriangle_bc(triangle[0],triangle[1],triangle[2])

rend.glFinish("output.bmp")