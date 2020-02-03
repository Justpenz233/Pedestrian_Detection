import cv2 as cv

toleranceRange = 200
toleranceCount = 5


def calc_area(rect1, rect2):
    xl1, yb1, xr1, yt1 = rect1
    xl2, yb2, xr2, yt2 = rect2
    xr1 += xl1
    yt1 += yb1
    xr2 += xl2
    yt2 += yb2
    xmin = max(xl1, xl2)
    ymin = max(yb1, yb2)
    xmax = min(xr1, xr2)
    ymax = min(yt1, yt2)
    width = xmax - xmin
    height = ymax - ymin
    if width <= 0 or height <= 0:
        return 0
    cross_square = width * height
    return cross_square


def toList(R: tuple):
    result = []
    for i in R:
        result.append(i)
    return result


def RectContin(r1, r2):
    r1 = toList(r1)
    r2 = toList(r2)

    R1Top = r1[0] + r1[2]
    R1Right = r1[1] + r1[3]

    R2Top = r2[0] + r2[2]
    R2Right = r2[1] + r2[3]

    if not R1Top > R2Top:
        return False


def CalcCenter(Rect):
    """

    :type Rect: tuple
    """
    x = int(Rect[0] + Rect[2] * 0.5)
    y = int(Rect[1] + Rect[3] * 0.5)
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
    return "/Users/makik/Desktop/untitled1/" + FileName


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
        NextSecond(cap, 0.05, FPS)
        rects, wei = hog.detectMultiScale(frame, winStride=(4, 4), padding=(8, 8), scale=1.03)

        tR = []
        for i in rects:
            tR.append(i)
        rects = tR

        for tP in Pedestrian:
            track_ok, bbox = tP.tracker.update(frame)
            if not track_ok:
                tP.fault = tP.fault + 1
                if tP.fault > toleranceCount:
                    Pedestrian.remove(tP)
            else:
                tP.setPos(bbox)

        for tP in Pedestrian:
            pos = tP.Center
            for index, dC in enumerate(rects):
                if dC[0] - toleranceRange < pos[0] < dC[0] + dC[2] + toleranceRange \
                        and dC[1] - toleranceRange < pos[1] < dC[1] + dC[3] + toleranceRange:
                    rects.pop(index)

        for R in rects:
            tR = tuple(R)
            Pedestrian.append(People(tR, frame))

        for tP in Pedestrian:
            (x, y, w, h) = tP.Rect
            x = int(x)
            y = int(y)
            w = int(w)
            h = int(h)
            cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)
            cv.circle(frame, tP.Center, 3, (0, 0, 0), -1)

        # for (x, y, w, h) in rects:
        #     cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)

        cv.imshow("FRAME", frame)
        if cv.waitKey(1) & 0xff == 27:
            break
