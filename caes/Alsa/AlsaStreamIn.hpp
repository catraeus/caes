
// $Id: AlsaStreamIn.hpp 181 2019-12-25 15:33:16Z duncang $

//=================================================================================================
// Original File Name : AlsaStreamIn.h
// Original Author    : duncang
// Creation Date      : May 19, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//
//=================================================================================================


#ifndef ALSASTREAMIN_H_
#define ALSASTREAMIN_H_
#include "AlsaStream.hpp"
#ifndef NO_SIGNALS
#include <sigc++/sigc++.h>
#endif

class AlsaStreamIn :public AlsaStream {
  public:
                    AlsaStreamIn(Alsa *i_ae);
           virtual ~AlsaStreamIn();
           void  Open         ();
           void  Start        (void);
   static  void _AlsaCallBack (snd_async_handler_t *_pAsyncHndl);
           void  Convert      (uint N, double *pL, double *pR);
           void  Stop         (void);
           void  Close        ();
#ifndef NO_SIGNALS
   sigc::signal<void, long> TakeWave;
#endif
  private:
  public:
           void                  (*pConvert)( llong   N, void   *dst,  double  *pL,  double *pR          );
           snd_async_handler_t    *asyncHndl;
           double                  phase;
           bool                    firstIsDone;
  private:
    static uint           n;
  };

#endif /* ALSASTREAMIN_H_ */
