#include "ringbuffer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "fcthread/fcthread.h"

using namespace std;

class ValueEntry : public AbstractEntry
{
private:
  long value_;
  
public:
  long getValue()
  {
    return value_;
  }
  
  void setValue(const long & value)
  {
    value_ = value;
  }
};

// Callback handler which can be implemented by consumers
class MyBatchHandler : public BatchHandler<ValueEntry>
{
private: 
    int batch_;
    int numInBatch_;
    long fromSeq_;
    long toSeq_;
    long total_;
    string name_;
public:
    MyBatchHandler(const char* name): batch_(0), numInBatch_(0), fromSeq_(-1L), toSeq_(-1), total_(0), name_(name) {
    }

  virtual void onAvailable(ValueEntry & entry) {
    // process a new entry as it becomes available.

      total_ += entry.getValue();
      /*
      long v =  entry.getValue();
      for (long l = 0; l<10000000; l++) {
           v += l % 50;
      }
      */
      if (0 == numInBatch_) {
          fromSeq_ = entry.getSequence();
      }     
      toSeq_ = entry.getSequence();
      ++ numInBatch_;
  }
  
  virtual void onEndOfBatch() {
    // useful for flushing results to an IO device if necessary.
      std::cout << name_ << " >> " << batch_++ << " : running total = " << total_  << ", ";
      std::cout << numInBatch_ << " entries in batch, [" << fromSeq_ << " : " << toSeq_ << "]" << std::endl; 
      numInBatch_ = 0;
  } 
};


class SlowBatchHandler : public MyBatchHandler {
public:
    SlowBatchHandler(const char* name) : MyBatchHandler(name) {

    }

    // override
    virtual void onAvailable(ValueEntry & entry) {
    // process a new entry as it becomes available.
      MyBatchHandler::onAvailable(entry);
      long v =  entry.getValue();
      for (long l = 0; l<1000000; l++) {
           v += l % 50;
      }
    }
};

class ConsumerThread : public fc::Thread {
public:
    ConsumerThread(BatchConsumer<ValueEntry>& consumer) : consumer_(consumer) {
    }

    void run() {
        consumer_.run();
    }

    void stop() {
        consumer_.stop();
    }

private:
  BatchConsumer<ValueEntry>& consumer_;  
};

int main () {
    char ss[256];
    printf("after debug,  enter to continue\n");
    gets(ss);

    RingBuffer<ValueEntry> ringBuffer(80);
    
    std::vector<Consumer *> consumerList(0);
    ConsumerBarrier<ValueEntry> * consumerBarrier = ringBuffer.createConsumerBarrier(consumerList);
  
    MyBatchHandler fastHandler("consumer1");
    BatchConsumer<ValueEntry> * batchConsumer = new BatchConsumer<ValueEntry>(*consumerBarrier, fastHandler);

    SlowBatchHandler slowHandler("consumer2");
    BatchConsumer<ValueEntry> * slowConsumer = new BatchConsumer<ValueEntry>(*consumerBarrier, slowHandler);

    // setup consumers to be waited by producer
    std::vector<Consumer *> batchConsumers(2);
    batchConsumers[0] = batchConsumer;
    batchConsumers[1] = slowConsumer;
  
    ProducerBarrier<ValueEntry> * producerBarrier = 
    ringBuffer.createProducerBarrier(batchConsumers);
    
    ConsumerThread thread1(*batchConsumer);
    ConsumerThread thread2(*slowConsumer);
    thread1.start();
    thread2.start();

    long expectedResult = 0;
    for (int i=0; i <= 1023; ++i) {
        // Producers claim entries in sequence
        ValueEntry * entry = producerBarrier->nextEntry();

        entry->setValue(i);

        // make the entry available to consumers
        producerBarrier->commit(*entry);   
        expectedResult += i;
    }

    // wait for a while
    fc::Thread::sleep(5);

    thread1.stop();
    thread2.stop();
    thread1.join();
    thread2.join();

    cout<< "--------------------------------------" << endl;
    cout<< "expected result = " << expectedResult << endl;
    cout << "exist with normal code" << endl;
    return 0;
}