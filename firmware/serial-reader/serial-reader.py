import serial
import requests
import json
import datetime
import multiprocessing

PORT = "/dev/ttyACM0"
BAUDRATE = 115200
POST_URL = "https://ptsv2.com/t/ua2gh-1621923584/post"



class DataPool:
    def __init__(self):
        self.queue = multiprocessing.Queue()
    def get_queue(self):
        return self.queue

def postHandle(dataPool):
    print("Post handle started!")
    
    while True:
        if dataPool.qsize() > 0:
            try:
                jsonObj = json.loads(dataPool.get())
                print("POST",jsonObj)
                dataTemplate ={
                    "user" : "mcu",
                    "thing" : "mcu-test",
                    "time" : str(datetime.datetime.now().isoformat()),
                    "content" : jsonObj
                }
                print(dataTemplate)
                try:
                    postReq = requests.post(POST_URL, json=dataTemplate)
                    print(postReq.status_code, postReq.reason)
                except Exception as e:
                    print("Post failed :",e)
            except Exception as e:
                    print("Invalid json :",e)

def serialHandle(dataPool):
    print("Serial handle started!")
    ser = serial.Serial(
    port=PORT,\
    baudrate=BAUDRATE,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
        timeout=0)

    print("connected to: " + ser.portstr)

    #this will store the line
    line = []

    while True:
        for c in ser.read():
            serialChar = chr(c)
            line.append(serialChar)
            if serialChar == '\n':
                # join chars and remove the \r\n at the end of string
                serialLine = ''.join(line)[:-2]
                line = []
                print("RECV",serialLine)
                dataPool.put(serialLine)
                break
    ser.close()

def main():
    pool = DataPool()
    serialThread = multiprocessing.Process(target=serialHandle,args=(pool.get_queue(),))
    postThread = multiprocessing.Process(target=postHandle,args=(pool.get_queue(),))
    serialThread.start()
    postThread.start()

    serialThread.join()
    postThread.join()

    

if __name__ == "__main__":
    main()