
// $Id: AlsaStream.hpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaFormat.h
// Original Author    : duncang
// Creation Date      : Apr 2, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
/*
ReThink!  Used to be a lengthy interview and capabilities possibility matrix that try try tried to
  get a legal setup.

NOW

The Alsa class has to have user interaction to set which device to use.  THEN it forces the stream
to connect.  This demands that the Alsa class has properly ferreted out what the chosen card/device
can do so as not to barf.  STILL, the stream has some things that are up to that late-bound setting
which is about buffers and frames.

*/
//
//=================================================================================================

#ifndef ALSASTREAM_H_
#define ALSASTREAM_H_

#include <caes/CaesTypes.hpp>
#include <alsa/asoundlib.h>
#include <caes/Alsa/Alsa.hpp>
#include <caes/Alsa/AlsaFormat.hpp>


class AlsaStream {
  public:
    enum eDirection{
      EDIR_BAD       = -1,
      EDIR_PLAYBACK  = SND_PCM_STREAM_PLAYBACK,
      EDIR_CAPTURE   = SND_PCM_STREAM_CAPTURE
    };
    enum eResamp{AHER_DIS = 0, AHER_EN = 1};
    enum eOpenMode{  // Remake the ALSA enums to be a field-ornented thingy.
      eSND_PCM_NONBLOCK         = 0x00000001LU,
      eSND_PCM_ASYNC            = 0X00000002LU,
      eSND_PCM_NO_AUTO_RESAMPLE = 0x00010000LU,
      eSND_PCM_NO_AUTO_CHANNELS = 0x00020000LU,
      eSND_PCM_NO_AUTO_FORMAT   = 0x00040000LU,
      eSND_PCM_NO_SOFTVOL       = 0x00080000LU,
      eSND_PCM_BIGG             = 0xFFFFFFFFLU // Force it to be a uint32
      };
    enum eBuffSizeMethod{
      ABSM_TIME = 1,
      ABSM_FRAMES,
      ABSM_BYTES
      };
    enum eBuffSetScope{
      ABSS_PERIOD = 1,
      ABSS_BUFFER
      };
  public:
                       AlsaStream(Alsa *i_ae);
                      ~AlsaStream();
    void               ListAllPcmIfs(void);
    void               ListAllDevices(void);
    void               ShowAvalableFormats(void);
    void               SetFS(uint i_FS);
    uint               GetFS(void) {return FS;};
    void               SetCh(uint i_ch);
    uint               GetCh(void) {return ch;};
    size_t             GetFrameBytes(void) {return ch * af->NumBytes();};
    size_t             GetMaxBufferBytes(void) {return maxBufferBytes;};
    void               SetMaxBufferBytes(size_t i_maxBufferBytes);
    ulong              GetPeriodTime(void) {return periodTime;};
    ulong              GetBufferTime(void) {return bufferTime;};
    void               SetBufferTime(ulong i_bufferTime);
    void               SetPeriodTime(ulong i_periodTime);
    snd_pcm_uframes_t  GetBufferFrames(void) {return bufferFrames;};
    snd_pcm_uframes_t  GetPeriodFrames(void) {return periodFrames;};
    void               SetBufferFrames(snd_pcm_uframes_t i_bufferFrames);
    void               SetPeriodFrames(snd_pcm_uframes_t i_periodFrames);
    size_t             GetBufferBytes(void) {return bufferBytes;};
    size_t             GetPeriodBytes(void) {return periodBytes;};
    void               SetBuffPerRatio(uint i_bpr) {buffPerRatio = i_bpr;};
    uint               GetBuffPerRatio(void)        {return buffPerRatio;};
    void               PushFormat(void);
    void               BoinkParams(void);
    void               Setup(void);
    void               Params(void);
    void               Announce(void);
    void               ShowMMapAreas(void);
  private:
    enum               eConts{ASC_MIN_PER = 64};
    void               SetBufferSpace(void);
  public:
    snd_output_t           *logStr;
    FILE                   *errStr;
    char                   *errBuff;
    char                   *deviceName;

    Alsa                   *theAlsa;

    eDirection              direction;
    snd_pcm_t              *pcmIf;
    snd_pcm_hw_params_t    *hwParams;
    snd_pcm_sw_params_t    *swParams;
    AlsaFormat             *af;
    AlsaOpenMode           *aom;

    eResamp                 resample;
    snd_pcm_access_t        buffAccess;

    u_char                 *pAudBlk;

    bool                    memMapped;     // warning ... this is a setup variable that gets packed into ALSA shit for real elsewhere
    bool                    interleaved;   // likewise.

    bool                    monotonic;
    bool                    canPause;

    eBuffSizeMethod         buffSizeMethod;
    eBuffSetScope           buffSetScope;


    int                     availMin;     // in microseconds
    int                     startDelay;   // in microseconds
    int                     stopDelay;

    int                     periodEvent;
    int                     err;

  protected:
    uint                    FS;
    uint                    ch;

    size_t                  maxBufferBytes;
    ulong                   buffPerRatio;

    ulong                   bufferTime;
    ulong                   periodTime;

    size_t                  bufferBytes;
    size_t                  periodBytes;

    snd_pcm_uframes_t       bufferFrames;
    snd_pcm_uframes_t       periodFrames;

  };

#endif // ALSASTREAM_H_
