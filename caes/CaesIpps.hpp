


/* $Id: CaesIpps.hpp 164 2019-04-26 22:36:24Z duncang $ */

#include <caes/CaesTypes.hpp>
  typedef               float Ipp32f;
  typedef unsigned long long  Ipp64u;

  Ipp32f *ippsMalloc_32f     (                                               uint len               );
  void    ippsZero_32f       (                              Ipp32f *pDst,    uint len               );
  void    ippsSet_32f        (              const Ipp32f C, Ipp32f *pDst,    uint len               );
  void    ippsMove_32f       (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               );
  void    ippsMulC_32f       (Ipp32f *pSrc, const Ipp32f C, Ipp32f *pDst,    uint len               );
  void    ippsMulC_32f_I     (              const Ipp32f C, Ipp32f *pDst,    uint len               );
  void    ippsAdd_32f_I      (Ipp32f *pSrc,                 Ipp32f *pSrcDst, uint len               );
  void    ippsAddC_32f_I     (              const Ipp32f C, Ipp32f *pDst,    uint len               );
  void    ippsAddProductC_32f(Ipp32f *pSrc, const Ipp32f C, Ipp32f *pDst,    uint len               );

  void    ippsSqr_32f        (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               );
  void    ippsAbs_32f        (Ipp32f *pSrc,                 Ipp32f *pDst,    uint len               );

  void    ippsMax_32f        (Ipp32f *pSrc,                                  uint len, Ipp32f *value);

  Ipp64u  ippGetCpuClocks    (void                                                                  );
