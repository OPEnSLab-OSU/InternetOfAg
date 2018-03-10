import socket
import time

UDP_PORT = 9436

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)

sock.bind(('', UDP_PORT))

def main():
    start = time.time()
    total = 0
    while (time.time() - start < 15):
        data, addr = sock.recvfrom(1024)
        if len(data) > 0:
            total += 28 # Account for packet header size
            total += len(data) #add data size
    print "Total size in bytes: ", total
    print "Data rate: ", total*8/15/(1000000), " Mbps"

main()
