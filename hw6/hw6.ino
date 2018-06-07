#include "FS.h"

FS newFS;

void setup() {
	Serial.begin(115200);
  // put your setup code here, to run once:
  newFS.reformat();
	newFS.initialize();
}

void loop() {
  // put your main code here, to run repeatedly:
	newFS.createFile("File1.txt");
  newFS.createFile("File2.txt");
  newFS.createFile("File3.txt");
  newFS.createFile("File4.txt");
  newFS.createFile("File5.txt");
  newFS.createFile("File6.txt");
  newFS.createFile("File7.txt");
  newFS.createFile("File8.txt");
  newFS.createFile("File9.txt");
  newFS.createFile("File10.txt");
  newFS.createFile("File11.txt");
  newFS.createFile("File12.txt");
  newFS.createFile("File13.txt");
  newFS.createFile("File14.txt");
  newFS.createFile("File15.txt");
  newFS.createFile("File16.txt");
  newFS.createFile("File17.txt");
  newFS.createFile("File18.txt");
  newFS.createFile("File19.txt");
  newFS.createFile("File20.txt");
  newFS.createFile("File21.txt");
  newFS.createFile("File22.txt");
  newFS.createFile("File23.txt");
  newFS.createFile("File24.txt");
  newFS.createFile("File25.txt");
  newFS.createFile("File26.txt");
  newFS.createFile("File27.txt");
  newFS.createFile("File28.txt");
  newFS.createFile("File29.txt");
  newFS.createFile("File30.txt");
  newFS.createFile("File31.txt");
  newFS.createFile("File32.txt");
  newFS.createFile("File33.txt");
  newFS.listFiles();
}
