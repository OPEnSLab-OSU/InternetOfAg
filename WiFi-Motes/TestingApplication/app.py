import socket
import time
import sys

UDP_PORT = 2390

sock = socket.socket(socket.AF_INET,
                     socket.SOCK_DGRAM)

sock.bind(('', UDP_PORT))

def main():
    count = 0
    start = time.time()
    total = 0
    while (time.time() - start < 15):
        data, addr = sock.recvfrom(1500)
        if len(data) > 0:
            count += 1
            total += 28 # Account for UDP packet header size
            # total += sys.getsizeof(data) #add data size
            total += len(data)
    print "Total size in bytes: ", total
    print "Data rate: ", float(total)*8/15/(1000000), " Mbps"
    print "Number of packets received: ", count

main()
