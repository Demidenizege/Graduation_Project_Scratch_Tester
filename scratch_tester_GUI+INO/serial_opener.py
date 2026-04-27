from serial import Serial
import threading
import serial.tools.list_ports

class SerialOpener:
    _ports = serial.tools.list_ports.comports()
    _portList = []

    _portName = ""
    _portBaudRate = 9600

    _thread = None

    serialConnector = Serial()

    def _getPorts(self):
        for port in self._ports:
            self._portList.append(str(port))
            print(str(port))

    def _startListeningThread(self, callback):
        while True:
            try:
                if self.serialConnector.is_open and self.serialConnector.in_waiting:
                    message = self.serialConnector.readline().decode("utf").rstrip("\n")
                    if message != None and message != "" and message != "None":
                        callback(message)

                if not self.serialConnector.is_open:
                    break
            except Exception as e:
                print(e)

    def _initPort(self, callback):
        if self._portName == "":
            raise RuntimeError("Port Name is empty or invalid.")
        
        self.serialConnector.baudrate = self._portBaudRate
        self.serialConnector.port = self._portName
        self.serialConnector.open()
        self._thread = threading.Thread(target = self._startListeningThread, args = (callback,))
        self._thread.daemon = True
        self._thread.start()

    def openPort(self, callback, isTesting = False):
        self._getPorts()
        selectedPort = input("Select Port: COM")

        for x in range(0, len(self._portList)):
            if self._portList[x].startswith("COM" + str(selectedPort)):
                self._portName = "COM" + str(selectedPort)

        if not isTesting:
            self._initPort(callback)

    def closePort(self):
        if not self.serialConnector.is_open:
            raise RuntimeError("You should have opened the port first in order to close it!")
        
        self.serialConnector.close()
        self._thread.join()

        if self.serialConnector.is_open:
            raise RuntimeError(f"Could not be able to close port with name: {self._portName}")
        else:
            print(f"Port with name {self._portName} closed successfuly")