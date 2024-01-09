#pragma once
#include "buffer_manager.h"
#include <thread>

class DataProcessor {
private:
  std::unique_ptr<std::thread> processor_thread;
  BufferManager *buffer_manager;
  void ProcessFromBuffer(void *buffer);

public:
  DataProcessor(BufferManager *bm);
  void Run();
};

DataProcessor::DataProcessor(BufferManager *bm) {
  processor_thread = std::make_unique<std::thread>(&DataProcessor::Run, this);
  buffer_manager = bm;
}

// This should be replaced by the actual processing
void DataProcessor::ProcessFromBuffer(void *buffer) {
  int cur_val = (int)(*(uint8_t *)buffer);
  printf("Reading from %ld, gets value %d\n", (std::size_t)buffer, cur_val);
  // emulate cases where we take a long time to process data
  if (cur_val % 10 == 9) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void DataProcessor::Run() {
  while (true) {
    void *reading_ptr = buffer_manager->AcquireReadableBuffer();
    ProcessFromBuffer(reading_ptr);
    buffer_manager->DoneReadingContent();
  }
};
