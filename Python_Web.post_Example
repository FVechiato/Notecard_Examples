
import binascii
import json
import os
import time
import sys
import notecard
import serial  # noqa: E402


def NotecardExceptionInfo(exception):
    """Construct a formatted Exception string.

    Args:
        exception (Exception): An exception object.

    Returns:
        string: a summary of the exception with line number and details.
    """
    s1 = '{}'.format(sys.exc_info()[-1].tb_lineno)
    s2 = exception.__class__.__name__
    return "line " + s1 + ": " + s2 + ": " + ' '.join(map(str, exception.args))


def configure_notecard(card):
    """Submit a simple JSON-based request to the Notecard.

    Args:
        card (object): An instance of the Notecard class

    """
    req = {"req": "hub.set"}
    req["product"] = productUID
    req["mode"] = "continuous"

    try:
        card.Transaction(req)
    except Exception as exception:
        print("Transaction error: " + NotecardExceptionInfo(exception))
        time.sleep(5)
  

def sendWebPost(body, chunkSize):

    data = json.dumps(body)
    total = len(data)
    if(total>chunkSize):
        loopcount = 0
        offset = 0
        Nofchunks = total // chunkSize + 1
        print("Sending " +str(total) + " bytes in " + str(Nofchunks) +" fragments of up to " + str(chunkSize) + "bytes")
        while offset < total:
            end = offset+chunkSize
            if end>total:
                end = total
            a = data[offset:end:1]
            reqWebPost['payload'] = str(binascii.b2a_base64(bytes(a, encoding='utf-8'))[:-1], 'utf-8')
            reqWebPost['offset'] = offset
            reqWebPost['total'] = total
            print(reqWebPost)
            rsp = card.Transaction(reqWebPost)
            loopcount = loopcount + 1
            print(rsp)
            print("Sending fragment #" + str(loopcount) + " of " + str(Nofchunks) + " --- " + str(offset) + " - " + str(end) +"bytes of " + str(total))
            offset = end
    else:
        print("Sending " +str(total) + " bytes")
        a = data
        print(data)
        noteAdd['body'] = body
        print(noteAdd)
        rsp = card.Transaction(noteAdd)
        print(rsp)


def sendJsonFile(dataSize, chunksize):

    data = {"testWebPost":1,"dataSize":dataSize,"chunksize":chunksize, "largefile":""}
    imageSize = dataSize
    imageData = bytes('a'*imageSize, encoding='utf-8') 
    data['largefile'] =str(binascii.b2a_base64(bytes(imageData ))[:-1], 'utf-8') 
    sendWebPost(data, chunksize)




dataSize=200*1024
chunksize=1024
productUID = "Please add your productID here"
#Change "route" to the route name you have created in Notehub
reqWebPost = {"req":"web.post","offset":0,"total":0,"payload":"","route":"ping","content":"text/plain"}
noteAdd = {"req": "note.add","file": "sensors.qo","body":"","sync": True}


# For UART and I2C IO
if sys.implementation.name != 'cpython':
    raise Exception("Please run this example in a CPython environment.")


sys.path.insert(0, os.path.abspath(
                os.path.join(os.path.dirname(__file__), '..')))


"""Connect to Notcard and run a transaction test."""
"""Set your serial port COM port"""

#Change the serial port information according with your OS and port name

print("Opening port...")
try:
    if sys.platform == "linux" or sys.platform == "linux2":
        port = serial.Serial(port="/dev/serial0",
                            baudrate=9600)
    elif sys.platform == "darwin":
        #port = serial.Serial(port="/dev/tty.usbmodemNOTE1",  
        port = serial.Serial(port="/dev/tty.usbserial-AR0JWTXY",                   
                            baudrate=9600)
    elif sys.platform == "win32":
        port = serial.Serial(port="COM21",
                            baudrate=9600)
except Exception as exception:
    raise Exception("error opening port: "
                    + NotecardExceptionInfo(exception))



print("Opening Notecard...")
try:
    card = notecard.OpenSerial(port)
except Exception as exception:
    raise Exception("error opening notecard: "
                    + NotecardExceptionInfo(exception))

req = {"req": "hub.set"}
req["product"] = productUID
req["mode"] = "continuous"

try:
    card.Transaction(req)
except Exception as exception:
    print("Transaction error: " + NotecardExceptionInfo(exception))
    time.sleep(5) 

sendJsonFile(dataSize, chunksize)



