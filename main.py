import cv2 as cv
import numpy
import colorsys
import collections

toleranceRange = 50
toleranceCount = 30


def CalcCenter(Rect):
    """

    :type Rect: tuple
    """
    x = int(Rect[0] + Rect[2] * 0.5)
    y = int(Rect[2] + Rect[3] * 0.5)
    return (x, y)


class People:
    def __init__(self, Rect, frame):
        self.Rect = Rect
        self.preRect = (0, 0, 0, 0)
        self.Center = CalcCenter(Rect)
        self.preCenter = (0, 0)
        self.speed = [0, 0]
        self.tracker = cv.TrackerKCF_create()
        self.tracker.init(frame, Rect)
        self.fault = 0

    def getCenter(self):
        return self.Center

    def getSpeed(self):
        return self.speed

    def setPos(self, Rect):
        self.preRect = self.Rect
        self.preCenter = self.Center
        self.Rect = Rect
        self.Center = CalcCenter(self.Rect)
        self.speed[0] = self.Center[0] - self.preCenter[0]
        self.speed[1] = self.Center[1] - self.preCenter[1]


def mk_path(FileName):
    return "/Users/makik/Desktop/FinalProject/" + FileName


def NextSecond(cap, Second, FPS):
    for i in range(int(FPS * Second)):
        cap.read()


if __name__ == '__main__':

    cv.namedWindow("FRAME")
    cap = cv.VideoCapture(mk_path("demo.mp4"))
    hog = cv.HOGDescriptor()
    hog.setSVMDetector(cv.HOGDescriptor_getDefaultPeopleDetector())

    FPS = cap.get(cv.CAP_PROP_FPS)
    Height = cap.get(cv.CAP_PROP_FRAME_HEIGHT)
    Width = cap.get(cv.CAP_PROP_FRAME_WIDTH)


    SETWidth = 600
    Pedestrian = []
    while True:
        isOpen, frame = cap.read()
        # frame = cv.resize(frame, (SETWidth, int(SETWidth * Height / Width)))
        if not isOpen:
            break
        NextSecond(cap, 0.2, FPS)
        rects, wei = hog.detectMultiScale(frame, winStride=(4, 4), padding=(8, 8), scale=1.03)

        rects = rects.tolist()
        for tP in Pedestrian:
            track_ok, bbox = tP.tracker.update(frame)
            if not track_ok:
                tP.fault = tP.fault + 1
                if tP.fault > toleranceCount:
                    Pedestrian.remove(tP)
            else:
                tP.setPos(bbox)

        for dC in rects:
            for tP in Pedestrian:
                pos = tP.Center
                if dC[0] - toleranceRange < pos[0] < dC[0] + dC[2] + toleranceRange \
                        and dC[1] - toleranceRange < pos[1] < dC[1] + dC[3] + toleranceRange:
                    rects.remove(dC)

        for R in rects:
            tR = tuple(R)
            Pedestrian.append(People(tR, frame))

        for tP in Pedestrian:
            (x, y, w, h) = tP.Rect
            cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

        # for (x, y, w, h) in rects:
        #     cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

        cv.imshow("FRAME", frame)
        if cv.waitKey(1) & 0xff == 27:  # esc键
            break
