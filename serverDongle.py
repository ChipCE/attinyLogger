import serial
import requests
import json
import datetime

class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            #else:

serialReader = serial.Serial('/dev/ttyUSB0', 115200)
pipe = ReadLine(serialReader)


def getUrl(data,baseUrl):
    if data.type == "accel":
        return baseUrl + "/accel?stamp=" + str(datetime.datetime.now().isoformat())

    if data.type == "interval":
        return baseUrl + "/interval?stamp=" + str(datetime.datetime.now().isoformat())

    if data.type == "random":
        return baseUrl + "random"

    # if data.type == "":
    #     return baseUrl + ""


while True:
    try:
        data = pipe.readline()
        print(data)
    except Exception as e:
        print("Serial error" + e)
    
    try:
        jsonData = json.loads(data)
        try:
            postReq = requests.post("https://ptsv2.com/t/a7xnz-1621833597/post", data=jsonData)
            print(postReq.status_code, postReq.reason)
        except Exception as e:
            print("Post error " + e)
    except Exception as e:
        print("Data format error" + e)