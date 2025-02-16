import time
timport serial
arduinoData = serial.Serial('com3', 9600)
time.sleep(1)
while True:
	while (arduinoData.inWaiting() == 0):
		pass
	dataPacket = arduinoData.readline()
	dataPacket = str(dataPacket, 'utf-8')
	dataPacket = dataPacket.strip('\r\n')	# Note, println must be used at the end of each commented list
	# splitPacket = dataPacket.split(",")	# Only needed if have comma semperated lists (which the code does not)
	
	print(dataPacket)