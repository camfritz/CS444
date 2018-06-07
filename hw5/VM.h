#include <Microchip_23LC1024.h>
#include <Arduino.h>

#define VERBOSE 0
#define TABLE_SIZE 24
#define SRAM 5
#define HOLD 7

class VM {
private:
	int currentPageIndex;
	int pageTable[TABLE_SIZE];
	byte physicalMemory[32 * TABLE_SIZE];
	Microchip_23LC1024 sram;
	unsigned long numberofMemoryReferences;
	unsigned long numberofPageFaults;
	int nextVictimPage;

	bool verifyPageTable(unsigned long number);
	void raisePageFault(unsigned long number);

public:
	VM();
	double getFaultRate();
	void resetFaultRate();
	byte &VM::operator[] (unsigned long index);
};
