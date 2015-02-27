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

sonar = win.addPlot(title="Sonar")
curvesnr0 = sonar.plot(pen=(255,0,0))
curvesnr0m = sonar.plot(pen=(255,255,0))

N = 300
snr0data = np.zeros(N)
snr0mdata = np.zeros(N)

while True:
	try:
		line = s.readline().decode("utf-8")
		#print("Leido: [" + line + "]")
		data = json.loads(line)

		snr0 = data['snr0']

	except:
		continue

	snr0m = np.mean(snr0data[0:50])
	snr0v = np.var(snr0data[0:50])
	print(str(snr0m) + "  \t+-" + str(snr0v))
	snr0data = np.roll(snr0data, 1)
	snr0mdata = np.roll(snr0mdata, 1)
	snr0data[0] = snr0
	snr0mdata[0] = snr0m

	curvesnr0.setData(snr0data)
	curvesnr0m.setData(snr0mdata)

	pg.QtGui.QApplication.processEvents()
