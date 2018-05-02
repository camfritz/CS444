#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
using namespace std;

struct Process {
	bool scheduled = false;
	int arrivalTime;
	int PID;
	int burstTime;
};

int sim_time, time_slice;
char *algorithm;
multimap <int, Process> processes;
multimap <int, Process>::iterator it;
// vector <int> arrival_times;
// vector <int> waiting_times;
// queue <int> schedule;

void FCFS() {
	fprintf(stderr, "****SCHEDULING AS FCFS****\n");

	//insert processes with PID's, burst times, arrival times
	int PID, arrival_time, cpu_burst;
	while(cin >> PID >> arrival_time >> cpu_burst) {
		Process newProcess;
		newProcess.arrivalTime = arrival_time;
		newProcess.PID = PID;
		newProcess.burstTime = cpu_burst;
		processes.insert(make_pair(arrival_time, newProcess));
		// arrival_times.push_back(arrival_time);
	}

	int time_passed = 0;
	while(time_passed < sim_time) {
		for(it = processes.begin(); it != processes.end(); it++) {
			if(it == processes.begin() && it->second.scheduled == false) {
				if(time_passed != 0) {
					--time_passed;
				}
				fprintf(stdout, "%d: SCHEDULING PID: %d\n", time_passed, it->second.PID);
				it->second.scheduled = true;
			}
			if(it->second.burstTime > 0 && it->second.scheduled == true) {
				--it->second.burstTime;
			}
			else if(it->second.burstTime <= 0 && it->second.scheduled == true) {
				fprintf(stdout, "%d: TERMINATING: %d\n", time_passed, it->second.PID);
				processes.erase(it);
				break;
			}
		}
		++time_passed;
	}
}

	// //find waiting times for all processes
	// int waitingSum = 0;
	// for(int i = 0; i < processes.size(); i++) {
	// 	if(i == 0) {
	// 		waiting_times.push_back(0);
	// 	}
	// 	else {
	// 		it = processes.find(i);
	// 		waitingSum += it->second;
	// 		waiting_times.push_back(waitingSum - arrival_times[i]);
	// 	}
	// }

	// while(sim_time > 0) {
	// 	//check to see if items still have to be scheduled
	// 	if(processes.size() <= 0) {
	// 		fprintf(stdout, "Scheduling finished\n");
	// 		exit(0);
	// 	}

	// 	//if items still have to be scheduled, put the next item into queue

	// }

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