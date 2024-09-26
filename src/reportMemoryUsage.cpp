#include <Arduino.h>
#include <stdint.h> // Include for uintptr_t

// Function to report free stack and heap memory
void reportMemoryUsage() {
  // Get stack usage
  extern unsigned long _ebss;
  char stack;
  size_t stackUsed = (size_t)&stack - (size_t)&_ebss;
  size_t stackFree = (size_t)0x20000000 - (size_t)&stack; // 0x20000000 is the end of RAM for Teensy 4.1

  // Get heap usage
  extern unsigned long _heap_start;
  extern unsigned long _heap_end;
  extern char *__brkval;
  size_t heapUsed = (size_t)((uintptr_t)(__brkval ? __brkval : (char*)&_heap_start) - (uintptr_t)&_heap_start);
  size_t heapFree = (size_t)((uintptr_t)&_heap_end - (uintptr_t)(__brkval ? __brkval : (char*)&_heap_start));

  // Print memory usage
  Serial.print("Stack used: ");
  Serial.print(stackUsed);
  Serial.print(" bytes, Stack free: ");
  Serial.print(stackFree);
  Serial.println(" bytes");

  Serial.print("Heap used: ");
  Serial.print(heapUsed);
  Serial.print(" bytes, Heap free: ");
  Serial.print(heapFree);
  Serial.println(" bytes");
}