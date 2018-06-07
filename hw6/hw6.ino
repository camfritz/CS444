#include "FS.h"

FS newFS;

void setup() {
	Serial.begin(115200);
  // put your setup code here, to run once:
  newFS.reformat();
	newFS.initialize();
  newFS.createFile("File1.txt");
  newFS.createFile("File2.txt");
  newFS.listFiles();
  newFS.openFile("File1.txt");
  newFS.writeData("Hello CS 444, this is file 00!!!");
  newFS.closeFile();
  newFS.listFiles();
}

void loop() {
  // put your main code here, to run repeatedly:
}

