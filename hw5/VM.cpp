#include "VM.h"

VM::VM() : sram(SRAM, HOLD) {
	//intialize refs and page fault counters
	numberofMemoryReferences = 0;
	numberofPageFaults = 0;
	//initialize the page table with empty slots
	for(int i = 0; i < TABLE_SIZE; i++) {
		pageTable[i] = -1;
	}
	nextVictimPage = 0;
}

double VM::getFaultRate() {
	return (double) numberofPageFaults / (double) numberofMemoryReferences;
}

void VM::resetFaultRate() {
	numberofMemoryReferences = 0;
	numberofPageFaults = 0;
}

bool VM::verifyPageTable(unsigned long number) {
	for(int i = 0; i < TABLE_SIZE; i++) {
		currentPageIndex = i;
		if(pageTable[i] == number) {
			return true;
		}
	}
	return false;
}

void VM::raisePageFault(unsigned long number) {
	++numberofPageFaults;
	bool emptySlotAvailable = false;
	int emptySlotNumber;
	//search for empty slot in page table
	for(int i = 0; i < TABLE_SIZE; i++) {
		if(pageTable[i] == -1) {
			emptySlotAvailable = true;
			emptySlotNumber = i;
			break;
		}
	}
	//if empty slot available, retrieve page from backing store
	if(emptySlotAvailable) {
		if(VERBOSE == 1) {
			Serial.println("PAGE TABLE SLOT AVAILABLE");
			Serial.print("Empty slot number: ");
			Serial.print(emptySlotNumber);
			Serial.print('\n');
			Serial.print("Reading SRAM memory page: ");
			Serial.print(number);
			Serial.print('\n');
		}
		sram.read_page(number, &physicalMemory[(32 * emptySlotNumber)]);
		//update page table
		pageTable[emptySlotNumber] = number;
		currentPageIndex = emptySlotNumber;
	}
	//if no empty slot available, identify victim page
	else {
		if(VERBOSE == 1) {
			Serial.println("NO SLOTS AVAILABLE IN PAGE TABLE");
			Serial.print("PAGE IN: ");
			Serial.print(number);
			Serial.print("\nPAGE OUT: ");
			Serial.print(pageTable[nextVictimPage]);
			Serial.print('\n');
		}
		//write victim page to backing store
		sram.write_page(pageTable[nextVictimPage], &physicalMemory[32 * nextVictimPage]);
		//retrieve new page from backing store
		sram.read_page(number, &physicalMemory[(32 * nextVictimPage)]);
		//update page table
		pageTable[nextVictimPage] = number;
		currentPageIndex = nextVictimPage;
		//update next victim page
		nextVictimPage = (nextVictimPage + 1) % TABLE_SIZE;
	}
}

byte &VM::operator[] (unsigned long index)
{
	//increment refs counter
	++numberofMemoryReferences;
	if(VERBOSE == 1){
		Serial.println("====================");
		Serial.print("REQUESTING VIRTUAL ADDRESS: ");
		Serial.print(index);
		Serial.print('\n');
	}
	//identify page number and page offset from VM address
	unsigned long pageNumber = index / 32;
	unsigned long pageOffset = index % 32;

	if(VERBOSE == 1) {
		Serial.print("PAGE = ");
		Serial.print(pageNumber);
		Serial.print(", OFFSET = ");
		Serial.print(pageOffset);
		Serial.print('\n');
	}
	//check to see if page is in the page table
	//if true, return the content from physical memory
	if(verifyPageTable(pageNumber)) {
		if(VERBOSE == 1) {
			Serial.println("PAGE FOUND");
			Serial.print("Page table entry: ");
			Serial.print(currentPageIndex);
			Serial.print('\n');
			Serial.print("Physical memory address: ");
			Serial.print((32 * currentPageIndex) + pageOffset);
			Serial.print('\n');
		}
		return physicalMemory[(32 * currentPageIndex) + pageOffset];
	}
	//else, page fault
	else {
		if(VERBOSE == 1) {
			Serial.println("PAGE FAULT!!!");
		}
		raisePageFault(pageNumber);
		if(VERBOSE == 1) {
			Serial.print("Physical memory address: ");
			Serial.print((32 * currentPageIndex) + pageOffset);
			Serial.print('\n');
		}
		return physicalMemory[(32 * currentPageIndex) + pageOffset];
	}
}
