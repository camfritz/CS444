#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

int sim_time, time_slice;
char *algorithm;
multimap <int, int> processes;
multimap <int, int>::iterator it;
vector <int> arrival_times;
vector <int> waiting_times;

void FCFS() {
	fprintf(stderr, "CALLING FCFS\n");

//insert processes with PID's, burst times, arrival times
	int PID, arrival_time, cpu_burst;
	while(cin >> PID >> arrival_time >> cpu_burst) {
		processes.insert(make_pair(PID, cpu_burst));
		arrival_times.push_back(arrival_time);
	}

	//find waiting times for all processes
	int waitingSum = 0;
	for(int i = 0; i < processes.size(); i++) {
		if(i == 0) {
			waiting_times.push_back(0);
		}
		else {
			it = processes.find(i);
			waitingSum += it->second;
			waiting_times.push_back(waitingSum - arrival_times[i]);
		}
	}

}

void SJF() {
	fprintf(stderr, "CALLING SJF\n");
}

void RR() {
	fprintf(stderr, "CALLING RR\n");
}

int main(int argc, char **argv) {
	if(argc < 3 || argc > 4) {
		fprintf(stderr, "usage: ./hw4 sim_time algorithm [time_slice]\n");
		exit(1);
	}

	sim_time = atoi(argv[1]);
	algorithm = argv[2];
	if(argc == 4) {
		time_slice = atoi(argv[3]);
	}

	if(strcmp(algorithm, "FCFS") == 0) {
		FCFS();
	}
	else if(strcmp(algorithm, "SJF") == 0) {
		SJF();
	}
	else if(strcmp(algorithm, "RR") == 0) {
		RR();
	}
	else {
		fprintf(stderr, "usage: ./hw4 sim_time algorithm [time_slice]\n");
		exit(1);
	}
}