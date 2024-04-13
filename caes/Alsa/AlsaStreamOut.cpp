// $Id: AlsaStreamOut.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaStreamOut.cpp
// Original Author    : duncang
// Creation Date      : May 21, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//
//=================================================================================================


#include <caes/Alsa/AlsaStreamOut.hpp>
#include <caes/CaesMath.hpp>

     AlsaStreamOut::AlsaStreamOut(Alsa *i_ae) :
  AlsaStream(i_ae) {
  asyncHndl = 0;
  phase     = 0.0;
  pConvert  = 0;
  direction = EDIR_PLAYBACK;
  }
     AlsaStreamOut::~AlsaStreamOut() {
  fprintf(stdout, "AlsaStreamOut destructor\n");
  }
void AlsaStreamOut::Open(void) {
  deviceName = theAlsa->GetPcmSpec();

  err = snd_pcm_open(&(pcmIf), deviceName, SND_PCM_STREAM_PLAYBACK, 0);
  if(err < 0) {
    fprintf(stdout, "PCM Render open error: %s\n", snd_strerror(err));
    return;
    }


  //================
  //Per Session - Derived
  if(af->NumBytes() == 4) {
    pConvert = &caes_Conv_f64v_i32i_s;
    fprintf(stdout, "Will insert 32-bit conversion.\n");
    }
  else if(af->NumBytes() == 2) {
    pConvert = &caes_Conv_f64v_i16i_s;
    fprintf(stdout, "Will insert 16-bit conversion.\n");
    }
  else {
    pConvert = &caes_Conv_f64v_i16i_s;
    fprintf(stdout, "Will insert 16-bit conversion.\n");
    }

  //Setup();
  Announce();
  return;
  }
void AlsaStreamOut::Start(void) {
  int count;

  err = 0;
  // Gets Called Once!
  fprintf(stdout, "AlsaStreamOut::Start()\n");
  phase = 0.0;
  err = snd_async_add_pcm_handler(&asyncHndl, pcmIf, _AlsaCallBack, this);
  if (err < 0) fprintf(stdout, "Unable to register async handler\n");
  for (count = 0; count < 2; count++) { // FIXME this looks like pure MAGICK numbers
#ifndef NO_SIGNALS
    MakeWave.emit(periodFrames, &(phase));
#endif
    err = snd_pcm_writei(pcmIf, pAudBlk, periodFrames);
    if (err < 0) fprintf(stdout, "Initial write error: %s\n", snd_strerror(err));
    if ((uint)err != periodFrames) fprintf(stdout, "Initial write error: written %d expected %lu\n", err, periodFrames);
    }
  if (snd_pcm_state(pcmIf) == SND_PCM_STATE_PREPARED) {
    err = snd_pcm_start(pcmIf);
    if (err < 0) fprintf(stdout, "Start error: %s\n", snd_strerror(err));
    }
  fprintf(stdout, "We Have PCM Output Startage!\n");
  return;
  }
void AlsaStreamOut::Close() {
  err = snd_pcm_close(pcmIf);
  if (err < 0) fprintf(stdout, "PCM Playback close error: %s\n", snd_strerror(err));
  return;
  }
void AlsaStreamOut::_AlsaCallBack(snd_async_handler_t *i_pAsyncHndl) {
  snd_pcm_t     *l_pPcmIf;
  word          *l_pAudBlk;
  AlsaStreamOut *l_pAo;
  uint           l_avail;
  //unsigned long long theRdtsc;
  //__asm__ __volatile__("rdtsc": "=A" (theRdtsc));
  //fprintf(stdout, "%Lu\n", theRdtsc);

  l_pPcmIf    = snd_async_handler_get_pcm(i_pAsyncHndl);
  l_pAo       = (AlsaStreamOut *) snd_async_handler_get_callback_private(i_pAsyncHndl);
  l_pAudBlk   = (word *) l_pAo->pAudBlk;
  l_avail     = (uint) snd_pcm_avail_update(l_pPcmIf);

  while ((l_avail >= l_pAo->periodFrames) && (l_pAo->err >= 0)) {
#ifndef NO_SIGNALS
    l_pAo->MakeWave.emit((long) l_pAo->periodFrames, &(l_pAo->phase));
#endif
    l_pAo->err = snd_pcm_writei(l_pPcmIf, l_pAudBlk, l_pAo->periodFrames);
    if (l_pAo->err < 0) {
      fprintf(stdout, "Write error: %s\n", snd_strerror(l_pAo->err));
    }
    if ((uint) l_pAo->err != l_pAo->periodFrames) {
      fprintf(stdout, "Write error: written %d expected %lu\n", l_pAo->err, l_pAo->periodFrames);
    }
    l_avail = (uint) snd_pcm_avail_update(l_pPcmIf);
  }
}
void AlsaStreamOut::Convert(uint N, double *pL, double *pR) {
  pConvert(N, pL, pR, pAudBlk);
  return;
  }
void AlsaStreamOut::Stop(void) {
  snd_async_del_handler(asyncHndl);
  err = snd_pcm_drop(pcmIf);
  return;
  }
