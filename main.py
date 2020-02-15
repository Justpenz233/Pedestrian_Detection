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
        # self.tracker = cv.TrackerKCF_create()
        self.tracker = cv.TrackerCSRT_create()
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

DisplayVideo = True
SaveVideo = True
TestMode = False

if __name__ == '__main__':

    if DisplayVideo:
        cv.namedWindow("FRAME")
    cap = cv.VideoCapture("demo.mp4")
    hog = cv.HOGDescriptor()
    hog.setSVMDetector(cv.HOGDescriptor_getDefaultPeopleDetector())



    FPS = cap.get(cv.CAP_PROP_FPS)
    Height = cap.get(cv.CAP_PROP_FRAME_HEIGHT)
    Width = cap.get(cv.CAP_PROP_FRAME_WIDTH)

    if SaveVideo:
        fourcc = cv.VideoWriter_fourcc(*'XVID')  # 保存视频的编码
        out = cv.VideoWriter('output.avi', fourcc, FPS, (int(Width), int(Height)))

    HASDONE = 0

    SETWidth = 600
    Pedestrian = []
    while True:
        HASDONE += 1
        if not DisplayVideo:
            print(HASDONE)
        isOpen, frame = cap.read()
        # frame = cv.resize(frame, (SETWidth, int(SETWidth * Height / Width)))
        if not isOpen:
            break

        if TestMode:
            NextSecond(cap, 0.2, FPS)

        rects, wei = hog.detectMultiScale(frame, winStride=(20, 20), padding=(8, 8), scale=1.005)

        tR = []
        for i in rects:
            tR.append(i)
        rects = tR

        for i in range(len(rects)):
            if i >= len(rects):
                break
            Ri = rects[i]
            Ai = Ri[2] * Ri[3]
            findFlag = True
            while findFlag:
                for j in range(i + 1, len(rects)):
                    Rj = rects[j]
                    Aj = Rj[2] * Rj[3]
                    Area = calc_area(Ri, Rj)
                    if Area > Ai * 0.5 or Area > Aj * 0.5:
                        rects.pop(j)
                        findFlag = True
                        break
                findFlag = False

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
            FindFlag = True
            while FindFlag :
                for index, dC in enumerate(rects):
                    if dC[0] - toleranceRange < pos[0] < dC[0] + dC[2] + toleranceRange \
                            and dC[1] - toleranceRange < pos[1] < dC[1] + dC[3] + toleranceRange:
                        rects.pop(index)
                        findFlag = True
                        break
                FindFlag = False


        for R in rects:
            tR = tuple(R)
            Pedestrian.append(People(tR, frame))

        for i in range(len(Pedestrian)):
            if i >= len(Pedestrian):
                break
            Ri = Pedestrian[i].Rect
            Ai = Ri[2] * Ri[3]
            findFlag = True
            while findFlag:
                for j in range(i + 1, len(Pedestrian)):
                    Rj = Pedestrian[j].Rect
                    Aj = Rj[2] * Rj[3]
                    Area = calc_area(Ri, Rj)
                    if Area > Ai * 0.8 or Area > Aj * 0.8:
                        Pedestrian.pop(j)
                        findFlag = True
                        break
                findFlag = False


        for tP in Pedestrian:
            (x, y, w, h) = tP.Rect
            x = int(x)
            y = int(y)
            w = int(w)
            h = int(h)
            cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)
            cv.circle(frame, tP.Center, 3, (0, 0, 0), -1)
            font = cv.FONT_HERSHEY_SIMPLEX
            messg = "Speed:"
            messg = messg + str(tP.speed[0]) + "," + str(tP.speed[1])
            frame = cv.putText(frame, messg, (x, y), font, 1.2, (255, 255, 255), 2)

        font = cv.FONT_HERSHEY_SIMPLEX
        messg = "Number:" + str(len(Pedestrian))
        frame = cv.putText(frame, messg, (0, 40), font, 1.2, (255, 255, 255), 2)
        # for (x, y, w, h) in rects:
        #     cv.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)


        if DisplayVideo:
            cv.imshow("FRAME", frame)
            if cv.waitKey(1) & 0xff == 27:
                break

        if SaveVideo:
            out.write(frame)
