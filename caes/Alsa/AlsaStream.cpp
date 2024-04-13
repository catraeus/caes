
// $Id: AlsaStream.cpp 224 2022-12-02 00:28:17Z duncang $

//=================================================================================================
// Original File Name : AlsaFormat.cpp
// Original Author    : duncang
// Creation Date      : Apr 2, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//=================================================================================================
/*
    cap is short for Capability.

    a particle is a single piece of information simply stated in a single place.

    fmt   is the (Database NOT normal enum)
    typ   is a data type like exponential, signed, unsigned, etc.
    bits  is the number of bits that mean something.
    bytes is the number of bytes that those bits are stored in on an atomic (hah when it's 3) basis.
    just  is which end of the byte array the bits are shoved to.
    endn  is the representation of the byte order.

    We will NOT allow for reverse-bit-order storage.

    I'm really glad that the ALSA people made snd_pcm_fmt_t into an enum and not a #define!
    However, let-s not get to appreciative ... Wouldn't it be nice if snd_pcm_format_t was entire
      over the set of all possible particle enumeration members.

    Use cases:
      - Take a snd_pcm_fmt_t, check for legality then push it down into the rest of a caps.
      - Given a particle value and a caps struct, see if changing that one particle of the caps
            leaves the new struct legal.
*/

#include <caes/Alsa/AlsaStream.hpp>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <caes/CaesMath.hpp>
#include <caes/CaesTypes.hpp>
//#include "../_PrjGlbl.hpp"

#define error(...) do {\
   sprintf(errBuff, "AlsaStream(): %s:%d: ", __FUNCTION__, __LINE__); \
   sprintf(errBuff, __VA_ARGS__); \
   sprintf(errBuff, "\n"); \
   fprintf(errStr, "%s", errBuff); \
} while (0)


     AlsaStream::AlsaStream(Alsa *i_ae)
: theAlsa(i_ae) {
  logStr        = NULL;
  errBuff       = new char[MAX_LINE_LEN];
  errStr        = stdout;
  pAudBlk       = NULL;
  af            = AlsaFormat::Instance();
  aom           = new AlsaOpenMode();
  direction     = EDIR_BAD;
  hwParams      =    NULL;
  resample      = AHER_DIS;
  buffAccess    = SND_PCM_ACCESS_RW_INTERLEAVED;

  FS            =  48000;
  ch            =      2;

  memMapped     =  false;
  interleaved   =   true;

  monotonic     =  true;
  canPause      =  false;

  buffSizeMethod = ABSM_FRAMES;
  buffSetScope   = ABSS_BUFFER;
  buffPerRatio   =      2;
  maxBufferBytes =  16384;
  periodFrames   =   1024;
  bufferFrames   =   2048;
  periodBytes    =      0;
  bufferBytes    =      0;
  periodTime     =      0;
  bufferTime     =      0;

  availMin      =      -1;
  startDelay    =       0;
  stopDelay     =       0;

  periodEvent   =       0;

  deviceName    =    NULL;
  af->SetFmt(SND_PCM_FORMAT_S32);
  PushFormat();

  err = snd_output_stdio_attach(&logStr, stdout, 0);
  if(err < 0)
    fprintf(stdout, "terminal connection from ALSA failed, verbose messages won't appear: %s\n", snd_strerror(err));
  }
     AlsaStream::~AlsaStream() {
  deviceName = NULL;
  return;
  if(errBuff != NULL)
    delete[] errBuff;

  snd_output_close(logStr);
  fprintf(stdout, "AlsaStream destructor\n");
  }
void AlsaStream::ListAllDevices(void) {
  snd_ctl_t           *ctlIf;
  int                  card;
  int                  err;
  int                  dev;
  int                  idx;
  snd_ctl_card_info_t *infoCard;
  snd_pcm_info_t      *infoPcm;

  snd_ctl_card_info_alloca(&infoCard);
  snd_pcm_info_alloca(&infoPcm);

  card = -1;
  if (snd_card_next(&card) < 0 || card < 0) {
    error("no soundcards found...");
    return;
  }
  printf("**** List of %s Hardware Devices ****\n", snd_pcm_stream_name((snd_pcm_stream_t)direction));
  while (card >= 0) {
    char alsaDeviceName[128];
    sprintf(alsaDeviceName, "hw:%d", card);
    err = snd_ctl_open(&ctlIf, alsaDeviceName, 0);
    if(err < 0) {
      error("control open (%i): %s", card, snd_strerror(err));
      }
    else {
      err = snd_ctl_card_info(ctlIf, infoCard);
      if (err < 0) {
        error("control hardware infoCard (%i): %s", card, snd_strerror(err));
        snd_ctl_close(ctlIf);
        }
      else {
        dev = -1;
        while (1) {
          unsigned int count;
          if (snd_ctl_pcm_next_device(ctlIf, &dev) < 0)
            error("snd_ctl_pcm_next_device");
          if (dev < 0)
            break;
          snd_pcm_info_set_device(infoPcm, dev);
          snd_pcm_info_set_subdevice(infoPcm, 0);
          snd_pcm_info_set_stream(infoPcm, (snd_pcm_stream_t)direction);
          err = snd_ctl_pcm_info(ctlIf, infoPcm);
          if (err < 0) {
            if (err != -ENOENT)
              error("control digital audio infoCard (%i): %s", card, snd_strerror(err));
            continue;
            }
          printf("card %i: %s [%s], device %i: %s [%s]\n", card,
              snd_ctl_card_info_get_id(infoCard),
              snd_ctl_card_info_get_name(infoCard), dev,
              snd_pcm_info_get_id(infoPcm), snd_pcm_info_get_name(infoPcm));
          count = snd_pcm_info_get_subdevices_count(infoPcm);
          fprintf(stdout, "  Subdevices: %i/%i\n",
              snd_pcm_info_get_subdevices_avail(infoPcm), count);
          for (idx = 0; idx < (int) count; idx++) {
            snd_pcm_info_set_subdevice(infoPcm, idx);
            err = snd_ctl_pcm_info(ctlIf, infoPcm);
            if (err < 0) {
              error("control digital audio playback infoCard (%i): %s", card, snd_strerror(err));
              }
            else {
              printf("  Subdevice #%i: %s\n", idx,
                  snd_pcm_info_get_subdevice_name(infoPcm));
              }
            }
          }
        snd_ctl_close(ctlIf);
        }
      }
    if (snd_card_next(&card) < 0) {
      error("snd_card_next");
      break;
      }
    }
  return;
  }
void AlsaStream::ListAllPcmIfs(void) {
        void **hints;
        void **n;
        char  *alsaDeviceName;
        char  *descr;
        char  *descr1;
        char  *io;
  const char  *filter;

  if (snd_device_name_hint(-1, "pcm", &hints) < 0)
    return;
  n = hints;
  if((snd_pcm_stream_t)direction == SND_PCM_STREAM_CAPTURE)
    filter =  "Input";
  else
    filter =  "Output";
  while (*n != NULL) {
  alsaDeviceName = snd_device_name_get_hint(*n, "NAME");
  descr          = snd_device_name_get_hint(*n, "DESC");
  io             = snd_device_name_get_hint(*n, "IOID");
  if (io == NULL || strcmp(io, filter) == 0) {
  printf("%s\n", alsaDeviceName);
  if ((descr1 = descr) != NULL) {
    printf("    ");
    while (*descr1) {
      if (*descr1 == '\n')
        printf("\n    ");
      else
        putchar(*descr1);
      descr1++;
      }
    putchar('\n');
    }
  }
  n++;
  }
  snd_device_name_free_hint(hints);
  return;
  }
void AlsaStream::ShowAvalableFormats(void) {
  fprintf(stderr, "Available formats:\n");
  for (uint i = 0; i < (uint) SND_PCM_FORMAT_LAST; i++) {
    if(snd_pcm_hw_params_test_format(pcmIf, hwParams, (snd_pcm_format_t)i) == 0)
      fprintf(stdout, "- %s\n", snd_pcm_format_name((snd_pcm_format_t)i));
    }
  return;
  }
void AlsaStream::PushFormat(void) {
  double td;

  //fprintf(stdout, "AlsaStream::PushFormat\n");
  switch(buffSetScope) {
    case ABSS_PERIOD:
      switch(buffSizeMethod) {
        case ABSM_TIME:
          td  = (double)periodTime * (double)GetFS() * 0.000001;// because periodTime was in microseconds
          periodFrames = (snd_pcm_uframes_t)(td + DBL_ROUNDER);
          bufferFrames = periodFrames * buffPerRatio;
          bufferBytes  = bufferFrames * GetFrameBytes();
          periodBytes  = periodFrames * GetFrameBytes();
          break;
        case ABSM_BYTES:
          bufferBytes = periodBytes * buffPerRatio;
          bufferFrames = bufferBytes / GetFrameBytes();
          periodFrames = periodBytes / GetFrameBytes();
          break;
        case ABSM_FRAMES:
        default:
          //fprintf(stdout, "AlsaStream::PushFormat %lu\n", (ulong)periodFrames);
          bufferFrames = periodFrames * buffPerRatio;
          bufferBytes  = bufferFrames * GetFrameBytes();
          periodBytes  = periodFrames * GetFrameBytes();
          break;
          }
      break;
    case ABSS_BUFFER:
    default:
      switch(buffSizeMethod) {
        case ABSM_TIME:
          td  = (double)bufferTime * (double)GetFS() * 0.000001;// because periodTime was in microseconds
          bufferFrames = (snd_pcm_uframes_t)(td + DBL_ROUNDER);
          periodFrames = bufferFrames / buffPerRatio;
          bufferBytes  = bufferFrames * GetFrameBytes();
          periodBytes  = periodFrames * GetFrameBytes();
          break;
        case ABSM_BYTES:
          periodBytes  = bufferBytes / buffPerRatio;
          bufferFrames = bufferBytes / GetFrameBytes();
          periodFrames = periodBytes / GetFrameBytes();
          break;
        case ABSM_FRAMES:
          periodFrames = bufferFrames / buffPerRatio;
          bufferBytes  = bufferFrames * GetFrameBytes();
          periodBytes  = periodFrames * GetFrameBytes();
          break;
        default:
          break;
          }
      break;
    }
  td = (double)bufferFrames / (double)GetFS() * 1000000.0;
  bufferTime = (ulong)(td + DBL_ROUNDER);
  td /= (double)buffPerRatio;
  periodTime = (ulong)(td + DBL_ROUNDER);
  SetBufferSpace();
  return;
  }
void AlsaStream::SetFS(uint i_FS) {
  FS = i_FS;
  PushFormat();
  return;
  }
void AlsaStream::SetCh(uint i_ch) {
  ch = i_ch;
  PushFormat();
  return;
  }
void AlsaStream::SetMaxBufferBytes(size_t i_maxBufferBytes) {
  maxBufferBytes = i_maxBufferBytes;
  PushFormat();
  return;
  }
void AlsaStream::SetPeriodTime(ulong i_periodTime) {
  buffSizeMethod = ABSM_TIME;
  buffSetScope   = ABSS_PERIOD;
  periodTime = i_periodTime;
  PushFormat();
  return;
  }
void AlsaStream::SetBufferTime(ulong i_bufferTime) {
  buffSizeMethod = ABSM_TIME;
  buffSetScope   = ABSS_BUFFER;
  bufferTime = i_bufferTime;
  PushFormat();
  return;
  }
void AlsaStream::SetPeriodFrames(ulong i_periodFrames) {
  buffSizeMethod = ABSM_FRAMES;
  buffSetScope   = ABSS_PERIOD;
  periodFrames = i_periodFrames;
  //fprintf(stdout, "AlsaStream::SetPeriodFrames %lu\n", i_periodFrames);
  PushFormat();
  return;
  }
void AlsaStream::SetBufferFrames(ulong i_bufferFrames) {
  buffSizeMethod = ABSM_FRAMES;
  buffSetScope   = ABSS_BUFFER;
  bufferFrames = i_bufferFrames;
  PushFormat();
  return;
  }
void AlsaStream::SetBufferSpace(void) {
  //fprintf(stdout, "-> AlsaStream::SetBufferSpace\n");
  //fprintf(stdout, "    bufferBytes   %8ld, maxBufferBytes %ld\n", (long)bufferBytes, (long)maxBufferBytes);
  if(bufferBytes > maxBufferBytes)
    bufferBytes = maxBufferBytes;
  bufferBytes  -= bufferBytes % (GetFrameBytes() * buffPerRatio);
  bufferFrames  = bufferBytes / GetFrameBytes();
  periodFrames  = bufferFrames / buffPerRatio;
  periodBytes   = periodFrames * GetFrameBytes();
  //fprintf(stdout, "    bufferBytes   %8ld, maxBufferBytes %ld\n", (long)bufferBytes, (long)maxBufferBytes);
  //fprintf(stdout, "    bufferFrames  %8ld\n", (long)bufferFrames);
  //fprintf(stdout, "    periodBytes   %8ld\n", (long)periodBytes);
  //fprintf(stdout, "    periodFrames  %8ld\n", (long)periodFrames);
  double td;
  td = (double)periodFrames / (double)FS * 1000000.0;
  periodTime = (uint)(td + DBL_ROUNDER);
  td = (double)bufferFrames / (double)FS * 1000000.0;
  bufferTime = (uint)(td + DBL_ROUNDER);
  if(pAudBlk != NULL) {
    delete[] pAudBlk;
    pAudBlk = NULL;
    }
  pAudBlk = new u_char[bufferBytes];
  if(pAudBlk == NULL) {
    fprintf(stdout, "Unable to allocate memory for the pAudBlk array.\n");
    err = true;
    }
  //fprintf(stdout, "<- AlsaStream::SetBufferSpace\n");
  return;
  }
void AlsaStream::BoinkParams(void) {
  //err = 0;
  //================
  //Preparatory
    snd_pcm_hw_params_alloca(&hwParams);
    err = snd_pcm_hw_params_any(pcmIf, hwParams);
    if(err < 0) {
      fprintf(stdout, "Broken configuration: no configurations available: %s\n", snd_strerror(err));
      return;
      }

  return;
  }
void AlsaStream::Params(void) {
  size_t            n;
  //uint              rate;
  snd_pcm_uframes_t start_threshold;
  snd_pcm_uframes_t stop_threshold;

  pAudBlk = (u_char *)malloc(GetMaxBufferBytes());
  if(pAudBlk == NULL) {
    error("not enough memory");
    err = -1;
    return;
    }

  fprintf(stdout, "In Param:  deviceName = %s  open mode bitfield: 0x%08x\n", deviceName, aom->GetBitField());
  err = snd_pcm_open(&(pcmIf), deviceName, (snd_pcm_stream_t)(direction), SND_PCM_NONBLOCK + SND_PCM_NO_AUTO_FORMAT );
  if(err < 0) {
    fprintf(stdout, "audio open error: %s\n", snd_strerror(err));
    return;
    }
//  if(aom->GetNonBlock()) {
//    fprintf(stdout, "setting to non-blocking\n");
//    err = snd_pcm_nonblock(pcmIf, 1);
//    if(err < 0) {
//      fprintf(stdout, "nonblock setting error: %s", snd_strerror(err));
//      return;
//      }
//    }
//  else
//    fprintf(stdout, "setting to blocking\n");

  err = 0;
  //================
  //Preparatory
  snd_pcm_hw_params_alloca(&hwParams);
  err = snd_pcm_hw_params_any(pcmIf, hwParams);
  if(err < 0) {
    fprintf(stdout, "Broken configuration: no configurations available: %s\n", snd_strerror(err));
    return;
    }
//#define PRINT_RATES
//#define PRINT_PERIODS
//#define PRINT_FORMATS
//#define SPILL_GUTS

#ifdef PRINT_RATES
  {
  for(uint theRate = 0; theRate < 200000; theRate++) {
    int k;
    k = snd_pcm_hw_params_test_rate(pcmIf, hwParams, theRate, 0);
    if(k == 0)
      fprintf(stdout, "  rate %6d result %d\n", theRate, k);
    }
  }
#endif
#ifdef PRINT_PERIODS
  {
  for(uint thePeriod = 0; thePeriod < 200000; thePeriod++) {
    int k;
    k = snd_pcm_hw_params_test_periods(pcmIf, hwParams, thePeriod, 0);
    if(k == 0)
      fprintf(stdout, "  periods %6d result %d\n", thePeriod, k);
    }
  }
#endif
#ifdef PRINT_FORMATS
  {
    AlsaFormat *theFormat;
    //char b[256];
    char *s;

    theFormat = new AlsaFormat();
    //b[0] = '\0';
    for(uint i = 0; i<theFormat->numLimited; i++) {
      int k = snd_pcm_hw_params_test_format(pcmIf, hwParams, theFormat->capsLimited[i].fmt);
      s = (char *)snd_pcm_format_description(theFormat->capsLimited[i].fmt);
      if(k >= 0)
        fprintf(stdout, "   can do --  %s\n", s);
      }
    fprintf(stdout, "\n");
  }
#endif
#ifdef SPILL_GUTS
  {
    int k;
    char *ss;
    snd_pcm_info_t *theInfo;

    snd_pcm_info_malloc(&theInfo);
    snd_pcm_info (pcmIf, theInfo);
    ss = (char *)snd_pcm_name(pcmIf);
    fprintf(stdout, "snd_pcm_name = %s\n", ss);
    k = snd_pcm_hw_params_can_mmap_sample_resolution(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_mmap_sample_resolution = %d\n", k);
    k = snd_pcm_hw_params_is_double(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_double = %d\n", k);
    k = snd_pcm_hw_params_is_batch(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_batch = %d\n", k);
    k = snd_pcm_hw_params_is_block_transfer(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_block_transfer = %d\n", k);
    k = snd_pcm_hw_params_is_monotonic(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_monotonic = %d\n", k);
    k = snd_pcm_hw_params_can_overrange(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_overrange = %d\n", k);
    k =   snd_pcm_hw_params_can_pause(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_pause = %d\n", k);
    k = snd_pcm_hw_params_can_resume(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_resume = %d\n", k);
    k = snd_pcm_hw_params_is_half_duplex(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_half_duplex = %d\n", k);
    k = snd_pcm_hw_params_is_joint_duplex(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_is_joint_duplex = %d\n", k);
    k = snd_pcm_hw_params_can_sync_start(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_sync_start = %d\n", k);
    k = snd_pcm_hw_params_can_disable_period_wakeup(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_can_disable_period_wakeup = %d\n", k);
    uint numerator;
    uint denominator;
    k = snd_pcm_hw_params_get_rate_numden(hwParams, &numerator, &denominator);
    fprintf(stdout, "snd_pcm_hw_params_get_rate_numden = %d   numerator: %d   denominator: %d\n", k, numerator, denominator);
    k = snd_pcm_hw_params_get_sbits(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_get_sbits = %d\n", k);
    k = snd_pcm_hw_params_get_fifo_size(hwParams);
    fprintf(stdout, "snd_pcm_hw_params_get_fifo_size = %d\n", k);
    k = snd_pcm_hw_params_get_channels_min(hwParams, &numerator);
    fprintf(stdout, "snd_pcm_hw_params_get_channels_min = %d value: %d\n", k, numerator);
    k = snd_pcm_hw_params_get_channels_max(hwParams, &numerator);
    fprintf(stdout, "snd_pcm_hw_params_get_channels_max = %d value: %d\n", k, numerator);
    snd_pcm_uframes_t theSize;
    k = snd_pcm_hw_params_get_buffer_size_min(hwParams, &theSize);
    fprintf(stdout, "snd_pcm_hw_params_get_buffer_size_min = %d value: %lu\n", k, theSize);
    k = snd_pcm_hw_params_get_buffer_size_max(hwParams, &theSize);
    fprintf(stdout, "snd_pcm_hw_params_get_buffer_size_max = %d value: %lu\n", k, theSize);
    k = snd_pcm_hw_params_get_period_size_min(hwParams, &theSize, 0);
    fprintf(stdout, "snd_pcm_hw_params_get_period_size_min = %d value: %lu\n", k, theSize);
    k = snd_pcm_hw_params_get_period_size_max(hwParams, &theSize, 0);
    fprintf(stdout, "snd_pcm_hw_params_get_period_size_max = %d value: %lu\n", k, theSize);
    k = snd_pcm_hw_params_get_min_align(hwParams, &theSize);
    fprintf(stdout, "snd_pcm_hw_params_get_min_align = %d value: %lu\n\n", k, theSize);
  }
#endif
  //================
  //Fixed
  err = snd_pcm_hw_params_set_rate_resample(pcmIf, hwParams, resample);
  if(err < 0) {
    fprintf(stdout, "Resampling setup failed: %s\n", snd_strerror(err));
    return;
    }

  //================
  //Per Session - Chosen
  uint tFS;
  tFS = FS;
  err = snd_pcm_hw_params_set_rate_near(pcmIf, hwParams, &(tFS), 0);
  if(err < 0) {
    fprintf(stdout, "Rate %iHz had problems ... shouldn't happen ... hhhmmmmmm ... ALSA sez:  %s\n", tFS, snd_strerror(err));
    return;
    }
  fprintf(stdout, "Sample Rate Requested:  %d Hz  --  Granted:  %d Hz)\n", GetFS(), tFS);
  SetFS(tFS);

  //================
  //Fixed
  if(memMapped) {
    snd_pcm_access_mask_t *mask = (snd_pcm_access_mask_t *)alloca(snd_pcm_access_mask_sizeof());
    snd_pcm_access_mask_none(mask);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_INTERLEAVED);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_COMPLEX);
    err = snd_pcm_hw_params_set_access_mask(pcmIf, hwParams, mask);
    if(err < 0) {
      error("Memory Mapped Access type not available");
      return;
      }
    fprintf(stdout, "Access is Memory Mapped\n");
  }
  else if(interleaved) {
    err = snd_pcm_hw_params_set_access(pcmIf, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(err < 0) {
      error("Interleaved Read/Write Access type not available");
      return;
      }
    fprintf(stdout, "Access is RW Interleaved\n");
    }
  else {
    err = snd_pcm_hw_params_set_access(pcmIf, hwParams, SND_PCM_ACCESS_RW_NONINTERLEAVED);
    if(err < 0) {
      error("Non-Interleaved Read/Write Access type not available");
      return;
      }
    fprintf(stdout, "Access is RW Non-Interleaved\n");
    }

  //================
  //Per Session - Chosen
  err = snd_pcm_hw_params_set_format(pcmIf, hwParams, af->caps.fmt);
  if(err < 0) {
    fprintf(stdout, "Sample format not available: %s\n", snd_strerror(err));
    ShowAvalableFormats();
    return;
    }

  //================
  //Per Session - Chosen
  err = snd_pcm_hw_params_set_channels(pcmIf, hwParams, ch);
  if(err < 0) {
    fprintf(stdout, "Channels count (%i) not available: %s\n", ch, snd_strerror(err));
    return;
    }


//=================================================================================================
  //fprintf(stdout, "before trials        --  periodFrames: %7lu     bufferFrames: %7lu    periodTime: %7lu     bufferTime: %7lu\n", periodFrames, bufferFrames, periodTime, bufferTime);
  // If you don't set buffers then you musn't set periods.
  //fprintf(stdout, "before ... maxBufferBytes: %lu\n", (ulong)maxBufferBytes);
  snd_pcm_hw_params_get_buffer_size_max(hwParams, (snd_pcm_uframes_t *)&(maxBufferBytes));
  maxBufferBytes *= GetFrameBytes();
  //fprintf(stdout, "after  ... maxBufferBytes: %lu\n", (ulong)maxBufferBytes);
  PushFormat();
  //fprintf(stdout, "after  ... maxBufferBytes: %lu\n", (ulong)maxBufferBytes);
  //fprintf(stdout, "after maxing by ALSA --  periodFrames: %7lu     bufferFrames: %7lu    periodTime: %7lu     bufferTime: %7lu\n", periodFrames, bufferFrames, periodTime, bufferTime);

  err = snd_pcm_hw_params_set_period_size_near(pcmIf, hwParams, (snd_pcm_uframes_t *)&(periodFrames), 0);
  if(err < 0) {
    error("Problem asking for period times or sizes ... should never happen ... Hmmmmmmm");
    return;
    }
  err = snd_pcm_hw_params_set_buffer_size_near(pcmIf, hwParams, &(bufferFrames));
  if(err < 0) {
    error("Problem asking for buffer times or sizes ... should never happen ... Hmmmmmmm");
    return;
    }
  //fprintf(stdout, "after  trials        --  periodFrames: %7lu     bufferFrames: %7lu    periodTime: %7lu     bufferTime: %7lu\n", periodFrames, bufferFrames, periodTime, bufferTime);
  //==
  //fprintf(stdout, "before installation  --  periodFrames: %7lu     bufferFrames: %7lu    periodTime: %7lu     bufferTime: %7lu\n", periodFrames, bufferFrames, periodTime, bufferTime);
  snd_pcm_hw_params_get_period_size(hwParams,                      &(periodFrames), 0);
  snd_pcm_hw_params_get_buffer_size(hwParams, (snd_pcm_uframes_t *)&(bufferFrames)   );
  //fprintf(stdout, "after  installation  --  periodFrames: %7lu     bufferFrames: %7lu    periodTime: %7lu     bufferTime: %7lu\n", periodFrames, bufferFrames, periodTime, bufferTime);
  if(periodFrames == bufferFrames) {
    error("Can't use period equal to buffer size (%lu == %lu)", periodFrames, bufferFrames);
    return;
    }



  //==
  monotonic = snd_pcm_hw_params_is_monotonic(hwParams);
  //==
  canPause = snd_pcm_hw_params_can_pause(hwParams);

  //================
  //Committ
  err = snd_pcm_hw_params(pcmIf, hwParams);
  if(err < 0) {
    error("Unable to install hw params:");
    snd_pcm_hw_params_dump(hwParams, logStr);
    return;
    }
#define SPILL_LATER_GUTS
#ifdef SPILL_LATER_GUTS
  {
  int k;
  snd_pcm_uframes_t theSize;
  k = snd_pcm_hw_params_get_min_align(hwParams, &theSize);
  fprintf(stdout, "snd_pcm_hw_params_get_min_align = %d value: %lu\n", k, theSize);
  k = snd_pcm_hw_params_get_buffer_size(hwParams, &theSize);
  fprintf(stdout, "snd_pcm_hw_params_get_buffer_size = %d value: %lu\n", k, theSize);
  k = snd_pcm_hw_params_get_buffer_time(hwParams, (uint *)(&theSize), 0);
  fprintf(stdout, "snd_pcm_hw_params_get_buffer_time = %d value: %lu\n", k, theSize);
  k = snd_pcm_hw_params_get_period_size(hwParams, &theSize, 0);
  fprintf(stdout, "snd_pcm_hw_params_get_period_size = %d value: %lu\n", k, theSize);
  k = snd_pcm_hw_params_get_period_time(hwParams, (uint *)(&theSize), 0);
  fprintf(stdout, "snd_pcm_hw_params_get_period_time = %d value: %lu\n", k, theSize);
  k = snd_pcm_hw_params_get_fifo_size(hwParams);
  fprintf(stdout, "snd_pcm_hw_params_get_fifo_size = %d\n\n", k);
  }
#endif

//=================================================================================================
  snd_pcm_sw_params_alloca(&swParams);
  snd_pcm_sw_params_current(pcmIf, swParams);
  if(availMin <= 0)
    n = periodFrames;
  else
    n = (size_t)((double)FS * (double)availMin / 1000000.0);
  err = snd_pcm_sw_params_set_avail_min(pcmIf, swParams, n);

//=================================================================================================
// Start Threshold
  // round up to closest transfer boundary
  n = bufferFrames;
  if(startDelay <= 0)
    start_threshold = n + (size_t)((double)FS * (double)startDelay / 1000000.0);
  else
    start_threshold =     (size_t)((double)FS * (double)startDelay / 1000000.0);
  if(start_threshold < 1)
    start_threshold = 1;
  if(start_threshold > n)
    start_threshold = n;
  err = snd_pcm_sw_params_set_start_threshold(pcmIf, swParams, start_threshold);
  if(err < 0) {
    error("Problem asking for Start Threshold ... should never happen ... Hmmmmmmm");
    return;
    }

//=================================================================================================
// Stop Threshold
  if(stopDelay <= 0)
    stop_threshold = bufferFrames + (double) GetFS() * stopDelay / 1000000;
  else
    stop_threshold = (double) GetFS() * stopDelay / 1000000;
  err = snd_pcm_sw_params_set_stop_threshold(pcmIf, swParams, stop_threshold);
  if(err < 0) {
    error("Problem asking for Stop Threshold ... should never happen ... Hmmmmmmm");
    return;
    }

  err = snd_pcm_sw_params(pcmIf, swParams);
  if(err < 0) {
    error("unable to install sw params:");
    snd_pcm_sw_params_dump(swParams, logStr);
    return;
    }


  periodBytes = periodFrames * GetFrameBytes();
  SetBufferSpace();

  ShowMMapAreas();

  return;
  }
void AlsaStream::Setup(void) {
  int dir;
  ulong k;


  err = 0;
  //================
  //Preparatory
  snd_pcm_hw_params_alloca(&(hwParams));
  err = snd_pcm_hw_params_any(pcmIf, hwParams);
  if(err < 0) {
    fprintf(stdout, "Broken configuration: no configurations available: %s\n", snd_strerror(err));
    return;
    }

//#define PRINT_RATES
//#define PRINT_PERIODS
//#define PRINT_FORMATS
//#define SPILL_GUTS

  #ifdef PRINT_RATES
    {
    for(uint theRate = 0; theRate < 200000; theRate++) {
      int k;
      k = snd_pcm_hw_params_test_rate(pcmIf, hwParams, theRate, 0);
      if(k == 0)
        fprintf(stdout, "  rate %6d result %d\n", theRate, k);
      }
    }
  #endif
  #ifdef PRINT_PERIODS
    {
    for(uint thePeriod = 0; thePeriod < 200000; thePeriod++) {
      int k;
      k = snd_pcm_hw_params_test_periods(pcmIf, hwParams, thePeriod, 0);
      if(k == 0)
        fprintf(stdout, "  periods %6d result %d\n", thePeriod, k);
      }
    }
  #endif
  #ifdef PRINT_FORMATS
    {
      AlsaFormat *theFormat;
      char b[256];
      char *s;

      theFormat = new AlsaFormat();
      b[0] = '\0';
      for(uint i = 0; i<theFormat->numLimited; i++) {
        int k = snd_pcm_hw_params_test_format(pcmIf, hwParams, theFormat->capsLimited[i].fmt);
        s = b;
        if(k < 0)
          s = (char *)snd_strerror(k);
        fprintf(stdout, "   fmt[%04d] = %d result: %d  %s\n", i, theFormat->capsLimited[i].fmt, k, s);
        }
      fprintf(stdout, "\n");
    }
  #endif
  #ifdef SPILL_GUTS
    {
      int k;

      k = snd_pcm_hw_params_can_mmap_sample_resolution(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_mmap_sample_resolution = %d\n", k);
      k = snd_pcm_hw_params_is_double(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_double = %d\n", k);
      k = snd_pcm_hw_params_is_batch(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_batch = %d\n", k);
      k = snd_pcm_hw_params_is_block_transfer(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_block_transfer = %d\n", k);
      k = snd_pcm_hw_params_is_monotonic(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_monotonic = %d\n", k);
      k = snd_pcm_hw_params_can_overrange(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_overrange = %d\n", k);
      k =   snd_pcm_hw_params_can_pause(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_pause = %d\n", k);
      k = snd_pcm_hw_params_can_resume(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_resume = %d\n", k);
      k = snd_pcm_hw_params_is_half_duplex(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_half_duplex = %d\n", k);
      k = snd_pcm_hw_params_is_joint_duplex(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_is_joint_duplex = %d\n", k);
      k = snd_pcm_hw_params_can_sync_start(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_sync_start = %d\n", k);
      k = snd_pcm_hw_params_can_disable_period_wakeup(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_can_disable_period_wakeup = %d\n", k);
      uint numerator;
      uint denominator;
      k = snd_pcm_hw_params_get_rate_numden(hwParams, &numerator, &denominator);
      fprintf(stdout, "snd_pcm_hw_params_get_rate_numden = %d   numerator: %d   denominator: %d\n", k, numerator, denominator);
      k = snd_pcm_hw_params_get_sbits(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_get_sbits = %d\n", k);
      k = snd_pcm_hw_params_get_fifo_size(hwParams);
      fprintf(stdout, "snd_pcm_hw_params_get_fifo_size = %d\n", k);
      k = snd_pcm_hw_params_get_channels_min(hwParams, &numerator);
      fprintf(stdout, "snd_pcm_hw_params_get_channels_min = %d value: %d\n", k, numerator);
      k = snd_pcm_hw_params_get_channels_max(hwParams, &numerator);
      fprintf(stdout, "snd_pcm_hw_params_get_channels_max = %d value: %d\n", k, numerator);
      snd_pcm_uframes_t bufferSize;
      k = snd_pcm_hw_params_get_buffer_size(hwParams, &bufferSize);
      fprintf(stdout, "snd_pcm_hw_params_get_buffer_size = %d value: %lu\n\n", k, bufferSize);
    }
  #endif
  //================
  //Fixed
  err = snd_pcm_hw_params_set_rate_resample(pcmIf, hwParams, resample);
  if(err < 0) {
    fprintf(stdout, "Resampling setup failed: %s\n", snd_strerror(err));
    return;
    }

  //================
  //Per Session - Chosen
  uint tFS;
  tFS = FS;
  err = snd_pcm_hw_params_set_rate_near(pcmIf, hwParams, &(tFS), 0);
  if(err < 0) {
    fprintf(stdout, "Rate %iHz had problems ... shouldn't happen ... hhhmmmmmm ... ALSA sez:  %s\n", tFS, snd_strerror(err));
    return;
    }
  fprintf(stdout, "Sample Rate Requested:  %d Hz  --  Granted:  %d Hz)\n", GetFS(), tFS);
  SetFS(tFS);

  //================
  //Fixed
  if(memMapped) {
    snd_pcm_access_mask_t *mask = (snd_pcm_access_mask_t *)alloca(snd_pcm_access_mask_sizeof());
    snd_pcm_access_mask_none(mask);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_INTERLEAVED);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_MMAP_COMPLEX);
    err = snd_pcm_hw_params_set_access_mask(pcmIf, hwParams, mask);
    if(err < 0) {
      error("Memory Mapped Access type not available");
      return;
      }
    fprintf(stdout, "Access is Memory Mapped\n");
  }
  else if(interleaved) {
    err = snd_pcm_hw_params_set_access(pcmIf, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if(err < 0) {
      error("Interleaved Read/Write Access type not available");
      return;
      }
    fprintf(stdout, "Access is RW Interleaved\n");
    }
  else {
    err = snd_pcm_hw_params_set_access(pcmIf, hwParams, SND_PCM_ACCESS_RW_NONINTERLEAVED);
    if(err < 0) {
      error("Non-Interleaved Read/Write Access type not available");
      return;
      }
    fprintf(stdout, "Access is RW Non-Interleaved\n");
    }

  //================
  //Per Session - Chosen
  err = snd_pcm_hw_params_set_format(pcmIf, hwParams, af->caps.fmt);
  if(err < 0) {
    fprintf(stdout, "Sample format not available: %s\n", snd_strerror(err));
    ShowAvalableFormats();
    return;
    }

  //================
  //Per Session - Chosen
  err = snd_pcm_hw_params_set_channels(pcmIf, hwParams, ch);
  if(err < 0) {
    fprintf(stdout, "Channels count (%i) not available: %s\n", ch, snd_strerror(err));
    return;
    }

  //================
  //Per Session - Derived
  k = bufferFrames;
  err = snd_pcm_hw_params_set_periods_integer (pcmIf, hwParams);
  if(err < 0) {
    fprintf(stdout, "Unable to set buffer size to integer periods: %s\n", snd_strerror(err));
    return;
    }
  err = snd_pcm_hw_params_set_buffer_size_near(pcmIf, hwParams, (snd_pcm_uframes_t *)&(bufferFrames));
  if(err < 0) {
    fprintf(stdout, "Unable to set buffer size %lu: %s\n", bufferFrames, snd_strerror(err));
    return;
    }
  fprintf(stdout, "Buffer -- Requested:  %lu     Granted:  %lu\n", k, bufferFrames);

  //================
  //Per Session - Derived
  k = periodFrames;
  //snd_pcm_uframes_t blabla;
  //blabla = 2048;
  err = snd_pcm_hw_params_set_period_size_near(pcmIf, hwParams, (snd_pcm_uframes_t *)(&periodFrames), &dir);
  //err = snd_pcm_hw_params_set_period_size_near(pcmIf, hwParams, &blabla, &dir);
  //periodFrames = blabla;
  if(err < 0) {
    fprintf(stdout, "Unable to set period size %lu: %s\n", periodFrames, snd_strerror(err));
    return;
    }
  fprintf(stdout, "Period -- Requested:  %lu     Granted:  %lu\n", k, periodFrames);

  //==
  monotonic = snd_pcm_hw_params_is_monotonic(hwParams);
  //==
  canPause = snd_pcm_hw_params_can_pause(hwParams);

  //================
  //Committ
  err = snd_pcm_hw_params(pcmIf, hwParams);
  if(err < 0) {
    fprintf(stdout, "Unable to set hw hwparams: %s\n", snd_strerror(err));
    return;
    }

  buffSizeMethod = ABSM_FRAMES;
  buffSetScope   = ABSS_BUFFER;
  SetBufferSpace();


  //=================================================================================================
  //================
  //Preparatory
  snd_pcm_sw_params_alloca(&swParams);
  err = snd_pcm_sw_params_current(pcmIf, swParams);
  if(err < 0) {
    fprintf(stdout, "Unable to determine current swParams: %s\n", snd_strerror(err));
    return;
    }
  err = snd_pcm_sw_params_set_start_threshold(pcmIf, swParams, bufferFrames);
  if(err < 0) {
    fprintf(stdout, "Unable to set start threshold mode: %s\n", snd_strerror(err));
    return;
    }
  err = snd_pcm_sw_params_set_avail_min(pcmIf, swParams, periodFrames);
  if(err < 0) {
    fprintf(stdout, "Unable to set avail min: %s\n", snd_strerror(err));
    return;
    }
  err = snd_pcm_sw_params_set_period_event(pcmIf, swParams, periodEvent);
  if(err < 0) {
    fprintf(stdout, "Unable to set period event: %s\n", snd_strerror(err));
    return;
    }

  //================
  //Committ
  err = snd_pcm_sw_params(pcmIf, swParams);
  if(err < 0) {
    fprintf(stdout, "Unable to set swparams: %s\n", snd_strerror(err));
    return;
    }
  return;
  }
void AlsaStream::Announce(void) {
  fprintf(stdout, "device name:   %s\n",    deviceName);
  fprintf(stdout, "Sample Rate:   %i Hz\n", FS);
  fprintf(stdout, "Format:        %s\n",    snd_pcm_format_name(af->caps.fmt));
  fprintf(stdout, "Channels:      %i\n",    ch);
  fprintf(stdout, "periodFrames:  %lu\n",   periodFrames);
  snd_pcm_dump(pcmIf, logStr);
  return;
  }
void AlsaStream::ShowMMapAreas(void) {
  if(memMapped) {
    const snd_pcm_channel_area_t *areas;
          snd_pcm_uframes_t       offset;
          snd_pcm_uframes_t       size;

    size = periodFrames;
    err = snd_pcm_mmap_begin(pcmIf, &areas, &offset, &size);
    if(err < 0) {
      error("snd_pcm_mmap_begin problem: %s", snd_strerror(err));
      }
    else {
    for (uint i = 0; i < (uint) ch; i++)
      fprintf(stdout, "mmap_area[%i] = %p,%u,%u (%u)\n", i, areas[i].addr, areas[i].first, areas[i].step,
          snd_pcm_format_physical_width(af->caps.fmt));
      }
    snd_pcm_mmap_commit(pcmIf, offset, 0);
    }
  return;
  }
