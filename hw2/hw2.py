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
numWaited = 0
#######################################################

def BarberAction(ID):
	barber = Barber()
	mutex.acquire()
	global availableBarberID
	global avg_sleep_time
	barber.IDNumber = availableBarberID
	availableBarberID += 1
	mutex.release()

	while(clientsDone == False):
		#Sleep until notified of a client
		cv.acquire()
		barber.Status = "Sleeping"
		print "barber " + str(barber.IDNumber) + ' ' + barber.Status
		begin = int(round(time.time() * 1000000))
		cv.wait()
		end = int(round(time.time() * 1000000))

		#If clients are done, release CV mutex and exit the thread
		if(clientsDone == True):
			cv.release()
			exit(0)

		#Do Haricut
		barber.Status = "Cutting hair"
		print "barber " + str(barber.IDNumber) + ' ' + barber.Status
		time.sleep(haircut_t / 1000000.0)
		avg_sleep_time += (end - begin)
		cv.release()

def ClientAction(ID):
	global haircuts_successful
	global haircuts_failed
	global avg_wait_time
	global numWaited
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
		client.Status = "Getting haircut"
		print "client " + str(client.IDNumber) + ' ' + client.Status
		cv.release()
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
		begin = int(round(time.time() * 1000000))
		barberSemaphore.acquire()
		chairSemaphore.release()
		end = int(round(time.time() * 1000000))
		cv.acquire()
		cv.notify()
		client.Status = "Getting haircut"
		print "client " + str(client.IDNumber) + ' ' + client.Status
		cv.release()
		time.sleep(haircut_t / 1000000.0)
		barberSemaphore.release()

		mutex.acquire()
		haircuts_successful += 1
		numWaited += 1
		avg_wait_time += (end - begin)
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

cv.acquire()
cv.notify_all()
cv.release()

for i in range(0, num_barbers):
	barberThreads[i].join()

print >> sys.stderr, "Number of haircuts: " + str(haircuts_successful)
print >> sys.stderr, "Number who left: " + str(haircuts_failed)
if(numWaited > 0):
	print >> sys.stderr, "Average client wait time: " + str(avg_wait_time / numWaited)
else:
	print >> sys.stderr, "Average client wait time: 0"
print >> sys.stderr, "Average barber sleep time: " + str(avg_sleep_time / num_barbers)














