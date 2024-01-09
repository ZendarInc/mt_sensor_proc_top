#pragma once
#include "buffer_manager.h"
#include <thread>

class DataProcessor {
private:
  std::unique_ptr<std::thread> processor_thread;
  BufferManager *buffer_manager;

public:
  DataProcessor(BufferManager *bm);
  void Run();
};

DataProcessor::DataProcessor(BufferManager *bm) {
  processor_thread = std::make_unique<std::thread>(&DataProcessor::Run, this);
  buffer_manager = bm;
}
void DataProcessor::Run() {
  int count = 1;
  while (true) {
    void *reading_ptr = buffer_manager->AcquireReadableBuffer();
    // take some time to read the buffer
    printf("Reading from %ld, gets value %d\n", (std::size_t)reading_ptr,
           (int)(*(uint8_t *)reading_ptr));
    // emulate cases where we take a long time to process data
    if (count % 10 == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    buffer_manager->DoneReadingContent();
    count += 1;
  }
};
