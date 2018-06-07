#include <Microchip_23LC1024.h>
#include <SPI.h>
#include "VM.h"

VM vm;

void setup()
{
  unsigned long sum = 0;

  Serial.begin(115200);
  Serial.println("======SUMMING VECTORS======");

  for(unsigned long i = 0; i < 100000; i++) {
    vm[i] = (byte) random(1, 255);
    if(i % 10000 == 0) {
      Serial.print("...");
    }
    sum += vm[i];
    if(i == 49999) {
     Serial.println();
     Serial.println("Sum of Vector 1: ");
     Serial.print(sum);
     sum = 0;
   }
   if(i == 99999) {
     Serial.print('\n');
     Serial.println("Sum of Vector 2: ");
     Serial.print(sum);
     Serial.print('\n');
   }
 }
 Serial.print('\n');
 Serial.println("Page Fault Rate: ");
 Serial.print(vm.getFaultRate());
 Serial.print('\n');

 Serial.println("======SIMULATE PAGE THRASHING======");
 vm.resetFaultRate();

 for(int i = 4096; i > 0; i--) {
  if(i % 128 == 0) {
    Serial.print("...");
  }
  vm[i * 32] = 0;
}
Serial.println("Page Fault Rate: ");
Serial.print(vm.getFaultRate());
Serial.print('\n');
}

void loop()
{
}

