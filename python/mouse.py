import json
import serial
from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import pyqtgraph as pg

#pygame.init()
#pantalla = pygame.display.set_mode((1200, 600), 0, 32)
app = QtGui.QApplication([])

s = serial.Serial('/dev/ttyUSB0', 115200, timeout=None)

win = pg.GraphicsWindow(title="Basic plotting examples")
win.resize(1000,600)
win.setWindowTitle('pyqtgraph example: Plotting')

# Enable antialiasing for prettier plots
pg.setConfigOptions(antialias=True)

posx = win.addPlot(title="Posición x")
posy = win.addPlot(title="Posición y")
vel = win.addPlot(title="Velocidad")
motor = win.addPlot(title="Motores")
curvex = posx.plot(pen=(255,0,0))
curvey = posy.plot(pen=(0,255,0))
curvedx = vel.plot(pen=(150,0,0))
curvedy = vel.plot(pen=(0,150,0))
curveml0 = motor.plot(pen=(255,0,255))
curvemr0 = motor.plot(pen=(0,255,255))
curveml1 = motor.plot(pen=(255,0,150))
curvemr1 = motor.plot(pen=(0,255,150))

N = 300
xdata = np.zeros(N)
ydata = np.zeros(N)
dxdata = np.zeros(N)
dydata = np.zeros(N)
ml0data = np.zeros(N)
mr0data = np.zeros(N)
ml1data = np.zeros(N)
mr1data = np.zeros(N)

while True:
	try:
		line = s.readline().decode("utf-8")
		print("Leido: [" + line + "]")
		data = json.loads(line)

		x = data['pos']['x']
		y = data['pos']['y']
		dx = data['pos']['dx']
		dy = data['pos']['dy']
		ml0 = data['pos']['ml0']
		mr0 = data['pos']['mr0']
		ml1 = data['pos']['ml1']
		mr1 = data['pos']['mr1']

	except:
		continue
	xdata[0] = x
	ydata[0] = y
	dxdata[0] = dx
	dydata[0] = dy
	ml0data[0] = ml0/256.
	mr0data[0] = mr0/256.
	ml1data[0] = ml1/256.
	mr1data[0] = mr1/256.

	xdata = np.roll(xdata, -1)
	ydata = np.roll(ydata, -1)
	dxdata = np.roll(dxdata, -1)
	dydata = np.roll(dydata, -1)
	ml0data = np.roll(ml0data, -1)
	mr0data = np.roll(mr0data, -1)
	ml1data = np.roll(ml1data, -1)
	mr1data = np.roll(mr1data, -1)

	curvex.setData(xdata)
	curvey.setData(ydata)
	curvedx.setData(dxdata)
	curvedy.setData(dydata)
	curveml0.setData(ml0data)
	curvemr0.setData(mr0data)
	curveml1.setData(ml1data)
	curvemr1.setData(mr1data)

	pg.QtGui.QApplication.processEvents()
