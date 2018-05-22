#include <Microchip_23LC1024.h>
#include <SPI.h>
#include "VM.h"

VM vm;
char *msg = "Hello from virtual memory!!!";

void setup()
{
	Serial.begin(115200);
  for(int i = 0; i < strlen(msg); i++)
  {
    vm[i*32] = msg[i];
  }
}

void loop()
{
  for(int i = 0; i < strlen(msg); i++)
  {
    Serial.print((char) vm[i*32]);
  }
  vm[1000] = 50;
  vm[200] = 20;
  Serial.println('\n');
  Serial.println(vm[1000] + vm[200]);
  delay(1000);
}

