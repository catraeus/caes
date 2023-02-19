
// $Id: CpuRdtsc.hpp 195 2020-11-21 05:41:49Z duncang $

static inline unsigned long long CpuRdtsc(void) {
  unsigned long long theRdtsc;

  __asm__ __volatile__("rdtsc": "=A" (theRdtsc));
  return theRdtsc;
  }


static inline unsigned long long CpuRdtscA(void){
  unsigned int lo,hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((unsigned long long)hi << 32) | lo;
  }
