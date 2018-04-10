
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

int num_prod, num_cons, buf_size, num_items;
vector <pthread_t> producerThreads;
vector <pthread_t> consumerThreads;
sem_t *producerSemaphore, *consumerSemaphore;
pthread_mutex_t mutex1;

struct Item {
	int itemID;
	int itemSleepTime;
};

struct processingSegment {
	int numToProduce;
};

vector <processingSegment> segments;
vector <Item> itemBuffer;
int availableItemID = 0;
int availableConsumerNumber = 0;

void *produceItem(void *arg) {
	struct processingSegment *segment = (processingSegment *) arg;
	int numToProduce = segment->numToProduce;
	while(numToProduce > 0) {
		usleep(rand() % (700-300 + 1) + 300);
		sem_wait(producerSemaphore);
		// fprintf(stderr, "PRODUCER LOCKING\n");
		pthread_mutex_lock(&mutex1);
		// fprintf(stderr, "****PRODUCER LOCKED****\n");
		Item newItem;
		newItem.itemID = availableItemID;
		++availableItemID;

		newItem.itemSleepTime = rand() % (900-200 + 1) + 200;
		// fprintf(stderr, "PRODUCER SLEEPING\n");
		itemBuffer.push_back(newItem);
		// fprintf(stderr, "ITEM PRODUCED\n");
		--numToProduce;
		// fprintf(stderr, "PRODUCER UNLOCKING\n");
		pthread_mutex_unlock(&mutex1);
		sem_post(consumerSemaphore);
	}
}

void *consumeItem(void *arg) {
	pthread_mutex_lock(&mutex1);
	int currentConsumerNumber = availableConsumerNumber;
	++availableConsumerNumber;
	pthread_mutex_unlock(&mutex1);
	int sleepTime;

	while(1) {
		sem_wait(consumerSemaphore);
		// fprintf(stderr, "CONSUMER LOCKING\n");
		pthread_mutex_lock(&mutex1);
		// fprintf(stderr, "****CONSUMER LOCKED****\n");
		Item itemToBeConsumed = itemBuffer.front();
		sleepTime = itemToBeConsumed.itemSleepTime;
		// fprintf(stderr, "CONSUMER SLEEPING\n");
		fprintf(stdout, "%d: consuming: %d\n", currentConsumerNumber, itemToBeConsumed.itemID);
		itemBuffer.erase(itemBuffer.begin());
		// fprintf(stderr, "CONSUMER UNLOCKING\n");
		pthread_mutex_unlock(&mutex1);
		sem_post(producerSemaphore);
		usleep(sleepTime);
	}
}

void signalHandler(int sig) {
	sem_close(producerSemaphore);
	sem_close(consumerSemaphore);

	sem_unlink("/producerSemaphore");
	sem_unlink("/consumerSemaphore");
	exit(0);
}

int main(int argc, char **argv) {
	signal(SIGINT, signalHandler);
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
	producerSemaphore = sem_open("/producerSemaphore", O_CREAT, 0777, buf_size);
	consumerSemaphore = sem_open("/consumerSemaphore", O_CREAT, 0777, 0);
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

	for(int i = 0; i < num_prod; i++) {
		pthread_t p;
		pthread_create(&p, NULL, produceItem, (void *) &segments[i]);
		producerThreads.push_back(p);
	}

	for(int i = 0; i < num_cons; i++) {
		pthread_t p;
		pthread_create(&p, NULL, consumeItem, (void *) range);
		consumerThreads.push_back(p);
	}

	for(int i = 0; i < num_prod; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	fprintf(stderr, "DONE PRODUCING!!\n");

	while(1);
}