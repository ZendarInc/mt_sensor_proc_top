#pragma once

#include <assert.h>
#include <condition_variable>
#include <mutex>
#include <string.h>

// Assumption: single reader + single writer + fixed number of preallocated
// buffers the actual buffer allocation is external to this class Double
// buffering is easy, the reader just always get the next newest token
#define SLOT_COUNT 2

//
// for reader thread, it can only consume valid slot
// it changes the slot status to busy before starting the process
// and then after reading change it to empty

// for writer thread, it can write both empty and overwrite valid slot
// it change the slot to busy, and then start writing, when it is done,
// change it to valid
enum SlotStatus { EMPTY, VALID, BUSY };
struct SlotInfo {
  SlotStatus status;
  void *data_ptr; // place holder for data
};

class BufferManager {

private:
  uint8_t write_ptr;
  uint8_t read_ptr;
  bool writing;
  bool reading;
  SlotInfo content[2];
  std::mutex slot_info_mutex;
  std::condition_variable data_avail;

public:
  BufferManager(void *allocated[SLOT_COUNT]) {
    for (int i = 0; i < SLOT_COUNT; i++) {
      content[i] = {EMPTY, allocated[i]};
    }
    write_ptr = 0;
    read_ptr = 0;

    writing = false;
    reading = false;
  }

  // this will always get a buffer, and it might
  // be a buffer with valid data in it
  void *AcquireWritableBuffer() {
    // find the slot to write to
    // first check if write_ptr is busy, if so
    // we make it point to the other slot,
    std::unique_lock<std::mutex> lock(slot_info_mutex);
    assert(!writing);
    writing = true;
    if (content[write_ptr].status == SlotStatus::BUSY) {
      write_ptr = (write_ptr - 1) % SLOT_COUNT;
    }
    // we	should have a writable slot
    assert(content[write_ptr].status != SlotStatus::BUSY);
    content[write_ptr].status = SlotStatus::BUSY;
    return content[write_ptr].data_ptr;
  }

  void CommitWrittenContent() {
    std::unique_lock<std::mutex> lock(slot_info_mutex);
    assert(writing);
    writing = false;
    content[write_ptr].status = SlotStatus::VALID;
    write_ptr = (write_ptr + 1) % SLOT_COUNT;
    // allow the consumer to wake up and consume
    data_avail.notify_one();
  }

  // This will block if no data available for reading
  void *AcquireReadableBuffer() {
    std::unique_lock<std::mutex> lock(slot_info_mutex);
    assert(!reading);
    // we check if read_ptr is point to valid data
    // if not, we wait
    data_avail.wait(
        lock, [&] { return content[read_ptr].status == SlotStatus::VALID; });

    reading = true;
    content[read_ptr].status = SlotStatus::BUSY;
    return content[read_ptr].data_ptr;
  }

  void DoneReadingContent() {
    std::unique_lock<std::mutex> lock(slot_info_mutex);
    assert(reading);
    content[read_ptr].status = SlotStatus::EMPTY;
    read_ptr = (read_ptr + 1) % SLOT_COUNT;
    reading = false;
  }
};
