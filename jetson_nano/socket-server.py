import socket
import numpy as np
import json
import time
import random
HOST = '192.168.50.89' # IP address for 'aoyi' AP
PORT = 6969 # Port to listen on (use ports > 1023)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.bind((HOST, PORT))
	print("Starting server at: ", (HOST, PORT))
	s.listen()
	print("Listening for client...")
	conn, addr = s.accept()
	with conn:
		print('Connected by', addr)
		while True:
			data = conn.recv(1024).decode('utf-8')
			print(data)
