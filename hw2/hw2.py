import sys
import threading
import signal

class Barber:
	def __init__(self):
		self.isSleeping = True

class Client:
	def __init__(self):
		self.isGettingHairCut = False
		self.hairHasBeenCut = False

class Chair:
	def __init__(self):
		self.hasClient = False

#Do some command line argument error checking
if(len(sys.argv) < 6):
	print ("usage: python hw2.py num_barbers num_clients num_chairs arrival_t haircut_t")
	sys.exit(1)

try:
	num_barbers = int(sys.argv[1])
	num_clients = int(sys.argv[2])
	num_chairs = int(sys.argv[3])
	arrival_t = int(sys.argv[4])
	haircut_t = int(sys.argv[5])

except:
	print ("ERROR: Arguments must be integers")
	sys.exit(1)

if(num_barbers <= 0 or num_clients <= 0 or num_chairs <= 0 or arrival_t <= 0 or haircut_t <= 0):
	print ("ERROR: Arguments must be greater than 0")
	sys.exit(1)

#######################################################

#Instantiate chair objects
chairs = [None] * num_chairs
for i in range (0, len(chairs)):
	chairs[i] = Chair()

#Instantiate client objects
clients = [None] * num_clients
for i in range(0, len(clients)):
	clients[i] = Client()

#Instantiate barber objects
barbers = [None] * num_barbers
for i in range(0, len(barbers)):
	barbers[i] = Barber()

#######################################################

threads = []
mutex = threading.Lock()

