import pandas as pd
from web3 import Web3


ganache_url = "http://127.0.0.1:7545/"  # Update if needed
w3 = Web3(Web3.HTTPProvider(ganache_url))

sender_address = "0xCbc882DFcda0fB7c3d218cD27eeDFFC305A34609"
private_key = "0x30d3f97e41748f1c985a74a0b7d26e9d9183fbf0b52fd826ad104474979486ea"

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

print(send_eth('0x43536F2F306254671f8d633fB8e4Bf6dE9717610'))

