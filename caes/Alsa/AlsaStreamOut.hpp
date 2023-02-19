// $Id: AlsaStreamOut.hpp 181 2019-12-25 15:33:16Z duncang $

//=================================================================================================
// Original File Name : AlsaStreamOut.h
// Original Author    : duncang
// Creation Date      : May 21, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//
//=================================================================================================


#ifndef __ALSA_STREAM_OUT_H_
#define __ALSA_STREAM_OUT_H_
#include "AlsaStream.hpp"
#ifndef NO_SIGNALS
#include <sigc++/sigc++.h>
#endif

class AlsaStreamOut : public AlsaStream {
  private:
  public:
                         AlsaStreamOut(Alsa *i_ae);
           virtual      ~AlsaStreamOut();
                   void  Open         ();
                   void  Start        (void);
           static  void _AlsaCallBack (snd_async_handler_t *_pAsyncHndl);
                   void  Convert      (uint N, double *pL, double *pR);
                   void  Stop         (void);
                   void  Close        ();
#ifndef NO_SIGNALS
           sigc::signal<void, long, double *> MakeWave;
#endif
  private:
  public:
                   void  (*pConvert)(llong N, double *pL, double *pR, void *dst);
    snd_async_handler_t    *asyncHndl;
    double                  phase;

  private:
  };

#endif // __ALSA_STREAM_OUT_H_
