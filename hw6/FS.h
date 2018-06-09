#include <Arduino.h>
#include <Microchip_24LC256.h>
static const byte MASKS[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

struct FCB {
	char fileName[30];
	int fileOffset = 0;
	int dataBlocks[16];
};

class FS {
private:
	//int current_FCB_block;
	byte freeSpaceList[64];
	int fileDirectory[32];
	byte tempDataBlock[64];
	FCB tempFCB;
	Microchip_24LC256 eeprom;

public:
	void initialize();
	void reformat();
	void createFile(char* name);
	FCB openFile(char* name);
	void writeData(FCB *fcb, void* data, int numberofBytes);
	void readData(FCB *fcb, void* data, int numberofBytes);
	void seekFile(FCB *fcb);
	void closeFile(FCB *fcb);
	void deleteFile(char* name);
	void listFiles();

	int findFreeBit();
	void freeBit(int bit);
	void setBit(int bit);
};
