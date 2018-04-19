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

clientThreads = []
barberThreads = []

barberSemaphore = threading.Semaphore(num_barbers)
chairSemaphore = threading.Semaphore(num_chairs)
mutex = threading.Lock()
availableBarberID = 1
availableClientID = 1
cv = threading.Condition()
clientsDone = False

haircuts_successful = 0
avg_sleep_time = 0
haircuts_failed = 0
avg_wait_time = 0
#######################################################

def BarberAction(ID):
	barber = Barber()
	mutex.acquire()
	global availableBarberID
	barber.IDNumber = availableBarberID
	availableBarberID += 1
	mutex.release()

	while(clientsDone == False):
		cv.acquire()
		barber.Status = "Sleeping"
		print "barber " + str(barber.IDNumber) + ' ' + barber.Status
		cv.wait()
		#Do Haricut
		barber.Status = "Cutting hair"
		print "barber " + str(barber.IDNumber) + ' ' + barber.Status
		time.sleep(haircut_t / 1000000.0)
		cv.release()

def ClientAction(ID):
	global haircuts_successful
	global haircuts_failed
	client = Client()
	mutex.acquire()
	global availableClientID
	client.IDNumber = availableClientID
	client.Status = "Arriving"
	print "client " + str(client.IDNumber) + ' ' + client.Status
	availableClientID += 1
	mutex.release()

	#Check if barber is available (trywait)
	if(barberSemaphore.acquire(False)):
		#If barber available, decrement barber semaphore, notify CV. increment once done. Exit
		cv.acquire()
		cv.notify()
		cv.release()
		client.Status = "Getting haircut"
		print "client " + str(client.IDNumber) + ' ' + client.Status
		time.sleep(haircut_t / 1000000.0)
		barberSemaphore.release()

		mutex.acquire()
		haircuts_successful += 1
		mutex.release()

		exit(0)

	#If no barber, check chair semaphore (trywait)
	elif(chairSemaphore.acquire(False)):
		#If chair available, wait on barber, notify CV, increment barber sem. Exit
		client.Status = "Waiting"
		print "client " + str(client.IDNumber) + ' ' + client.Status
		barberSemaphore.acquire()
		chairSemaphore.release()

		cv.acquire()
		cv.notify()
		cv.release()

		client.Status = "Getting haircut"
		time.sleep(haircut_t / 1000000.0)
		print "client " + str(client.IDNumber) + ' ' + client.Status
		barberSemaphore.release()

		mutex.acquire()
		haircuts_successful += 1
		mutex.release()

		exit(0)

	#Else, exit
	else:
		client.Status = "Leaving (Did not get haircut)"
		print "client " + str(client.IDNumber) + ' ' + client.Status

		mutex.acquire()
		haircuts_failed += 1
		mutex.release()
		exit(0)


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

for i in range(0, num_clients):
	clientThreads[i].join()

clientsDone = True
print "CLIENTS ARE DONE!!!"

cv.acquire()
cv.notify_all()
cv.release()

for i in range(0, num_barbers):
	barberThreads[i].join()

print "SUCCESSFUL HAIRCUTS: " + str(haircuts_successful)
print "FAILED HAIRCUTS: " + str(haircuts_failed)














