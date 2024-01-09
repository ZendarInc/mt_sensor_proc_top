#include "buffer_manager.h"
#include "datasource.h"
#include "processor.h"
#include <stdio.h>
int main() {
  int buffer_slot_size = 1024;
  // not sure what is the pointer type to use for now, so its all void*
  // the life cycle of these buffers might not be tied to the app as well
  // so just use raw pointer
  void *allocated_buffers[SLOT_COUNT];
  for (int i = 0; i < SLOT_COUNT; i++)
    allocated_buffers[i] = (void *)malloc(buffer_slot_size);

  BufferManager bm(allocated_buffers);
  // start the reader thread
  DataProcessor dp(&bm);
  DataSource ds(&bm);
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  return 0;
}
