
// $Id: CaesPThread.hpp 162 2019-04-26 22:33:30Z duncang $

//=================================================================================================
// Original Name      : CaesPThread.hpp
// Original Author    : duncang
// Creation Date      : 2015-05-30
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
// Description        :
/*
    A c++ wrapper to get pthread to work.  I know that boost is coming to standard c but venerable
      pthread does a fine job and goes directly to the kernel.
    The pattern is that this interface definer should never be instantiated.  Instead ... a derived
      class will have any level of functionality but be able to have a function implememted.  If
      we really need to have complex functionality on many levels, then this can be used
      as a pattern for a class that isn't derived.  After all ... the static function can be as
      manifold as needed.
    WARNING:
      I don't know how thread-safe a static function is ... is it duplicated by the OS or the
      compiler?  If it isn't ever duplicated, then there is the re-entrant problem all over again
      more times.  In classical function instantiation, without threads, the calling context ...
      the "Entry" static function below ... stays in context to pop back out.
*/
//
//=================================================================================================

#ifndef CAEV_CAEV_PTHREAD_HPP_
#define CAEV_CAEV_PTHREAD_HPP_
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

class PThread {
  public:
                  PThread(void *iD) : _thread((pthread_t)NULL), iD(iD) {}
   virtual       ~PThread() {}
            bool  Spawn  () {
    int err;
    err = pthread_create(&_thread, NULL, Entry, this); // assumes pthread_create returns 0 meaning goodness
    if(err != 0)
      fprintf(stderr, "Thread Spawn error: %s", strerror(err));
    return err == 0;
    }
            bool  Join   () { // blocking function to hold up the thread spawning function until complete.
  int err;
  err = pthread_join(_thread, NULL);
  if(err != 0)
    fprintf(stderr, "Thread Join error: %s", strerror(err));
  return err == 0;
}
bool  JoinTry   () { // blocking function to hold up the thread spawning function until complete.
  int err;

  err = pthread_tryjoin_np(_thread, NULL);
  return err == 0;
}
  protected:
    virtual void  Executor(void *iD) = 0;  // Implement this in your derived version of the thread class.
  private:
    static  void *Entry(void *i_pInstance) { // Uses neither global nor static data, therefore thread safe by re-entrant safe.
                                             // Never make this in your derived class.
      ((PThread *)i_pInstance)->Executor(((PThread *)i_pInstance)->iD);
      return NULL;
      }
  pthread_t _thread;
    void   *iD;
  // Don't forget to grow this threaded class as needed
  };

#endif /* CAEV_CAEV_PTHREAD_HPP_ */
