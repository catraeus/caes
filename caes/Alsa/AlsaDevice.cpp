//
// $Id: AlsaDevice.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaDevice.cpp
// Original Author    : duncang
// Creation Date      : Dec 18, 2010
// Copyright          : Copyright © 2011 - 2022 by Catraeus and Duncan Gray
//
// Description:
//     Fourth level down
//
//=================================================================================================

#include <stdio.h>
#include <malloc.h>

#include <caes/CaesString.hpp>

#include "AlsaDevice.hpp"
#include "AlsaIfTypes.hpp"
#include "AlsaStream.hpp"
//#include "../_PrjGlbl.hpp"


#ifdef PRINT_VERBOSE
#define PRINT_DEV0(io, msg)           fprintf(io, msg, no)
#define PRINT_DEV1(io, msg, no1)      fprintf(io, msg, no1)
#define PRINT_DEV2(io, msg, no1, no2) fprintf(io, msg, no1, no2)
#define PRINT_DEVx(io, msg)           fprintf(io, msg)
#else
#define PRINT_DEV0(io, dev)
#define PRINT_DEV1(io, dev, no1)
#define PRINT_DEV2(io, dev, no1, no2)
#define PRINT_DEVx(io, msg)
#endif


      AlsaDevice::AlsaDevice(ullong  i_cardNo, ullong i_ifNo, ullong i_devNo, char *i_hint)
: cardNo(i_cardNo)
, ifNo(i_ifNo)
, devNo(i_devNo) {
#ifdef PRINT_FUNCTION
        fprintf(stdout, "AlsaDevice::AlsaDevice(cardNo(%d), ifNo(%d), devNo(%d), hintClean(%s))\n", i_cardNo, i_ifNo, i_devNo, i_hint); fflush(stdout);
#endif


  hintClean     = new char[MAX_LIST_LEN]; // CR changed to & to stay away from the ALSA | character they  use as a delimiter.
  hintALSA      = new char[MAX_LIST_LEN]; // With the CR character

  AADevNm       = new char[MAX_NAME_LEN];
  AACardNm      = new char[MAX_NAME_LEN];
//AADevNo is an integer, though sscanf'd from the hint.
  AADesc        = new char[MAX_NAME_LEN];
  AAIoId        = new char[MAX_NAME_LEN];

  pcmSpec       = new char[MAX_NAME_LEN];

  FSs           = new ullong[MAX_NAME_LEN];

  fmts          = new snd_pcm_format_t[MAX_NAME_LEN];
  fmtStrs       = new char *[MAX_NAME_LEN];
  fmts          = new snd_pcm_format_t[MAX_LINE_LEN];

  Rescan(i_hint);

  return;
}
      AlsaDevice::~AlsaDevice() {
  Wipe();
  delete(hintClean);
  delete(hintALSA);
  delete(AADevNm);
  delete(AACardNm);
  delete(AADesc);
  delete(AAIoId);
  delete(pcmSpec);
  delete(FSs);
  delete(fmts);
  delete(fmtStrs);
  delete(fmts);
  return;
}

void  AlsaDevice::Wipe(void) {
  numFSs       = 0;
  for(ullong fsDex=0; fsDex<MAX_NAME_LEN; fsDex++)
    FSs[fsDex] = 0;
  FSCurr       = 0;

  numFmts       = 0;
  fmtCurr       = 0;


  hintClean[0]  = '\0';
  hintALSA[0]   = '\0';
  CleanHint();

  AADevNm[0]    = '\0';

  AACardNm[0]   = '\0';
  AADevNo       =   0 ;
  AADesc[0]     = '\0';
  AAIoId[0]     = '\0';

  pcmSpec[0]    = '\0';

  hasAADevNo      = false;
  present       = false;
  busy          = false;
  canTX         = false;
  canTX         = false;
  devErr        = false;

  return;
}

void  AlsaDevice::Rescan(char *i_hint) {
  char *pSrc;
  char *tPtr;
  char *tStr = new char[32768];
#ifdef PRINT_FUNCTIO
  fprintf(stdout, "AlsaDevice::Rescan()\n",hintALSA); fflush(stdout);
#endif

  Wipe();

  strcpy(hintALSA, i_hint);
  strcpy(hintClean, i_hint);
  CleanHint();
// FIXME ... Currently, I am assuming position dependency, NAME -> DEV -> DESC -> IOID  But that might break someday

  pSrc = hintClean;
  PRINT_DEV2(stdout, "                hintClean[%d]:    [%s]\n", devNo, hintClean); fflush(stdout);

  if(strncmp(pSrc, "NAME", 4) != 0) {
    fprintf(stderr, "poop\n"); fflush(stderr);
    Wipe();
    delete tStr;
    return;
  }

  pSrc = GetPast((char *)"NAME", pSrc);
  if(pSrc != NULL) {
    pSrc = CopyUntil((char *)":CARD=", pSrc, AADevNm);
    PRINT_DEV1(stdout, "            AADevNm [%s]\n", AADevNm); fflush(stdout);
    }
  else {
    PRINT_DEVx(stdout, "            AADevNm [<none>]}\n"); fflush(stdout);
    fprintf(stderr, "poop\n"); fflush(stderr);
    Wipe();
    delete tStr;
    return;
  }

  if(pSrc != NULL) {
    pSrc = GetPast((char *)":CARD=", pSrc);
  }
//====
//  Now it gets tricksy ... there might or might not be a ,DEV=
  tPtr = FindAnyCharList(pSrc, (char *)",DEV=");
  if(tPtr == NULL) { // Nope, so look for their (ALSA'a) field delimiter the | character
#ifdef PRINT_VERBOSE
    fprintf(stdout, "NO GOOD FINDING ,DEV=    .%s.\n", pSrc);
#endif
    pSrc = CopyUntil((char *)"|DESC", pSrc, tStr);
    strcpy(AACardNm,          tStr);
    AADevNo = 1000; // MAGICK this should be way past any reasonable machine, it is a violation of all that is holy but I also have the bool
    hasAADevNo = false;
  }
  else { // So we did get the DEV in there somewhere.
    pSrc = CopyUntil((char *)",DEV=", pSrc, tStr);
    strcpy(AACardNm,          tStr);
    pSrc = GetPast  ((char *)",DEV=", pSrc);
    pSrc = CopyUntil((char *)"|DESC", pSrc, tStr);  // WARNING All seem to have DESC
    sscanf(tStr, "%llu", &AADevNo);
    hasAADevNo = true;
  }
  PRINT_DEV1(stdout, "            AADevNm [%s]\n", AADevNm); fflush(stdout);
  PRINT_DEV1(stdout, "            AACardNm [%s]\n", AACardNm); fflush(stdout);





//======================
// All seem to have DESC next.

  if(pSrc != NULL) {
    pSrc = GetPast  ((char *)"|DESC", pSrc);
    }
  else {
    PRINT_DEVx(stdout, "            AADesc [<none>]\n"); fflush(stdout);
    fprintf(stderr, "poop\n"); fflush(stderr);
    Wipe();
    delete tStr;
    return;
  }

  tPtr = FindAnyCharList(pSrc, (char *)"|IOID"); // WARNING Assuming IOID comes next.
  if(tPtr == NULL) { // So, this is the end of the line, just copy and get out of here.
#ifdef PRINT_VERBOSE
    fprintf(stdout, "NO GOOD FINDING IOID    .%s.\n", pSrc);
#endif
    strcpy(AADesc, pSrc);
    strcpy(AAIoId, "<none>");
  }
  else {
    pSrc = CopyUntil((char *)"|IOID", pSrc, tStr);
    strcpy(AADesc,          tStr);
    pSrc = GetPast  ((char *)"|IOID", pSrc);
    strcpy(AAIoId, pSrc);
  }
  PRINT_DEV1(stdout, "            AADesc [%s]\n", AADesc); fflush(stdout);
  PRINT_DEV1(stdout, "            AAIoId [%s]\n", AAIoId); fflush(stdout);

// WONDERFUL
  present = true;

//==== Build a PCM Spec MAGICK this seems to be the pattern, TODO find out if ALSA has a convenience function.
  strcpy(tStr, AADevNm);
  strcat(tStr, ":");
  strcat(tStr, AACardNm);
  if(hasAADevNo) {
    strcat(tStr, ",");
    sprintf(pcmSpec, "%s%llu",tStr, AADevNo);
  }
  else
    strcpy(pcmSpec, tStr);
  //if((strcmp(AADevNm, "hw") == 0) && (ifNo == AlsaIfTypes::EAIT_PCM))
    FindCaps();
  delete tStr;


  return;
}

void  AlsaDevice::Print(void) {
#ifdef PRINT_FUNCTION
  fprintf(stdout, "AlsaDevice::Print()\n"); fflush(stdout);
#endif
  char *tStr;
  tStr = new char[256];

  sprintf(tStr, "Device-----[%02lld]  cardNo: %2lld", devNo, cardNo);
//PadSpaceLead(tStr, 4);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  AACardNm: .%s.", AACardNm);
  PadSpaceTrail(tStr, 27);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  ifNo: %2lld", ifNo);
//  PadSpaceLead(tStr, 4);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  IfName: .%s.", AlsaIfTypes::typeNames[ifNo]);
  PadSpaceTrail(tStr, 17);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr,   "  AADevNm: .%s.", AADevNm);
  PadSpaceTrail(tStr, 25);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  HintDev: %4lld", AADevNo);
//PadSpaceLead(tStr, 4);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  HintDesc1: .%s.", AADesc);
  PadSpaceTrail(tStr, 124);
  fprintf(stdout, "%s", tStr); fflush(stdout);

  sprintf(tStr, "  HintIoId: .%s.", AAIoId);
  PadSpaceTrail(tStr, 26);
  fprintf(stdout, "%s\n", tStr); fflush(stdout);

  delete(tStr);
  return;
}

void  AlsaDevice::SetFmt(ullong i_fmtCurr) {

  return;
}

void  AlsaDevice::FindCaps(void) {
#ifdef PRINT_FUNCTION
  fprintf(stdout, "AlsaDevice::FindCaps()\n");
#endif
  snd_pcm_t           *pcmIf;
  snd_pcm_hw_params_t *hwParams;
  int                  err;


#ifdef PRINT_VERBOSE
  fprintf(stdout, "card[%d]  if[%d]  dev[%d]  AADevNm[%s]  AACardNm[%s]  AADevNo[%d] AAIoId[%s]  AADesc[%s]\n", cardNo, ifNo, devNo, AADevNm, AACardNm, AADevNo, AAIoId, AADesc); fflush(stdout);
#endif




  busy = false;


//==== Try the Input type
  if((strcmp("<none>", AAIoId) == 0) || (strcmp("Input", AAIoId) == 0)) {
    err = snd_pcm_open(&(pcmIf), pcmSpec, (snd_pcm_stream_t)(AlsaStream::EDIR_CAPTURE), 0);
#ifdef PRINT_VERBOSE
      fprintf(stdout, "UNSPECIFIED or Input\n"); fflush(stdout);
      fprintf(stdout, "  err from      snd_pcm_open for record  %d (human) [%s]\n", err, snd_strerror(err)); fflush(stdout);
#endif
    if(err >= 0) {
#ifdef PRINT_VERBOSE
      fprintf(stdout, "                snd_pcm_name                      [%s]\n", snd_pcm_name(pcmIf)); fflush(stdout);
      fprintf(stdout, "                snd_pcm_state_name                [%s]\n", snd_pcm_state_name(snd_pcm_state(pcmIf))); fflush(stdout);
#endif
      snd_pcm_hw_params_alloca(&(hwParams));
#ifdef PRINT_VERBOSE
      fprintf(stdout, "                snd_pcm_hw_params_alloca          0x%016lx\n", (unsigned long)hwParams); fflush(stdout);
#endif
      err = snd_pcm_hw_params_any(pcmIf, hwParams);
#ifdef PRINT_VERBOSE
      fprintf(stdout, "                snd_pcm_hw_params_any    %d (human) [%s]\n", err, snd_strerror(err)); fflush(stdout);
#endif
      canRX = true;
      FindFSs(pcmIf, hwParams);
      FindFormats(pcmIf, hwParams);
      snd_pcm_close(pcmIf);
//      snd_pcm_hw_params_free(hwParams);
    }
    else {
      int fErr = -err;
      if(fErr == CENO_BUSY)
        busy = true;
      else
        devErr = true;
      fprintf(stdout, "  err from   snd_pcm_open trying Input on pcmSpec %s    %d (human) [%s]\n", pcmSpec, err, snd_strerror(err)); fflush(stdout);
      canRX = false;
    }
  }






//==== Try the Output type
  if((strcmp("<none>", AAIoId) == 0) || (strcmp("Output", AAIoId) == 0)) {
    err = snd_pcm_open(&(pcmIf), pcmSpec, (snd_pcm_stream_t)(AlsaStream::EDIR_PLAYBACK), 0 );
#ifdef PRINT_VERBOSE
    fprintf(stdout, "UNSPECIFIED or Output\n"); fflush(stdout);
    fprintf(stdout, "  err from   snd_pcm_open %d (human) [%s]\n", err, snd_strerror(err)); fflush(stdout);
#endif
    if(err >= 0) {
#ifdef PRINT_VERBOSE
      fprintf(stdout, "  ifc name  snd_pcm_name                 %s\n", snd_pcm_name(pcmIf)); fflush(stdout);
      fprintf(stdout, "            snd_pcm_state_name for this interface: %s\n", snd_pcm_state_name(snd_pcm_state(pcmIf))); fflush(stdout);
#endif
      snd_pcm_hw_params_alloca(&(hwParams));
#ifdef PRINT_VERBOSE
      fprintf(stdout, "  handle of snd_pcm_hw_params_alloca     0x%16lx\n", (unsigned long)hwParams); fflush(stdout);
#endif
      err = snd_pcm_hw_params_any(pcmIf, hwParams);
#ifdef PRINT_VERBOSE
      fprintf(stdout, "  err from snd_pcm_hw_params_any         %08dd         which is       %s\n", err, snd_strerror(err)); fflush(stdout);
#endif
      canTX = true;
      FindFSs(pcmIf, hwParams);
      FindFormats(pcmIf, hwParams);
      snd_pcm_close(pcmIf);
//      snd_pcm_hw_params_free(hwParams);
    }
    else {
      int fErr = -err;
      if(fErr == CENO_BUSY)
        busy = true;
      else
        devErr = true;
      fprintf(stdout, "  err from   snd_pcm_open trying Output on pcmSpec %s    %d (human) [%s]\n", pcmSpec, err, snd_strerror(err)); fflush(stdout);
      canTX = false;
    }
  }
  return;
}
void  AlsaDevice::FindFSs(snd_pcm_t *i_pcmIf, snd_pcm_hw_params_t *i_hwParams) {
#ifdef PRINT_FUNCTION
  fprintf(stdout, "AlsaDevice::FindFSs(pcmIf(%llX), snd_pcm_hw_params_t(0x%llX))\n", (ullong)i_pcmIf, (ullong)i_hwParams); fflush(stdout);
#endif
  int err;
  numFSs = 0;
  for(ullong rateDex=0; rateDex < MAX_NAME_LEN; rateDex++)
    FSs[rateDex] = 0;
  for(ullong rateTest=0; rateTest<200000; rateTest++) {
    err = snd_pcm_hw_params_test_rate(i_pcmIf, i_hwParams, (int)rateTest, 0); // The rate must be int, never uint ... etc.
    if(err >= 0) {
#ifdef PRINT_VERBOSE
      fprintf(stdout, "        rate[%3d] = %6u\n", numFSs, rateTest); fflush(stdout);
#endif
      FSs[numFSs] = rateTest;
      numFSs++;
      if(numFSs > 10)
        rateTest = 200000;
    }
  }
  return;
}
void  AlsaDevice::FindFormats(snd_pcm_t *i_pcmIf, snd_pcm_hw_params_t *i_hwParams) {
#ifdef PRINT_FUNCTION
  fprintf(stdout, "AlsaDevice::FindFormats()\n"); fflush(stdout);
#endif
  int                result;
  snd_pcm_format_t   val;
  AlsaFormat *b;

  numFmts = 0;
  b = AlsaFormat::Instance();
  for(ullong fmtDex = 0; fmtDex < MAX_NAME_LEN; fmtDex++)
    fmts[fmtDex] = (snd_pcm_format_t)32767;

  for(ullong fmtDex=0; fmtDex<b->numLimited; fmtDex++) {
    val = b->capsLimited[fmtDex].fmt;
    result = snd_pcm_hw_params_test_format (i_pcmIf, i_hwParams, val);
    if(result >= 0) {
#ifdef PRINT_VERBOSE
      fprintf(stdout, "        fmt[%3d] = %6u  (human)%s\n", numFmts, fmtDex, b->capsLimited[fmtDex].fStr); fflush(stdout);
#endif
      fmts[numFmts++] = val;
      if(numFmts > 10)
        fmtDex = b->numLimited;
      }
    }
  return;
  }
ullong   AlsaDevice::GetFSs         (ullong  **o_rates) {
  *o_rates = FSs;
  return numFSs;
}

ullong   AlsaDevice::GetFmtList     (char   ***o_fmtStrs) {
  *o_fmtStrs  = fmtStrs;
  return numFmts;
}

void  AlsaDevice::CleanHint(void) {
  char *pStr;

  pStr = hintClean;
  while(*pStr != '\0') {
    if(*pStr == '\n')
      *pStr = '&';
    pStr++;
    }
  return;
  }


char *AlsaDevice::GetPast(char *i_test, char *i_pSrc) {
  ullong k;
  k = (ullong)strlen(i_test);
  for(ullong i=0; i<k; i++) {
    if(*i_pSrc == '\0')
      return NULL;
    if(*(i_test++) != *(i_pSrc++))
      return NULL;
  }
  return i_pSrc;
}
char *AlsaDevice::CopyUntil(char *i_test, char *i_pSrc, char *i_pDst) {
  bool  notYet;
  ullong   dex;
  ullong   k;
  ullong   i;
  char *p;

  notYet = true;
  p = i_pSrc;
  k = (ullong)(strlen(i_pSrc) - strlen(i_test));
  for(i=0; ((i<k) && notYet); i++) {
    ullong dd = strncmp(p, i_test, strlen(i_test));
    if(dd == 0) {
      dex = i;
      notYet = false;
    }
    p++;
  }
  if(notYet)
    dex = i+1;
  for(ullong i=0; i<dex; i++)
    *(i_pDst++) = *(i_pSrc++);
  *i_pDst = '\0';
  if(notYet)
    return NULL;
  else
    return i_pSrc;
}


// For your parser design pleasure a few examples of hints.
/*

NAMEfront:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
Front speakers

NAMErear:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
Rear speakers|IOIDOutput

NAMEsurround40:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
4.0 Surround output to Front and Rear speakers|IOIDOutput

NAMEsurround41:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
4.1 Surround output to Front, Rear and Subwoofer speakers|IOIDOutput

NAMEsurround50:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
5.0 Surround output to Front, Center and Rear speakers|IOIDOutput

NAMEsurround51:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
5.1 Surround output to Front, Center, Rear and Subwoofer speakers|IOIDOutput

NAMEsurround71:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI 2nd DAC
7.1 Surround output to Front, Center, Side, Rear and Woofer speakers|IOIDOutput

NAMEiec958:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
IEC958 (S/PDIF) Digital Audio Output

NAMEdmix:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
Direct sample mixing device

NAMEdmix:CARD=CMI8762,DEV=1|DESCC-Media CMI8762, C-Media PCI 2nd DAC
Direct sample mixing device|IOIDOutput

NAMEdmix:CARD=CMI8762,DEV=2|DESCC-Media CMI8762, C-Media PCI IEC958
Direct sample mixing device

NAMEdsnoop:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
Direct sample snooping device

NAMEdsnoop:CARD=CMI8762,DEV=1|DESCC-Media CMI8762, C-Media PCI 2nd DAC
Direct sample snooping device|IOIDOutput

NAMEdsnoop:CARD=CMI8762,DEV=2|DESCC-Media CMI8762, C-Media PCI IEC958
Direct sample snooping device

NAMEhw:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
Direct hardware device without any conversions

NAMEhw:CARD=CMI8762,DEV=1|DESCC-Media CMI8762, C-Media PCI 2nd DAC
Direct hardware device without any conversions|IOIDOutput

NAMEhw:CARD=CMI8762,DEV=2|DESCC-Media CMI8762, C-Media PCI IEC958
Direct hardware device without any conversions

NAMEplughw:CARD=CMI8762,DEV=0|DESCC-Media CMI8762, C-Media PCI DAC/ADC
Hardware device with all software conversions

NAMEplughw:CARD=CMI8762,DEV=1|DESCC-Media CMI8762, C-Media PCI 2nd DAC
Hardware device with all software conversions|IOIDOutput

NAMEplughw:CARD=CMI8762,DEV=2|DESCC-Media CMI8762, C-Media PCI IEC958
Hardware device with all software conversions

NAMEhw:CARD=PCH,DEV=0|DESCHDA Intel PCH, HDA Codec 0

NAMEfront:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
Front speakers

NAMEsurround40:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
4.0 Surround output to Front and Rear speakers

NAMEsurround41:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
4.1 Surround output to Front, Rear and Subwoofer speakers

NAMEsurround50:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
5.0 Surround output to Front, Center and Rear speakers

NAMEsurround51:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
5.1 Surround output to Front, Center, Rear and Subwoofer speakers

NAMEsurround71:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
7.1 Surround output to Front, Center, Side, Rear and Woofer speakers

NAMEiec958:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Digital
IEC958 (S/PDIF) Digital Audio Output|IOIDOutput

NAMEdmix:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
Direct sample mixing device

NAMEdmix:CARD=PCH,DEV=1|DESCHDA Intel PCH, ALC892 Digital
Direct sample mixing device|IOIDOutput

NAMEdsnoop:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
Direct sample snooping device

NAMEdsnoop:CARD=PCH,DEV=1|DESCHDA Intel PCH, ALC892 Digital
Direct sample snooping device|IOIDOutput

NAMEhw:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
Direct hardware device without any conversions

NAMEhw:CARD=PCH,DEV=1|DESCHDA Intel PCH, ALC892 Digital
Direct hardware device without any conversions|IOIDOutput

NAMEplughw:CARD=PCH,DEV=0|DESCHDA Intel PCH, ALC892 Analog
Hardware device with all software conversions

NAMEplughw:CARD=PCH,DEV=1|DESCHDA Intel PCH, ALC892 Digital
Hardware device with all software conversions|IOIDOutput

NAMEfront:CARD=STX,DEV=0|DESCXonar STX, Multichannel
Front speakers

NAMEsurround40:CARD=STX,DEV=0|DESCXonar STX, Multichannel
4.0 Surround output to Front and Rear speakers

NAMEsurround41:CARD=STX,DEV=0|DESCXonar STX, Multichannel
4.1 Surround output to Front, Rear and Subwoofer speakers

NAMEsurround50:CARD=STX,DEV=0|DESCXonar STX, Multichannel
5.0 Surround output to Front, Center and Rear speakers

NAMEsurround51:CARD=STX,DEV=0|DESCXonar STX, Multichannel
5.1 Surround output to Front, Center, Rear and Subwoofer speakers

NAMEsurround71:CARD=STX,DEV=0|DESCXonar STX, Multichannel
7.1 Surround output to Front, Center, Side, Rear and Woofer speakers

NAMEiec958:CARD=STX,DEV=0|DESCXonar STX, Multichannel
IEC958 (S/PDIF) Digital Audio Output

NAMEdmix:CARD=STX,DEV=0|DESCXonar STX, Multichannel
Direct sample mixing device

NAMEdmix:CARD=STX,DEV=1|DESCXonar STX, Digital
Direct sample mixing device|IOIDOutput

NAMEdsnoop:CARD=STX,DEV=0|DESCXonar STX, Multichannel
Direct sample snooping device

NAMEdsnoop:CARD=STX,DEV=1|DESCXonar STX, Digital
Direct sample snooping device|IOIDOutput

NAMEhw:CARD=STX,DEV=0|DESCXonar STX, Multichannel
Direct hardware device without any conversions

NAMEhw:CARD=STX,DEV=1|DESCXonar STX, Digital
Direct hardware device without any conversions|IOIDOutput

NAMEplughw:CARD=STX,DEV=0|DESCXonar STX, Multichannel
Hardware device with all software conversions

NAMEplughw:CARD=STX,DEV=1|DESCXonar STX, Digital
Hardware device with all software conversions|IOIDOutput

NAMEhw:CARD=HDMI,DEV=0|DESCHDA ATI HDMI, HDA Codec 0

NAMEhdmi:CARD=HDMI,DEV=0|DESCHDA ATI HDMI, HDMI 0
HDMI Audio Output|IOIDOutput

NAMEdmix:CARD=HDMI,DEV=3|DESCHDA ATI HDMI, HDMI 0
Direct sample mixing device|IOIDOutput

NAMEdsnoop:CARD=HDMI,DEV=3|DESCHDA ATI HDMI, HDMI 0
Direct sample snooping device|IOIDOutput

NAMEhw:CARD=HDMI,DEV=3|DESCHDA ATI HDMI, HDMI 0
Direct hardware device without any conversions|IOIDOutput

NAMEplughw:CARD=HDMI,DEV=3|DESCHDA ATI HDMI, HDMI 0
Hardware device with all software conversions|IOIDOutput
ALSA Version:  1.0.24.1

*/
