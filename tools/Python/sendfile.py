import socket
import sys

HOST = "127.0.0.1"
PORT = 5555

with open(sys.argv[1], "rb") as f:
    data = f.read()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

s.sendall(data)

s.shutdown(socket.SHUT_WR)
s.close()

print("Sent:", len(data), "bytes")