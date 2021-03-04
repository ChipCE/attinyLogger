LOG_FILE = "log.txt"
EXPORT_FILE = "output.csv"
data = []

def readLog():
    global data
    logFile = open(LOG_FILE, 'r')
    data = logFile.readlines()

 
def exportData():
    dataCount = 0
    timeMillis = 0
    global data
    expFile = open(EXPORT_FILE, 'w')
    expFile.write("time,x,y,z\n")
    for line in data:
        if not "#" in line:
            tmpArr = line.split(",")
            timeMillis = int(tmpArr[0]) + timeMillis
            tmpArr[0] = str(timeMillis)
            expFile.write(tmpArr[0] + "," + tmpArr[1] + "," + tmpArr[2] + "," + tmpArr[3])
            dataCount = dataCount + 1
    expFile.close()
    print("Exported",dataCount,"items")

readLog()
exportData()


 
# https://chart-studio.plotly.com/create/#/