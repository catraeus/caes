

/* $Id: CaesIpps.cpp 164 2019-04-26 22:36:24Z duncang $ */

#include <malloc.h>
#include "CaesIpps.hpp"

  Ipp32f *ippsMalloc_32f     (                                               uint len               ) {
    return (Ipp32f *)malloc(len * sizeof(Ipp32f));
    }
  void    ippsZero_32f       (                              Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] = 0.0;
    }
  void    ippsSet_32f        (              const Ipp32f C, Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] = C;
    }
  void    ippsMove_32f       (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] = pSrc[i];
    }
  void    ippsMulC_32f       (Ipp32f *pSrc, const Ipp32f C, Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] = pSrc[i] * C;
    }
  void    ippsMulC_32f_I     (              const Ipp32f C, Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] *= C;
    }
  void    ippsAddC_32f_I     (              const Ipp32f C, Ipp32f *pSrcDst, uint len               ) {
    for(uint i=0; i<len; i++)
      pSrcDst[i] += C;
    }
  void    ippsAdd_32f_I      (Ipp32f *pSrc,                 Ipp32f *pSrcDst, uint len               ) {
    for(uint i=0; i<len; i++)
      pSrcDst[i] += pSrc[i];
    }
  void    ippsAddProductC_32f(Ipp32f *pSrc, const Ipp32f C, Ipp32f *pSrcDst, uint len               ) {
    for(uint i=0; i<len; i++)
      pSrcDst[i] += pSrc[i] * C;
    }
  void    ippsSqr_32f        (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++) {
      pDst[i] = pSrc[i];
      pDst[i] *= pSrc[i];
      }
    }
  void    ippsAbs_32f        (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               ) {
    for(uint i=0; i<len; i++)
      pDst[i] = (pSrc[i] < 0.0) ? -1.0 * pSrc[i] : pSrc[i];
    }
  void    ippsMax_32f        (Ipp32f *pSrc,                                  uint len, Ipp32f *value) {
    *value = pSrc[0];
    for(uint i=0; i<len; i++)
      if(*value < pSrc[i]) *value = pSrc[i];
    }
  Ipp64u  ippGetCpuClocks    (void                                                                  ) {
    return 0;
    }
