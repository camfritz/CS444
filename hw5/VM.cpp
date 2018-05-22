#include "VM.h"

VM::VM() : sram(SRAM, HOLD) {
	//initialize the page table with empty slots
	for(int i = 0; i < TABLE_SIZE; i++) {
		pageTable[i] = -1;
	}
	nextVictimPage = 0;
}

void VM::getFaultRate() {

}

void VM::resetFaultRate() {

}

bool VM::verifyPageTable(int number) {
	for(int i = 0; i < TABLE_SIZE; i++) {
		currentPageIndex = i;
		if(pageTable[i] == number) {
			return true;
		}
	}
	return false;
}

void VM::raisePageFault(int number) {
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
		//Serial.println("SLOT AVAILABLE");
		sram.read_page(number, &physicalMemory[(32 * emptySlotNumber)]);
		//update page table
		pageTable[emptySlotNumber] = number;
		currentPageIndex = emptySlotNumber;
	}
	//if no empty slot available, identify victim page
	else {
		//Serial.println("NO SLOTS AVAILABLE");
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

byte &VM::operator[] (int index)
{
	//Serial.println("ACCESSING VIRTUAL MEMORY");
	//identify page number and page offset from VM address
	int pageNumber = index / 32;
	int pageOffset = index % 32;

	//check to see if page is in the page table
	//if true, return the content from physical memory
	//Serial.println("VERIFYING PAGE TABLE");
	if(verifyPageTable(pageNumber)) {
    //Serial.println("PAGE IN TABLE");
		return physicalMemory[(32 * currentPageIndex) + pageOffset];
	}
	//else, page fault
	else {
    //Serial.println("PAGE FAULT!!!");
		raisePageFault(pageNumber);
		return physicalMemory[(32 * currentPageIndex) + pageOffset];
	}
}
