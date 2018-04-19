import sys
import threading
import signal
import time
import random

class Barber:
	def __init__(self):
		self.Status = None
		self.IDNumber = None

class Client:
	def __init__(self):
		self.Status = None
		self.IDNumber = None

# class Chair:
# 	def __init__(self):
# 		self.hasClient = False

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

# #Instantiate chair objects
# chairs = [None] * num_chairs
# for i in range (0, len(chairs)):
# 	chairs[i] = Chair()

# #Instantiate client objects
# clients = [None] * num_clients
# for i in range(0, len(clients)):
# 	clients[i] = Client()
# 	clients[i].IDNumber = (i + 1)

# #Instantiate barber objects
# barbers = [None] * num_barbers
# for i in range(0, len(barbers)):
# 	barbers[i] = Barber()
# 	barbers[i].IDNumber = (i + 1)

#######################################################

clientThreads = []
barberThreads = []

barberSemaphore = threading.Semaphore(num_barbers)
chairSemaphore = threading.Semaphore(num_chairs)
mutex = threading.Lock()
availableBarberID = 1
availableClientID = 1
cv = threading.Condition()

def BarberAction(ID):
	barber = Barber()
	mutex.acquire()
	global availableBarberID
	barber.IDNumber = availableBarberID
	barber.Status = "Sleeping"
	availableBarberID += 1
	mutex.release()
	# print "BARBER ID: " + str(barber.IDNumber)

	while(1):
		cv.acquire()
		cv.wait()
		#Do Haricut
		barber.Status = "Cutting hair"
		time.sleep(haircut_t / 1000000.0)
		cv.release()
		barber.Status = "Sleeping"

def ClientAction(ID):
	client = Client()
	mutex.acquire()
	global availableClientID
	client.IDNumber = availableClientID
	client.Status = "Arriving"
	availableClientID += 1
	mutex.release()
	# print "CLIENT ID: " + str(client.IDNumber)

#Start barber threads
for i in range(0, num_barbers):
	t = threading.Thread(target=BarberAction, args=(availableBarberID,))
	barberThreads.append(t)
	t.start()

#Start client threads
for i in range(0, num_clients):
	t = threading.Thread(target=ClientAction, args=(availableClientID,))
	clientThreads.append(t)
	time.sleep(random.randint(1, arrival_t) / 1000000.0)
	t.start()














