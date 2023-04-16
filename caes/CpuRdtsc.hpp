
// $Id: CpuRdtsc.hpp 195 2020-11-21 05:41:49Z duncang $

static inline unsigned long long CpuRdtsc(void) { // OOO happens
  unsigned long long theRdtsc;

  __asm__ __volatile__("rdtsc": "=A" (theRdtsc));
  return theRdtsc;
  }


static inline unsigned long long CpuRdtscA(void) { // seems to avoid OOO execution
  unsigned int lo,hi;
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return ((unsigned long long)hi << 32) | lo;
  }

static inline unsigned long long CpuRdtscP(void) { // Seems to add 30-ish ticks extra time, no OOO observed
  unsigned long long theRdtsc;
  __asm__ __volatile__("rdtscp; "         // serializing read of tsc
                       "shl $32,%%rdx; "  // shift higher 32 bits stored in rdx up
                       "or %%rdx,%%rax"   // and or onto rax
                       : "=a"(theRdtsc)   // output to theRdtsc variable
                       :
                       : "%rcx", "%rdx"); // rcx and rdx are clobbered
  return theRdtsc;
  }
