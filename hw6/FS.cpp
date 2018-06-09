#include "FS.h"

int FS::findFreeBit() {
	for(int i = 0; i < 64; i++) {
		for(int j = 0; j < 8; j++) {
			if((freeSpaceList[i] & MASKS[j]) == MASKS[j]) {
				//set free bit to occupied
				freeSpaceList[i] = (freeSpaceList[i] & ~MASKS[j]);
				return (i * 8) + j;
			}
		}
	}
	return -1;
}

void FS::setBit(int bit) {
	int blockNumber = bit / 64;
	int offset = bit % 64;

	byte mask = MASKS[offset];

	freeSpaceList[blockNumber] = (freeSpaceList[blockNumber] & ~mask);
}

void FS::freeBit(int bit) {
	int blockNumber = bit / 64;
	int offset = bit % 64;

	byte mask = MASKS[offset];

	freeSpaceList[blockNumber] = (freeSpaceList[blockNumber] ^ mask);
}

void FS::initialize() {
	eeprom.read_page(0, freeSpaceList);
	eeprom.read_page(1, (byte*) fileDirectory);
}

void FS::reformat() {
	Serial.println("Formating EEPROM...");
	memset(freeSpaceList, 0xFF, 64);
	for(int i = 0; i < 32; i++) {
		fileDirectory[i] = -1;
	}

	setBit(0);
	setBit(1);
	eeprom.write_page(0, freeSpaceList);
	eeprom.write_page(1, (byte*) fileDirectory);
}

void FS::createFile(char* name) {
	Serial.print("Creating File: ");
	Serial.print(name);
	Serial.println();
	//check if file already in directory
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

FCB FS::openFile(char* name) {
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			if(strcmp(tempFCB.fileName, name) == 0) {
				Serial.print("Opening file: ");
				Serial.print(name);
				Serial.println();
				return tempFCB;
			}
		}
	}
	Serial.println("ERROR: File does not exist!");
	return;
}

void FS::writeData(FCB *fcb, void* data, int numberofBytes) {
	byte *buf = (byte*) data;
	for(int i = 0; i < numberofBytes; i++) {
  //find data block & location to write to
  //if data block is empty, find a new location in eeprom for data block
		if(fcb->dataBlocks[fcb->fileOffset / 64] == -1) {
			fcb->dataBlocks[fcb->fileOffset / 64] = findFreeBit();
		}

		eeprom.write_byte(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64), buf[i]);
		// Serial.print("BYTE NUMBER WRITTEN TO: ");
		// Serial.print(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64));
		// Serial.println();
		// Serial.print("CONTENT: ");
		// Serial.print((char) eeprom.read_byte(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64)));
		// Serial.println();
		++fcb->fileOffset;
	}
}

void FS::closeFile(FCB *fcb) {
  //write temp FCB to eeprom
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			if(strcmp(tempFCB.fileName, fcb->fileName) == 0) {
				Serial.print("Closing file: ");
				Serial.print(fcb->fileName);
				Serial.println();
				eeprom.write_page(fileDirectory[i], (byte*) fcb);
				//write free space list to eeprom
				eeprom.write_page(0, freeSpaceList);
				break;
			}
		}
	}

	delete(fcb);
}

void FS::readData(FCB *fcb, void* data, int numberofBytes) {
	byte *buf = (byte*) data;
	for(int i = 0; i < numberofBytes; i++) {
		// Serial.print("BYTE NUMBER READ FROM: ");
		// Serial.print(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64));
		// Serial.println();
		// Serial.print("CONTENT: ");
		// Serial.print((char)eeprom.read_byte(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64)));
		// Serial.println();
		buf[i] = eeprom.read_byte(((fcb->dataBlocks[fcb->fileOffset / 64]) * 64) + (fcb->fileOffset % 64));
   //Serial.println((char) buf[i]);
		++fcb->fileOffset;
	}
}

void FS::seekFile(FCB *fcb) {
	fcb->fileOffset = 0;
}

void FS::deleteFile(char* name) {
	for(int i = 0; i < 32; i++) {
		if(fileDirectory[i] != -1) {
			eeprom.read_page(fileDirectory[i], (byte*) &tempFCB);
			if(strcmp(tempFCB.fileName, name) == 0) {
				Serial.print("Deleting file: ");
				Serial.print(tempFCB.fileName);
				Serial.println();
				freeBit(fileDirectory[i]);
				fileDirectory[i] = -1;

				//write file directory to eeprom
				eeprom.write_page(1, (byte*) fileDirectory);
				//write free space list into eeprom
				eeprom.write_page(0, freeSpaceList);
				return;
			}
		}
	}
}
