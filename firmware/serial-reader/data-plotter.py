import json
import requests

dataSet = []
sampleInMin = 10
lastCounter = -1
queryUrl = "http://172.17.198.12:8000/CKDPlatform.io/api/v2/record/mcu/?user=test&maxNum=1000"

def requestData(toId):
    if toId > 0:
        try:
            url = queryUrl + "&filterTidFromTo=," + str(toId-1)
            print("\nGet : ",url)
            response = requests.get(url) 
            return response.json()
        except Exception as e:
            print(e)
            return None
    elif toId == -1:
        try:
            print("Get : ",queryUrl)
            response = requests.get(queryUrl) 
            return response.json()
        except Exception as e:
            print(e)
            return None
    else:
        return None



def analyze(jsonData):
    if jsonData == None:
        print("Invalid resonse json, break!")
        return None

    global lastCounter

    print("Data count :",len(jsonData['with']))
    print("TransactionId range:",jsonData['with'][len(jsonData['with'])-1]['transaction'],"~",jsonData['with'][0]['transaction'])
    print("Counter range:",jsonData['with'][len(jsonData['with'])-1]['content']['counter'],"~",jsonData['with'][0]['content']['counter'])
    if int(jsonData['with'][0]['transaction']) < int(jsonData['with'][len(jsonData['with'])-1]['transaction']):
        print("Err : reversed order!")
        quit()

    for dataObj in jsonData['with']:
        if lastCounter != dataObj['content']['counter']:
            if lastCounter < dataObj['content']['counter'] and lastCounter != -1:
                # print(lastCounter,dataObj['content']['counter'])
                print("New data set @",dataObj['transaction']," , break!")
                return None

            lastCounter = dataObj['content']['counter']
            # print("Dataset:",dataObj['content']['counter'],dataObj['content']['voltage'])
            dataSetStr = str(dataObj['content']['counter']) + "," + str(dataObj['content']['voltage'])+"\n"
            dataSet.append(dataSetStr)
    return int(jsonData['with'][len(jsonData['with'])-1]['transaction'])

def writeResult():
    global sampleInMin
    dataSet.reverse()
    sampleData = []
    for i in range(0,len(dataSet),sampleInMin):
        sampleData.append(dataSet[i])
    with open("output.csv", "w") as resultFile:
        resultFile.writelines(sampleData)
        print("File exported : output.csv!")



# get first data
res = requestData(-1)
nextId = analyze(res)
# then loop
while nextId != None:
    res = requestData(nextId)
    nextId = analyze(res)
writeResult()
