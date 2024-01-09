#pragma once
#include "buffer_manager.h"
#include <thread>

class DataSource {
private:
  std::unique_ptr<std::thread> generator_thread;
  BufferManager *buffer_manager;

public:
  DataSource(BufferManager *bm);
  void Run();
};

DataSource::DataSource(BufferManager *bm) {
  generator_thread = std::make_unique<std::thread>(&DataSource::Run, this);
  buffer_manager = bm;
}
void DataSource::Run() {
  // basic counter to sequence the package
  uint8_t count = 1;
  while (true) {
    void *write_ptr = buffer_manager->AcquireWritableBuffer();
    // take 100 milliseconds to finish writing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // take some time to read the buffer
    printf("Writing to %ld value %d\n", (std::size_t)write_ptr, (int)count);
    *(uint8_t *)write_ptr = count;
    count++;
    buffer_manager->CommitWrittenContent();
  }
};
