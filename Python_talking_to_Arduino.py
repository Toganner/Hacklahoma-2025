# import os
import time
import serial
import pandas as pd
from web3 import Web3

# Change 'COM3' to the correct port for your Arduino
ser = serial.Serial('COM3', 9600)
time.sleep(5)

def is_valid_nine_digit_string(s):
    return len(s) == 9 and s.isdigit()

def write_to_excel(user_id):
        # Define Excel file path
    # file_path = r'Book1.xlsx'  # Saves in current folder
    file_path = r"C:\Users\Owner\Documents\Arduino\Attendance_Scanner\Attendance.xlsx"

    # Check if file exists
    try:
        df = pd.read_excel(file_path)
        print(f"Opened existing workbook: {file_path}")
    except FileNotFoundError:
        print(f"Creating new workbook: {file_path}")
        df = pd.DataFrame(columns=["User_ID", "Student_Attendance", "Ethereum_Account"])
        df.to_excel(file_path, index=False)

    # Ensure 'User_ID' column is a string
    df["User_ID"] = df["User_ID"].astype(str)

    ethereum_account = ""
    found = False

    # Find the row with the matching User_ID and increment Student_Attendance
    for index, row in df.iterrows():
        if row["User_ID"] == user_id:
            df.at[index, "Student_Attendance"] = int(row["Student_Attendance"] or 0) + 1
            ethereum_account = row["Ethereum_Account"] if pd.notna(row["Ethereum_Account"]) else "N/A"
            print(f"Ethereum Account: {ethereum_account}")
            print(f"Updated Student_Attendance for User_ID {user_id}")
            found = True
            break

    if not found:
        print(f"User_ID {user_id} not found in the Excel file.")

    # Save the updated Excel file
    df.to_excel(file_path, index=False)
    print(f"Excel file {file_path} updated successfully.")

    ganache_url = "http://127.0.0.1:7545/"  # Update if needed
    w3 = Web3(Web3.HTTPProvider(ganache_url))

    sender_address = "0xaE1AE1Ad2B2784C2b0C72a37e062f023A2B4C865"
    private_key = "0x9f2b812bcc4d80ca79c33f2bdd55f190035cb23d87c0370cd2f65a3048395f15"

    def send_eth(receiver_address):
        nonce = w3.eth.get_transaction_count(sender_address)
        txn = {
            'to': receiver_address,
            'value': Web3.to_wei(0.01, 'ether'),  # Amount to send
            'gas': 21000,
            'gasPrice': Web3.to_wei('10', 'gwei'),
            'nonce': nonce,
            'chainId': 1337  # Ganache's Chain ID
        }

        signed_txn = w3.eth.account.sign_transaction(txn, private_key)
        tx_hash = w3.eth.send_raw_transaction(signed_txn.raw_transaction)
        return w3.to_hex(tx_hash)
    
    send_eth(ethereum_account)


loopRun = 0

# def main():
while True:
    # Wait for the User ID from the Arduino
    if ser.in_waiting > 0 and loopRun!=0:
        user_id = ser.readline().decode('utf-8').strip()

        if user_id == "Read Mode":
            print("Reading")
            user_id = ser.readline().decode('utf-8').strip()
            if is_valid_nine_digit_string(user_id):
                print(f"User ID: {user_id}")
                write_to_excel(user_id)
        if user_id == "Write Mode":    
            print("Writing")
            user_input = input("Enter message for Arduino (or 'exit' to quit): ")
            if user_input.lower() == 'exit':
                print("Closing connection...")
                break
            ser.write(user_input.encode())  # Send input to Arduino
            print(f"Sent: {user_input}")
    loopRun+=1
ser.close()


