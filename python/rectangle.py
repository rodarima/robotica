import json
import pygame
from pygame.locals import *
import sys
import serial

pygame.init()
pantalla = pygame.display.set_mode((1200, 600), 0, 32)

s = serial.Serial('/dev/ttyUSB0', 9600, timeout=None)

while True:
    line = s.readline().decode("utf-8")
    print("Leido: " + line)
    data = json.loads(str(line))

    coff = data['color']['off']
    r = data['color']['r']
    g = data['color']['g']
    b = data['color']['b']
    doff = data['distance']['off']
    don = data['distance']['on']

    dist = doff - don
    print("Distancia: " + str(dist))

    r = 1024 - r;
    g = 1024 - g;
    b = 1024 - b;
    print("r:"+str(r)+" g:"+str(g)+" b:"+str(b))

#    r=max(min(r*255.0/500.0 -150, 255),0);
#    g=max(min(g*255.0*2.0/380.0 -20, 255),0);
#    b=max(min(b*255.0/500.0 -150, 255),0);
#
#    print("r:"+str(r)+" g:"+str(g)+" b:"+str(b))
#
#    pantalla.fill((r,g,b))
#    pygame.display.update()

pygame.quit()
sys.exit()
