#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	//variables for number of frames, a single page and frames array
	int page;
	int num_frames;
	int *frames = NULL;
	int pageFaults = 0;

	//error check the cmd line args
	if(argc != 3) {
		fprintf(stderr, "usage: ./hw5 num_frames algorithm\n");
		exit(1);
	}

	//allocate "physical memory" with specified number of frames
	num_frames = atoi(argv[1]);
	frames = (int*) malloc(sizeof(int) * num_frames);

	//initialize all frames to be empty
	for(int i = 0; i < num_frames; i++) {
		frames[i] = -1;
	}

	if(strcmp(argv[2], "FIFO") == 0) {
		int replacementIndex = 0;
		while(cin >> page) {
			fprintf(stdout, "requesting page: %d\n", page);
			//scan through array to check if page is in frames array
			bool pageinMemory = false;
			for(int i = 0; i < num_frames; i++) {
				if(frames[i] == page) {
					pageinMemory = true;
				}
			}

			//if page is not in memory, page fault
			if(!pageinMemory) {
				fprintf(stdout, "PAGE FAULT!\n");
				++pageFaults;
				bool emptySlotAvailable = false;
				int insertionIndex;
				//find an empty slot
				for(int i = 0; i < num_frames; i++) {
					if(frames[i] == -1) {
						emptySlotAvailable = true;
						insertionIndex = i;
						break;
					}
				}

				if(emptySlotAvailable) {
					frames[insertionIndex] = page;
				}
				//if no empty slot, FIFO replace a page
				else {
					frames[replacementIndex % num_frames] = page;
					++replacementIndex;
				}
			}

			//print frames
			fprintf(stdout, "====================\n");
			for(int i = 0; i < num_frames; i++) {
				fprintf(stdout, "%d\n", frames[i]);
			}
			fprintf(stdout, "====================\n");
		}
		fprintf(stderr, "%d page faults occurred\n", pageFaults);
	}
	else if(strcmp(argv[2], "OPT") == 0) {
		vector <int> pageTracker;
		int replacementIndex = 0, trackCounter = 0;
		while(cin >> page) {
			pageTracker.push_back(page);
		}

		for(int j = 0; j < pageTracker.size(); j++) {
			fprintf(stdout, "requesting page: %d\n", pageTracker[j]);
			//scan through array to check if page is in frames array
			bool pageinMemory = false;
			for(int i = 0; i < num_frames; i++) {
				if(frames[i] == pageTracker[i]) {
					pageinMemory = true;
				}
			}

			//if page is not in memory, page fault
			if(!pageinMemory) {
				fprintf(stdout, "PAGE FAULT!\n");
				++pageFaults;
				bool emptySlotAvailable = false;
				int insertionIndex;
				//find an empty slot
				for(int i = 0; i < num_frames; i++) {
					if(frames[i] == -1) {
						emptySlotAvailable = true;
						insertionIndex = i;
						break;
					}
				}

				if(emptySlotAvailable) {
					frames[insertionIndex] = pageTracker[j];
				}
				//if no empty slot, OPT replace a page
				else {
					for(int i = 0; i < num_frames; i++) {
						for(int k = 0; k < pageTracker.size(); k++) {
							if(pageTracker[k] == frames[i]) {
								++trackCounter;
							}
						}
					}
				}
			}

			//print frames
			fprintf(stdout, "====================\n");
			for(int i = 0; i < num_frames; i++) {
				fprintf(stdout, "%d\n", frames[i]);
			}
			fprintf(stdout, "====================\n");
		}
	}
	else if(strcmp(argv[2], "LRU") == 0) {
		//LRU algo
	}
}