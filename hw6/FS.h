#include <Arduino.h>
#include <Microchip_24LC256.h>
static const byte MASKS[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

struct FCB {
	char fileName[30];
	int fileOffset;
	int dataBlocks[16];
};

class FS {
private:
	byte freeSpaceList[64];
	int fileDirectory[32];
	FCB tempFCB;
	Microchip_24LC256 eeprom;

public:
	void initialize();
	void reformat();
	void createFile(char* name);
	void openFile(char* name);
	void writeByte(void* data);
	readByte();
	seekFile();
	closeFile();
	deleteFile();
	void listFiles();

	int findFreeBit();
};
