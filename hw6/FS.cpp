#include "FS.h"

int FS::findFreeBit() {
	for(int i = 0; i < 64; i++) {
		for(int j = 0; j < 8; j++) {
			if((freeSpaceList[i] & MASKS[j]) == MASKS[j]) {
				//set free bit to occupied
				freeSpaceList[i] = (freeSpaceList[i] ^ MASKS[j]);
				return (i * 8) + j;
			}
		}
	}
	return -1;
}

void FS::initialize() {
	eeprom.read_page(0, freeSpaceList);
	eeprom.read_page(1, (byte*) fileDirectory);
}

void FS::reformat() {
	Serial.println("Formating EEPROM...");
	memset(freeSpaceList, 0xFF, 64);
	// memset((byte*) fileDirectory, 0xFF, 64);
	for(int i = 0; i < 32; i++) {
		fileDirectory[i] = -1;
	}

	freeSpaceList[0] = 0xFC;
	eeprom.write_page(0, freeSpaceList);
	eeprom.write_page(1, (byte*) fileDirectory);
}

void FS::createFile(char* name) {
	Serial.println("Creating File...");
	//TODO check if file already in directory
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			if(strcmp(tempFCB.fileName, name) == 0) {
				Serial.println("ERROR: File already exists in directory");
				return;
			}
		}
	}

	//find free space in bit vector for FCB
	int freeBit = findFreeBit();
	if(freeBit == -1) {
		Serial.println("ERROR: No space in EEPROM");
		return;
	}

	Serial.println(freeBit);
	//enter address of FCB into file directory
	bool spaceinDirectory = false;
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] == -1) {
			fileDirectory[i] = freeBit;
			spaceinDirectory = true;
			break;
		}
	}

	if(!spaceinDirectory) {
		Serial.println("ERROR: No space in file directory");
		return;
	}

	//set properties of new FCB
	//initialize FCB filename to null
	for(int i = 0; i < 30; i++) {
		tempFCB.fileName[i] = '\0';
	}
	//set file name of FCB
	for(int i = 0; i < strlen(name); i++) {
		tempFCB.fileName[i] = name[i];
	}

	//intialize file offset
	tempFCB.fileOffset = 0;

	//initialize data blocks
	for(int i = 0; i < 16; i++) {
		tempFCB.dataBlocks[i] = -1;
	}

	//write FCB into eeprom
	// eeprom.write_page(freeBit, reinterpret_cast<byte*> (&tempFCB));
	eeprom.write_page(freeBit, (byte*) &tempFCB);

	//write file directory to eeprom
	eeprom.write_page(1, (byte*) fileDirectory);
	//write free space list into eeprom
	eeprom.write_page(0, freeSpaceList);
}

void FS::listFiles() {
	Serial.println("listing files...");

	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			//eeprom.read_page(fileDirectory[i], reinterpret_cast<byte*> (&tempFCB));
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			int fileSize = 0;
			for(int j = 0; j < 16; j++) {
				if(tempFCB.dataBlocks[j] != -1) {
					fileSize += 64;
				}
			}

			Serial.print("File: ");
			Serial.print(tempFCB.fileName);
			Serial.print(", ");
			Serial.print(fileSize);
			Serial.print(" bytes");
			Serial.print('\n');
		}
	}
}

void FS::openFile(char* name) {
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			if(strcmp(tempFCB.fileName, name) == 0) {
				Serial.println("Opening file...");
				return;
			}
		}
	}
	Serial.println("ERROR: File does not exist!");
	return;
}

void FS::writeByte(void* data) {

}
