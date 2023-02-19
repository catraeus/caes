
// $Id: AlsaFormat.cpp 226 2022-12-11 22:05:17Z duncang $

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

#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <alsa/pcm.h>

#include "AlsaFormat.hpp"
#include "AlsaStream.hpp"

AlsaFormat* AlsaFormat::pAlsaFormat = 0;



     AlsaFormat::AlsaFormat() {
  capsLimited   = NULL;
  capsFull      = NULL;
  cpuEndianness = AlsaFormat::CE_UNK;
  bitsLookup    = NULL;
  bytesLookup   = NULL;

  numLimited   =    0;
  numFull      =    0;
  if(capsLimited == NULL) {
    BuildTable();
    BuildMatrix();
    }
  SetFmt(SND_PCM_FORMAT_S32_LE);
  }
     AlsaFormat::~AlsaFormat(){
  delete capsLimited;
  }
AlsaFormat* AlsaFormat::Instance(void) {
  if(pAlsaFormat == 0)
    pAlsaFormat = new AlsaFormat;
  return pAlsaFormat;
  }
void AlsaFormat::BuildTable(void) {
  if     (__BYTE_ORDER == __LITTLE_ENDIAN)  cpuEndianness = CE_LE;
  else if(__BYTE_ORDER == __BIG_ENDIAN   )  cpuEndianness = CE_BE;
  else                                      cpuEndianness = CE_UNK;
#include "AlsaFormatCaps.hpp"
  return;
  }
void AlsaFormat::BuildMatrix(void) {
  capsFull = (AlsaFmtCap *)malloc(1024 * sizeof(AlsaFmtCap));
  for(uint t=ADT_FIRST + 1; t<ADT_TC; t++) {
    for(uint j=ADJ_FIRST + 1; j<ADJ_TC; j++) {
      for(uint e=ADE_FIRST + 1; e<ADE_TC; e++) {
        for(uint i=ADI_FIRST + 1; i<ADI_TC; i++) {
          for(uint y=ADY_FIRST + 1; y<ADY_TC; y++) {
              capsFull[numFull].typ   = (eAlsaDataType)t;
              capsFull[numFull].just  = (eAlsaDataJust)j;
              capsFull[numFull].endn  = (eAlsaDataEndian)e;
              capsFull[numFull].bits  = (eAlsaDataNBits)i;
              capsFull[numFull].bytes = (eAlsaDataNBytes)y;
              LegalPull(&(capsFull[numFull]));
              numFull++;
            }
          }
        }
      }
    }
  bitsLookup  = (uint *)malloc(128 * sizeof(uint));
  bytesLookup = (uint *)malloc(128 * sizeof(uint));
  bitsLookup[ADI_FIRST]  =  0;
  bitsLookup[ADI_04]     =  4;
  bitsLookup[ADI_08]     =  8;
  bitsLookup[ADI_16]     = 16;
  bitsLookup[ADI_18]     = 18;
  bitsLookup[ADI_20]     = 20;
  bitsLookup[ADI_24]     = 24;
  bitsLookup[ADI_32]     = 32;
  bitsLookup[ADI_64]     = 64;
  bitsLookup[ADI_TC]     =  0;
  bytesLookup[ADY_FIRST] =  0;
  bytesLookup[ADY_1]     =  1;
  bytesLookup[ADY_2]     =  2;
  bytesLookup[ADY_3]     =  3;
  bytesLookup[ADY_4]     =  4;
  bytesLookup[ADY_8]     =  8;
  bytesLookup[ADY_TC]    =  0;
  return;
  }
bool AlsaFormat::SetFmt(snd_pcm_format_t i_fmt) { // Pushes the proposed ALSA fmt down into the rest
  bool r;
  r = false;
  for(uint i=0; i<numLimited; i++) {
    if(capsLimited[i].fmt == i_fmt) {
      caps.fmt   = capsLimited[i].fmt;
      caps.typ   = capsLimited[i].typ;
      caps.bits  = capsLimited[i].bits;
      caps.bytes = capsLimited[i].bytes;
      caps.just  = capsLimited[i].just;
      caps.endn  = capsLimited[i].endn;
      i     = numLimited + 1;
      r     = true;
      }
    }
  return r;
  }
bool AlsaFormat::SetFmt(AlsaFmtCap       i_caps) { // Pulls the proposed ALSA fmt up to the snd_pcm_fmt_t
  bool r;

  r = LegalPull(&i_caps);
  if(r)
      Copy(&caps, &i_caps);
  return r;
  }
bool AlsaFormat::SetFmt(eAlsaDataType    i_typ) {
  bool       r;
  AlsaFmtCap tc;

  Copy(&tc, &caps);
  tc.typ = i_typ;
  r = LegalPull(&tc);
  if(r) Copy(&caps, &tc);
  return r;
  }
bool AlsaFormat::SetFmt(eAlsaDataNBits   i_bits) {
  bool r;
  AlsaFmtCap tc;
  Copy(&tc, &caps);
  tc.bits = i_bits;
  BitsToBytes(&tc);
  r = LegalPull(&tc);
  if(r)
    Copy(&caps, &tc);
  return r;
  }
bool AlsaFormat::SetFmt(eAlsaDataNBytes  i_bytes) {
  bool r;
  AlsaFmtCap tc;
  Copy(&tc, &caps);
  tc.bytes = i_bytes;
  BytesToBits(&tc);
  r = LegalPull(&tc);
  if(r) Copy(&caps, &tc);
  return r;
  }
bool AlsaFormat::SetFmt(eAlsaDataJust    i_just) {
  bool r;

  r = false;

  return r;
  }
bool AlsaFormat::SetFmt(eAlsaDataEndian  i_endn) {
  bool r;
  AlsaFmtCap tc;
  Copy(&tc, &caps);
  tc.endn = i_endn;
  r = LegalPull(&tc);
  if(r) Copy(&caps, &tc);
  return r;
  }
bool AlsaFormat::Equal(AlsaFmtCap c1, AlsaFmtCap c2) {
  bool r;
  r = false;
  if(
     (c1.typ   == c2.typ  ) &&
     (c1.bits  == c2.bits ) &&
     (c1.bytes == c2.bytes) &&
     (c1.just  == c2.just ) &&
     (c1.endn  == c2.endn )
    )
    r = true;
  return r;
  }
bool AlsaFormat::LegalPull(AlsaFmtCap *f) { // Populates the full matrix with the ALSA-legal limited caps type.
  bool r;
  r = false;
  f->fmt = SND_PCM_FORMAT_UNKNOWN;
  for(uint i=0; i<numLimited; i++) {
    if(
        (f->typ   == capsLimited[i].typ  ) &&
        (f->bits  == capsLimited[i].bits ) &&
        (f->bytes == capsLimited[i].bytes) &&
        (f->just  == capsLimited[i].just ) &&
        (f->endn  == capsLimited[i].endn )
      ) {
      f->fmt = capsLimited[i].fmt;
      i     = numLimited + 1;
      r     = true;
      }
    }
  return r;
  }
void AlsaFormat::Copy(AlsaFmtCap *dst, AlsaFmtCap *src) {
  memcpy((void *)(dst), (void *)(src), sizeof(AlsaFmtCap));
  return;
  }
void AlsaFormat::TestAlsa(void) {
  for(uint i=0; i<numLimited; i++) {
    fprintf(stdout, "%20s  ", snd_pcm_format_name(capsLimited[i].fmt));
    if((snd_pcm_format_big_endian(capsLimited[i].fmt) == 1) && (capsLimited[i].endn == ADE_BE))
      fprintf(stdout, "%2u    bendn   ", i);
    else
      fprintf(stdout, "%2u no-bendn   ", i);
    if((snd_pcm_format_little_endian(capsLimited[i].fmt) == 1) && (capsLimited[i].endn == ADE_LE))
      fprintf(stdout, "%2u    lendn   ", i);
    else
      fprintf(stdout, "%2u no-lendn   ", i);
    fprintf(stdout, "bits_store %2u * %3d   ", NumBytes(capsLimited[i].bytes) * 8, snd_pcm_format_physical_width(capsLimited[i].fmt));
    fprintf(stdout, "bits_sig %2u * %3d   ", NumBits(capsLimited[i].bits), snd_pcm_format_width(capsLimited[i].fmt));
    fprintf(stdout, "\n");
    }
  return;
  }
uint AlsaFormat::NumBits(void){return bitsLookup[caps.bits];}
uint AlsaFormat::NumBits(eAlsaDataNBits b){return bitsLookup[b];}
uint AlsaFormat::NumBits(AlsaFmtCap f){return bitsLookup[f.bits];}
uint AlsaFormat::NumBytes(void){return bytesLookup[caps.bytes];}
uint AlsaFormat::NumBytes(eAlsaDataNBytes b){return bytesLookup[b];}
uint AlsaFormat::NumBytes(AlsaFmtCap f){return bytesLookup[f.bytes];}
void AlsaFormat::BitsToBytes(AlsaFmtCap *f) {
  switch(f->bits) {
    case ADI_04:
      f->bytes = ADY_1;
      break;
    case ADI_08:
      f->bytes = ADY_1;
      break;
    case ADI_16:
      f->bytes = ADY_2;
      break;
    case ADI_18:
      f->bytes = ADY_4;
      break;
    case ADI_20:
      f->bytes = ADY_4;
      break;
    case ADI_24:
      f->bytes = ADY_4;
      break;
    case ADI_32:
      f->bytes = ADY_4;
      break;
    case ADI_64:
      f->bytes = ADY_8;
      break;
    default:
      f->bytes = ADY_FIRST;
      break;
    }
  return;
  }
void AlsaFormat::BytesToBits(AlsaFmtCap *f) {
  switch(f->bytes) {
    case ADY_1:
      f->bits = ADI_08;
      break;
    case ADY_2:
      f->bits = ADI_16;
      break;
    case ADY_3:
      f->bits = ADI_24;
      break;
    case ADY_4:
      f->bits = ADI_32;
      break;
    case ADY_8:
      f->bits = ADI_64;
      break;
    default:
      f->bits = ADI_FIRST;
      break;
    }
  return;
  }
     AlsaOpenMode::AlsaOpenMode(void) {
  nonBlock     = false;
  async        = false;
  noAutoResamp = true;
  noAutoChan   = true;
  noAutoFmt    = true;
  noSoftVol    = true;
  BoolsToBits();
  return;
  }
     AlsaOpenMode::~AlsaOpenMode(void) {
  return;
  }
void AlsaOpenMode::BitsToBools(void) {
  nonBlock     = ((bitField & NONBLOCK      ) != 0);
  async        = ((bitField & ASYNC         ) != 0);
  noAutoResamp = ((bitField & NO_AUTO_RESAMP) != 0);
  noAutoChan   = ((bitField & NO_AUTO_CHAN  ) != 0);
  noAutoFmt    = ((bitField & NO_AUTO_FMT   ) != 0);
  noSoftVol    = ((bitField & NO_SOFT_VOL   ) != 0);
  return;
  }
void AlsaOpenMode::BoolsToBits(void) {
  bitField = 0;
  if(nonBlock)     bitField += NONBLOCK;
  if(async)        bitField += ASYNC;
  if(noAutoResamp) bitField += NO_AUTO_RESAMP;
  if(noAutoChan)   bitField += NO_AUTO_CHAN;
  if(noAutoFmt)    bitField += NO_AUTO_FMT;
  if(noSoftVol)    bitField += NO_SOFT_VOL;
  return;
  }
