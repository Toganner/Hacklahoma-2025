# Define libraries
import time
import serial
from web3 import Web3
import hashlib
from datetime import datetime
import pandas as pd

# print(serial.__file__)

'''
--------------------------------------------------
Arduino Serial Port Dat Extraction
--------------------------------------------------
'''

# Read serial port for Arduino output of NFC ID
serial_port = 'dev/tty.usbmodem112401'
baud_rate = 9600
arduinoData = serial.Serial(serial_port, baud_rate)
time.sleep(2)
NFC_read = False

while True:
	while (arduinoData.inWaiting() == 0):
		pass
	dataPacket = arduinoData.readline()
	dataPacket = str(dataPacket, 'utf-8')
	dataPacket = dataPacket.strip('\r\n')	# Note, println must be used at the end of each commented list
	# splitPacket = dataPacket.split(",")	# Only needed if have comma seperated lists (which the code does not)
	print(dataPacket)

# Determine if NFC is in READ or WRITE state
if (dataPacket == 'LED On'):
	NFC_read = True
else:
	NFC_read = False

# Reassign dataPacket value to numerical ID for debugging
if (NFC_read == True):
	append_value = '110689123'

else:
	append_value = 0

print(append_value)

'''
# --------------------------------------------------
# Excel Access and Manipulation
# --------------------------------------------------
# '''

# # Excel access and manipulation
# file_path = '/Users/kreynolds/Desktop/Hacklahoma 2025/ Automated_Attendance_Tracker.xlsx'

# event_title = input("Enter the event title you would like to track attendance of: ").strip()



# if os.path.exists(file_path):	# Check if the file exists
# 	df_existing = pd.read_excel('/Users/kreynolds/Desktop/Hacklahoma 2025/ Automated_Attendance_Tracker.xlsx');	# Read file



# 	search_value = '110689123'

# 	matching_row = df[df['Column2'] == search_value]	# Check if the value exists
# 	if not matching_row.empty:
# 		correlating_value = matching_row['Column']
# else:
# 	print("File not found!")

# # Connect to Ethereum
# infura_project_id = "e1778b660af24c23a45f2b4e51435e16"
# web3 = Web3(Web3.HTTPProvider(f"https://sepolia.infura.io/v3/{e1778b660af24c23a45f2b4e51435e16}"))

# if web3.isConnected():	# Ensure connection is successful
# 	print("Connected to Ethereum blockchain")
# else:
# 	print("Connection failed")


# # Generate POAP Proof
# def generate_proof(uid):
# 	timestamp = datetime.now().isoformat()	# Get current timestamp
# 	proof = hashlib.sha256(f"{uid}{timestamp}".encode()).hexdigest()	# Create SHA-256 hash
# 	return proof


# # Mint the POAP NFT
# POAP_contract_address = 