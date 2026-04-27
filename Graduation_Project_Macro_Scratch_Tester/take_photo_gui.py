import tkinter as tk
from PIL import ImageTk, Image
from cv2 import VideoCapture, imwrite

class ScratchTester():
    def __init__(self):
        self.loadedImage: ImageTk.PhotoImage = None

        # Setup Cam Connection
        cam_port = 1 #USB'YE GÖRE DEĞİŞTİR
        self.cam = VideoCapture(cam_port)

        # Setup Image Pointer
        self.testedImage: tk.Label = None

        # Setup Button Pointer
        self.mainProcessButton: tk.Button = None

        # Setup Root GUI
        self.win = tk.Tk()

    def start(self):
        self.__setupAndStartUI()

    # Functions
    def __destroyWindow(self):
        self.cam.release()
        self.win.destroy()
        
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
        self.testedImage.grid(column=1, row=1)

    def __setupAndStartUI(self):
        self.win.title("Scratch Tester")
        self.win.minsize(200,60)

        self.mainProcessButton = tk.Button(self.win, bd=4, text="Resim Çek", command=self.__takeAndShowPicture)
        self.mainProcessButton.grid(column=1, row=2)

        # start & open window continuously
        self.win.protocol('WM_DELETE_WINDOW', self.__destroyWindow)
        self.win.mainloop()
            
if __name__ == "__main__":
    lele = ScratchTester()
    lele.start()

