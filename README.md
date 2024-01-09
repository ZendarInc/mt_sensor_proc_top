## Description of Demo

This example contains one BufferManager class, one data source thread, and one processor thread. It uses a double buffering scheme to allow one thread to read data while the other thread is writing the next chunk of data.

### BufferManager
The BufferManager would manage two pre-allocated buffer space and provides the double buffering mechanism. It assumes a **single writer thread** and a **single reader thread**, and it ensures that when it makes a piece of memory buffer available for writing, 
the writing process can happen over a period of time during which the reader would not be reading from this partially filled buffer. Similarly, when the reader acquires a buffer for reading, the buffer would
contain good data and won't be overwritten until the reader thread signals that the read is complete. This is achieved by the two pairs of functions: *AcquireWritableBuffer()/CommitWrittenContent()* and *AcquireReadableBuffer()/DoneReadingContent()*.

In a real use case, it is expected for the writer thread to first call *AcquireWritableBuffer()* to get a writable piece of memory, and after it finishes populating the memory, it can call *CommitWrittenContent()* to mark it ready for consumption.
Similarly, the reader thread would first call *AcquireReadableBuffer()*, which blocks until there is some valid data, and then process the data, before calling *DoneReadingContent()* to make the read buffer available for the writer to populate again,

Note: In the case where reader thread is consuming data slower than the writer thread generates data, the buffer manager makes sure the reader thread would always be reading the newest chunk written by the writer (older valid data gets overwritten). 

### DataSource
This is a standin for the sensor data ingestion module. In the example, its *Run()* function is running in a standalone thread, dumping fake data into preallocated buffers managed by BufferManager.

### DataProcessor
This is a standin for the processing pipeline. In this example, its *Run()* function is running in a standalone thread, reading data from preallocated buffers managed by BufferManager.

### Runtime behavior
To run the example, simply type make, and run the generated executable *sensor_proc_top*, you should see stdout showing a sequence of number being written to addresses and being read out from those addresses. 
Note the example purposely slow down the reader from time to time, so you can see the reader can skip a few numbers as these represent stale data written out when the reader is busy processing previous data.

