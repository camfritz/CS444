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
vector <int> completed_tasks;
vector <int> waiting_times;
vector <int> turnaround_times;

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
	}

	//find waiting times for all processes
	float waitingSum = 0;
	for(it = processes.begin(); it != processes.end(); it++) {
		if(it == processes.begin()) {
			waiting_times.push_back(0);
		}
		else {
			--it;
			waitingSum += it->second.burstTime;
			++it;
			waiting_times.push_back(waitingSum - it->second.arrivalTime);
		}
	}
	waitingSum = 0;
	for(int i = 0; i < waiting_times.size(); i++) {
		waitingSum += waiting_times[i];
	}
	waitingSum = waitingSum / waiting_times.size();

	//find turnaround times for all processes
	float turnaroundSum = 0;
	int i = 0;
	for(it = processes.begin(); it != processes.end(); it++) {
		turnaround_times.push_back(it->second.burstTime + waiting_times[i]);
		turnaroundSum += turnaround_times[i];
		i++;
	}
	turnaroundSum = turnaroundSum / turnaround_times.size();

//FCFS
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
				completed_tasks.push_back(it->second.PID);
				processes.erase(it);
				break;
			}
		}
		++time_passed;
	}

	cout << "THROUGHPUT: " << completed_tasks.size() << endl;
	cout << "AVERAGE WAIT TIME: " << waitingSum << endl;
	cout << "AVERAGE TURNAROUND TIME: " << turnaroundSum << endl;
	cout << "REMAINING TASKS: " << processes.size() << endl;
}

void SJF() {
	fprintf(stderr, "****SCHEDULING AS SJF****\n");

	multimap <int, Process> SJFMap;
	multimap <int, Process>::iterator it2;
	bool hasScheduled = false;
	//insert processes with PID's, burst times, arrival times
	int PID, arrival_time, cpu_burst;
	while(cin >> PID >> arrival_time >> cpu_burst) {
		Process newProcess;
		newProcess.arrivalTime = arrival_time;
		newProcess.PID = PID;
		newProcess.burstTime = cpu_burst;
		processes.insert(make_pair(arrival_time, newProcess));
	}

//if time passed == arrival time, insert process into SJF map for processing
	int time_passed = 0;
	while(time_passed < sim_time) {
		for(it = processes.begin(); it != processes.end(); it++) {
			if(time_passed == it->second.arrivalTime) {
				SJFMap.insert(make_pair(it->second.burstTime, it->second));
			}
		}

//SJF
		for(it2 = SJFMap.begin(); it2 != SJFMap.end(); it2++) {
			if(hasScheduled == false && it2->second.scheduled == false) {
				if(time_passed != 0) {
					--time_passed;
				}
				fprintf(stdout, "%d: SCHEDULING PID: %d\n", time_passed, it2->second.PID);
				it2->second.scheduled = true;
				hasScheduled = true;
			}
			if(it2->second.scheduled == true) {
				if(it2->second.burstTime > 0) {
					--it2->second.burstTime;
				}
				else {
					fprintf(stdout, "%d: TERMINATING: %d\n", time_passed, it2->second.PID);
					completed_tasks.push_back(it2->second.PID);
					SJFMap.erase(it2);
					hasScheduled = false;
					break;
				}
			}
		}
		++time_passed;
	}

	cout << "THROUGHPUT: " << completed_tasks.size() << endl;
	cout << "REMAINING TASKS: " << SJFMap.size() << endl;
}

void RR() {
	fprintf(stderr, "****SCHEDULING AS RR****\n");

	multimap <int, Process> RRProcesses;
	multimap <int, Process>::iterator it2;
	bool itemsRemaining = false;
	int currentPID;
	int RRcounter = time_slice;
	int PID, arrival_time, cpu_burst;
	while(cin >> PID >> arrival_time >> cpu_burst) {
		Process newProcess;
		newProcess.arrivalTime = arrival_time;
		newProcess.PID = PID;
		newProcess.burstTime = cpu_burst;
		processes.insert(make_pair(arrival_time, newProcess));
	}

//RR
	int time_passed = 0;
	while(time_passed < sim_time) {
		for(it = processes.begin(); it != processes.end(); it++) {
			if(it->second.arrivalTime >= arrival_time && it->second.burstTime > 0) {
				fprintf(stdout, "%d: SCHEDULING: %d\n", time_passed, it->second.PID);
				while(RRcounter > 0) {
					if(it->second.burstTime > 0) {
						--it->second.burstTime;
						--RRcounter;
						++time_passed;
						if(time_passed >= sim_time) {
							break;
						}
					}
					else {
						fprintf(stdout, "%d: TERMINATING: %d\n", time_passed, it->second.PID);
						it->second.scheduled = true;
						break;
					}
				}
				if(it->second.burstTime > 0) {
					fprintf(stdout, "%d: SUSPENDING: %d\n", time_passed, it->second.PID);
				}
				else {
					if(it->second.scheduled == false) {
						fprintf(stdout, "%d: TERMINATING: %d\n", time_passed, it->second.PID);
					}
				}
				if(time_passed >= sim_time) {
					break;
				}
				RRcounter = time_slice;
			}
		}
	}
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