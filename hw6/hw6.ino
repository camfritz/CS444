#include "FS.h"

FS newFS;

void setup() {
	Serial.begin(115200);
  // put your setup code here, to run once:
	newFS.reformat();
	newFS.initialize();
	newFS.listFiles();
	newFS.createFile("File1.txt");
  newFS.createFile("anotherFile.txt");

	FCB file = newFS.openFile("File1.txt");
  FCB file2 = newFS.openFile("anotherFile.txt");
	char charBuf[18];
	newFS.writeData(&file, "Hello from File 1!", 18);
  newFS.writeData(&file2, "This is file 2!!!!", 18);
	newFS.closeFile(&file);
  newFS.closeFile(&file2);

	file = newFS.openFile("File1.txt");
  file2 = newFS.openFile("anotherFile.txt");
	newFS.seekFile(&file);
  newFS.seekFile(&file2);
	newFS.readData(&file, charBuf, 18);
  Serial.println(charBuf);
  newFS.readData(&file2, charBuf, 18);
  Serial.println(charBuf);

  newFS.listFiles();
}

void loop() {
  // put your main code here, to run repeatedly:
}

