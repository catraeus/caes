// $Id: AlsaStreamIn.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaStreamIn.cpp
// Original Author    : duncang
// Creation Date      : May 19, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//
//=================================================================================================


#include <caes/Alsa/AlsaStreamIn.hpp>
#include <caes/CaesMath.hpp>

uint AlsaStreamIn::n = 0;

     AlsaStreamIn::AlsaStreamIn(Alsa *i_ae)
: AlsaStream(i_ae)
, firstIsDone(false)  {
  direction = EDIR_CAPTURE;
  pConvert  = NULL;
  asyncHndl = NULL;
  phase = 0.0;
  }
     AlsaStreamIn::~AlsaStreamIn() {
  fprintf(stdout, "AlsaStreamIn destructor\n");
  }
void AlsaStreamIn::Open(void) {
  deviceName = theAlsa->GetPcmSpec();

  err = snd_pcm_open(&(pcmIf), deviceName, SND_PCM_STREAM_CAPTURE, 0 );
  if(err < 0)
    fprintf(stdout, "PCM Tome open error: %s\n", snd_strerror(err));


  //================
  //Per Session - Derived
  if(af->NumBytes() == 4) {
    pConvert = &caes_Conv_i32i_f64v_s;
    fprintf(stdout, "Will insert 32-bit conversion.\n");
    }
  else if(af->NumBytes() == 2) {
    pConvert = &caes_Conv_i16i_f64v_s;
    fprintf(stdout, "Will insert 16-bit conversion.\n");
    }
  else {
    pConvert = &caes_Conv_i16i_f64v_s;
    fprintf(stdout, "Will insert 16-bit conversion.\n");
    }

  Setup();
  Announce();
  return;
  }
void AlsaStreamIn::Start(void) {

  err = 0;
  // Gets Called Once!
  fprintf(stdout, "AlsaStreamIn::Start()\n");
  phase = 0.0;
  err = snd_async_add_pcm_handler(&asyncHndl, pcmIf, _AlsaCallBack, this);
  if (err < 0) fprintf(stdout, "Unable to register async handler\n");
  if (snd_pcm_state(pcmIf) == SND_PCM_STATE_PREPARED) {
    err = snd_pcm_start(pcmIf);
    if (err < 0) fprintf(stdout, "Start error: %s\n", snd_strerror(err));
    }
  fprintf(stdout, "We Have PCM Input Startage!\n");
  return;
  }
void AlsaStreamIn::Close() {
  err = snd_pcm_close(pcmIf);
  if (err < 0) fprintf(stdout, "PCM Capture close error: %s\n", snd_strerror(err));
  return;
  }
void AlsaStreamIn::_AlsaCallBack(snd_async_handler_t *i_pAsyncHndl) {
  snd_pcm_t     *l_pPcmIf;
  word          *l_pAudBlk;
  AlsaStreamIn  *l_pAi;
  uint           l_avail;
  //unsigned long long theRdtsc;
  //__asm__ __volatile__("rdtsc": "=A" (theRdtsc));
  //fprintf(stdout, "%Lu\n", theRdtsc);

  l_pPcmIf    = snd_async_handler_get_pcm(i_pAsyncHndl);
  l_pAi       = (AlsaStreamIn *) snd_async_handler_get_callback_private(i_pAsyncHndl);
  l_pAudBlk   = (word *)l_pAi->pAudBlk;
  l_avail     = (uint)snd_pcm_avail_update(l_pPcmIf);

  while (l_avail >= l_pAi->GetPeriodFrames()) {
    l_pAi->err = snd_pcm_readi(l_pPcmIf, l_pAudBlk, l_pAi->GetPeriodFrames());
    if (l_pAi->err < 0) fprintf(stdout, "Read error: %s\n", snd_strerror(l_pAi->err));
    if ((uint) l_pAi->err != l_pAi->GetPeriodFrames()) fprintf(stdout, "Read error: written %d expected %lu\n", l_pAi->err, l_pAi->GetPeriodFrames());
    l_avail = (uint) snd_pcm_avail_update(l_pPcmIf);
#ifndef NO_SIGNALS
    l_pAi->TakeWave.emit((long)l_pAi->periodFrames);
#endif
    }
  }
void AlsaStreamIn::Convert(uint N, double *pL, double *pR) {
  pConvert(N, pAudBlk, pL, pR);
  return;
  }
void AlsaStreamIn::Stop(void) {
  snd_async_del_handler(asyncHndl);
  err = snd_pcm_drop(pcmIf);
  return;
  }
