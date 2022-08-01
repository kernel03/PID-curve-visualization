import serial
import threading
import numpy as np
import pyqtgraph as pg

TARGET = 50


def uart():
    while True:
        if mSerial.inWaiting():
            ret = mSerial.read()

            print(float(ord(ret)))
            dat = int.from_bytes(ret, byteorder='little', signed=True)

            global i
            global target
            target[i-1] = TARGET
            if i < historyLength:
                data[i] = dat
                i = i+1
            else:
                data[:-1] = data[1:]
                data[i-1] = dat


def plot_data():
    curve1.setData(data)
    curve2.setData(target)


if __name__ == '__main__':
    app = pg.mkQApp()
    win = pg.GraphicsWindow()
    win.setWindowTitle(u'pid曲线')
    win.resize(1000, 800)

    i = 1
    historyLength = 1000
    data = np.zeros(historyLength, dtype=np.float)
    target = np.zeros(historyLength, dtype=np.float)

    p = win.addPlot()
    p.showGrid(x=True, y=True)
    p.setRange(xRange=[0, historyLength], padding=0)
    p.setLabel(axis='left', text='value')
    p.setLabel(axis='bottom', text='time')
    p.setTitle('pid曲线')
    curve1 = p.plot(pen='y')
    curve2 = p.plot(pen='r')

    mSerial = serial.Serial('COM12', 115200)
    if mSerial.isOpen():
        print('open success')
        mSerial.flushInput()
    else:
        print('open failed')
        mSerial.close()

    th1 = threading.Thread(target=uart)
    th1.start()

    timer = pg.QtCore.QTimer()
    timer.timeout.connect(plot_data)
    timer.start(50)

    app.exec_()
