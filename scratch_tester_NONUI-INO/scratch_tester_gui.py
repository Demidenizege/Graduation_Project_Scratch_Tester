import tkinter as tk
from PIL import ImageTk, Image
from cv2 import VideoCapture, imwrite
from process_state import ProcessState, getProcessMessage, getProcessButtonMessage
from serial_opener import SerialOpener

class ScratchTester():
    def __init__(self, isTesting: bool = False):
        # Setup State
        self.processState = ProcessState.IDLE
        self.loadCellValues = []
        self.isTesting = isTesting
        self.loadedImage: ImageTk.PhotoImage = None
        self.current_test_state = 0

        # Setup Serial Connection
        self.so = SerialOpener()
        self.so.openPort(self.__processStateMessage,isTesting = isTesting)

        # Setup Cam Connection
        cam_port = 0 #USB'YE GÖRE DEĞİŞTİR
        self.cam = VideoCapture(cam_port)

        # Setup Image Pointer
        self.testedImage: tk.Label = None

        # Setup Label Pointer
        self.mainProcessLabel: tk.Label = None
        self.loadCellValueLabel: tk.Label = None

        # Setup Button Pointer
        self.mainProcessButton: tk.Button = None

        # Setup Root GUI
        self.win = tk.Tk()

    def start(self):
        self.__setupAndStartUI()

    def __processStateMessage(self, message: str):
        stateInt = -1
        loadCellValue = ""

        if "." in message:
            loadCellValue = message[:-1]
            self.loadCellValues.append(loadCellValue)
            self.__updateUI()
            return
        else:
            stateInt = int(message)

        if stateInt == 6: # If process state is Picture:
            self.__takeAndShowPicture()
        
        if stateInt == 8:
            pass #RESET STATE

        match stateInt:
            case 0:
                self.processState = ProcessState.IDLE
            case 1:
                self.processState = ProcessState.LINEAR_ACTUATOR_DOWN
            case 2:
                self.processState = ProcessState.LINEAR_ACTUATOR_RUNNING
            case 3:
                self.processState = ProcessState.LINEAR_ACTUATOR_STOPPING
            case 4:
                self.processState = ProcessState.LINEAR_ACTUATOR_UP
            case 5:
                self.processState = ProcessState.MOTOR_MOVING_TO_CAM
            case 6:
                self.processState = ProcessState.PICTURE_TAKING
            case 7:
                self.processState = ProcessState.PICTURE_TAKING_DONE
            case 8:
                self.processState = ProcessState.RESETTING
            case 9:
                self.processState = ProcessState.HALTED
            case _:
                self.processState = ProcessState.HALTED
        self.__updateUI()

    # Functions
    def __destroyWindow(self):
        self.so.closePort()
        self.cam.release()
        self.win.destroy()

    def __runProcess(self):
        if self.isTesting:
            self.__runCameraSimulation()
        else:
            self.so.serialConnector.write(b's')
        self.__updateUI()

    def __getLoadCellValue(self):
        if len(self.loadCellValues) == 0:
            return "Ölçüm yok."
        else:
            return f"Yük hücresi değeri: {self.loadCellValues[-1]}"
        
    def __takeAndShowPicture(self):
        # Take Image
        result, image = self.cam.read()

        # If taking image is successful, then save
        if result:
            imwrite("image.png", image)
        else:
            print("image basilamadi") 
        
        # Load Image
        self.loadedImage = ImageTk.PhotoImage(Image.open("image.png"))

        # Show Image
        self.testedImage = tk.Label(self.win, image=self.loadedImage)
        self.testedImage.grid(column=1, row=2)
        self.__updateUI()

        # Return signal to Ardunio
        self.so.serialConnector.write(b'c')

    def __updateUI(self):
        self.mainProcessButton.config(text=getProcessButtonMessage(self.processState))
        self.mainProcessLabel.config(text=getProcessMessage(self.processState))
        self.loadCellValueLabel.config(text=self.__getLoadCellValue())
        if self.processState == ProcessState.RESETTING:
            self.testedImage.destroy()

    def __setupAndStartUI(self):
        self.win.title("Scratch Tester")
        self.win.minsize(200,60)

        # Label widget
        self.mainProcessLabel = tk.Label(self.win, text=self.__getLoadCellValue())
        self.mainProcessLabel.grid(column=1, row=3)

        self.loadCellValueLabel = tk.Label(self.win, text=getProcessMessage(self.processState))
        self.loadCellValueLabel.grid(column=1, row=1)

        # Button widget2
        self.mainProcessButton = tk.Button(self.win, bd=4, text=getProcessButtonMessage(self.processState), command=self.__runProcess)
        self.mainProcessButton.grid(column=1, row=4)

        # start & open window continuously
        self.win.protocol('WM_DELETE_WINDOW', self.__destroyWindow)
        self.win.mainloop()

    def __runCameraSimulation(self):
        self.current_test_state += 1
        self.__processStateMessage(message=str(self.current_test_state))
        if self.current_test_state < 5: #TODO: CHANGE STATE
            self.win.after(2000, self.__runCameraSimulation)
        if self.current_test_state == 5:
            self.current_test_state = 0
            
if __name__ == "__main__":
    lele = ScratchTester()
    lele.start()

