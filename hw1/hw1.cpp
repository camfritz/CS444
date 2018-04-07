#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <vector>
using namespace std;

int num_prod, num_cons, buf_size, num_items;
pthread_t *consumerThreads, *producerThreads;
sem_t producerSemaphore, consumerSemaphore;
pthread_mutex_t mutex1;

struct Item {
	int itemID;
	int itemSleepTime;
};

vector <Item> itemBuffer;
int availableItemID = 0;

void *produceItem(void *arg) {
	sem_wait(&producerSemaphore);

	usleep(rand() % (700-300 + 1) + 300);
	Item newItem;

	pthread_mutex_lock(&mutex1);
	newItem.itemID = availableItemID;
	++availableItemID;

	newItem.itemSleepTime = rand() % (900-200 + 1) + 200;

	itemBuffer.push_back(newItem);

	pthread_mutex_unlock(&mutex1);
	sem_post(&consumerSemaphore);

}

void *consumeItem(void *arg) {

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

	//allocate consumer and producer threads
	pthread_t *producerThreads = (pthread_t *) malloc(sizeof(pthread_t) * num_prod);
	pthread_t *consumerThreads = (pthread_t *) malloc(sizeof(pthread_t) * num_cons);

	//initialize semaphores and mutex
	sem_init(&producerSemaphore, 0, buf_size);
	sem_init(&consumerSemaphore, 0, 0);
	pthread_mutex_init(&mutex1, NULL);

	int range;
	for(int i = 0; i < num_prod; i++) {
		pthread_create(&producerThreads[i], NULL, produceItem, (void *) range);
	}

	for(int i = 0; i < num_cons; i++) {
		pthread_create(&consumerThreads[i], NULL, consumeItem, (void *) range);
	}

	for(int i = 0; i < num_prod; i++) {
		pthread_join(producerThreads[i], NULL);
	}

	for(int i = 0; i < num_cons; i++) {
		pthread_join(consumerThreads[i], NULL);
	}

	for(int i = 0; i < itemBuffer.size(); i++) {
		printf("ITEM ID: %d\n", itemBuffer[i].itemID);
		printf("ITEM SLEEP TIME: %d\n", itemBuffer[i].itemSleepTime);
	}

}