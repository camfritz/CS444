
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <vector>
#include <signal.h>
#include <math.h>
using namespace std;

//declare mutexes, semaphores, thread vectors, cmd line integers
int num_prod, num_cons, buf_size, num_items;
vector <pthread_t> producerThreads;
vector <pthread_t> consumerThreads;
sem_t producerSemaphore, consumerSemaphore;
pthread_mutex_t mutex1;

//struct to represent an item.
struct Item {
	int itemID;
	int itemSleepTime;
};

//struct to represent processing segment to split work between threads
struct processingSegment {
	int numToProduce;
};

//declare vectors to store items and processing segments.
vector <processingSegment> segments;
vector <Item> itemBuffer;

//declare ints for initial itemID's and consumer #'s
int availableItemID = 0;
int availableConsumerNumber = 0;

void *produceItem(void *arg) {
	//pass processing segment int, parse number of items for this thread to produce from segment
	struct processingSegment *segment = (processingSegment *) arg;
	int numToProduce = segment->numToProduce;

	while(numToProduce > 0) {
		//randomly sleep, then wait for the semaphore to open
		usleep(rand() % (700-300 + 1) + 300);
		sem_wait(&producerSemaphore);
		Item newItem;
		pthread_mutex_lock(&mutex1);
		//produce a new item with the current available item ID, increment available item ID for the next thread to use
		newItem.itemID = availableItemID;
		++availableItemID;

		//assign random sleep time to item
		newItem.itemSleepTime = rand() % (900-200 + 1) + 200;

		//push item into vector, decrement the counter, unlock mutex and post to the consumer semaphore to signal an item has been produced
		itemBuffer.push_back(newItem);
		pthread_mutex_unlock(&mutex1);
		--numToProduce;
		sem_post(&consumerSemaphore);
	}
}

void *consumeItem(void *arg) {
	//lock mutex, assign this consumer the current available consumer number and increment counter for next consumer to use. Unlock mutex after assignment
	pthread_mutex_lock(&mutex1);
	int currentConsumerNumber = availableConsumerNumber;
	++availableConsumerNumber;
	pthread_mutex_unlock(&mutex1);

	//int for consumer to sleep after consuming item
	int sleepTime;

	while(1) {
		//wait for consumer semaphore to open
		sem_wait(&consumerSemaphore);
		//lock mutex, fetch front item of the vector
		pthread_mutex_lock(&mutex1);
		Item itemToBeConsumed = itemBuffer.front();

		//assign the amount of time the consumer will sleep after consuming the item
		sleepTime = itemToBeConsumed.itemSleepTime;

		//consume the item..erase item from buffer
		itemBuffer.erase(itemBuffer.begin());

		//unlock mutex
		pthread_mutex_unlock(&mutex1);

		fprintf(stdout, "%d: consuming: %d\n", currentConsumerNumber, itemToBeConsumed.itemID);
		fflush(stdout);
		//consumer sleeps for specified amount of time contained in the consumed item, post to producer semaphore to signal space is available to produce an item.
		usleep(sleepTime);
		sem_post(&producerSemaphore);
	}
}

int main(int argc, char **argv) {
	//error check for correct argument counts
	if(argc != 5) {
		printf("Usage: ./hw1 num_prod num_cons buf_size num_items\n");
		exit(1);
	}

	//error check for valid command line argument values
	if(atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || atoi(argv[4]) <= 0) {
		printf("Values must be greater than 0\n");
		exit(1);
	}

	//assign values to global vars
	num_prod = atoi(argv[1]);
	num_cons = atoi(argv[2]);
	buf_size = atoi(argv[3]);
	num_items = atoi(argv[4]);

	//initialize semaphores and mutex
	sem_init(&producerSemaphore, 0, buf_size);
	sem_init(&consumerSemaphore, 0, 0);
	pthread_mutex_init(&mutex1, NULL);

	//create processing segments for thread operations
	int range;
	if((num_items % num_prod) == 0) {
		range = num_items / num_prod;
		for(int i = 0; i < num_prod; i++) {
			processingSegment newSegment;
			newSegment.numToProduce = range;
			segments.push_back(newSegment);
		}
	}
	else {
		range = floor(num_items / num_prod);
		int rangeCounter = num_items;

		for(int i = 0; i < num_prod; i++) {
			if(i < num_prod - 1) {
				rangeCounter -= range;
			}
			else {
				range = rangeCounter;
			}
			processingSegment newSegment;
			newSegment.numToProduce = range;
			segments.push_back(newSegment);
		}
	}
//create threads for producers
	for(int i = 0; i < num_prod; i++) {
		pthread_t p;
		pthread_create(&p, NULL, produceItem, (void *) &segments[i]);
		producerThreads.push_back(p);
	}
//create threads for consumers
	for(int i = 0; i < num_cons; i++) {
		pthread_t p;
		pthread_create(&p, NULL, consumeItem, (void *) range);
		consumerThreads.push_back(p);
	}
//join producer threads
	for(int i = 0; i < num_prod; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	fprintf(stderr, "DONE PRODUCING!!\n");
//wait until SIGINT is recevied
	while(1);
}