
// $Id: AlsaFormat.hpp 226 2022-12-11 22:05:17Z duncang $

//=================================================================================================
// Original File Name : AlsaFormat.h
// Original Author    : duncang
// Creation Date      : Apr 2, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//=================================================================================================

#ifndef ALSAFORMAT_H_
#define ALSAFORMAT_H_
#include <alsa/asoundlib.h>
class AlsaFormat {
  public:
  enum eCpuEndianness  {CE_LE, CE_BE, CE_UNK};
  enum eAlsaDataType   {ADT_FIRST=1, ADT_VAR,  ADT_DAT,  ADT_NL,   ADT_EXP,  ADT_SIGN, ADT_UNS,                     ADT_TC};  //   6
  enum eAlsaDataJust   {ADJ_FIRST=1, ADJ_HIGH, ADJ_LOW,                                                             ADJ_TC};  //   2
  enum eAlsaDataEndian {ADE_FIRST=1, ADE_LE,   ADE_BE,                                                              ADE_TC};  //   2
  enum eAlsaDataNBits  {ADI_FIRST=1, ADI_04,   ADI_08,   ADI_16,   ADI_18,   ADI_20,   ADI_24,   ADI_32,   ADI_64 , ADI_TC};  //   8
  enum eAlsaDataNBytes {ADY_FIRST=1, ADY_1 ,   ADY_2,    ADY_3,    ADY_4,    ADY_8,                                 ADY_TC};  //   5
                                                                                                                              // 960 combos
  struct AlsaFmtCap    {
    snd_pcm_format_t fmt;
    eAlsaDataType    typ;
    eAlsaDataNBits   bits;
    eAlsaDataNBytes  bytes;
    eAlsaDataJust    just;
    eAlsaDataEndian  endn;
    const char *     fStr;
  };
  private:
#include "AlsaFormatStrs.hpp"
  public:
  static AlsaFormat *Instance(void);
          bool       SetFmt(snd_pcm_format_t i_fmt);
          bool       SetFmt(AlsaFmtCap       i_caps);
          bool       SetFmt(eAlsaDataType    i_typ);
          bool       SetFmt(eAlsaDataNBits   i_bits);
          bool       SetFmt(eAlsaDataNBytes  i_bytes);
          bool       SetFmt(eAlsaDataJust    i_just);
          bool       SetFmt(eAlsaDataEndian  i_endn);
          uint       NumBits(void);
          uint       NumBits(eAlsaDataNBits);
          uint       NumBits(AlsaFmtCap f);
          uint       NumBytes(void);
          uint       NumBytes(eAlsaDataNBytes);
          uint       NumBytes(AlsaFmtCap f);
          bool       Equal(AlsaFmtCap f1, AlsaFmtCap f2);
          void       Copy(AlsaFmtCap *dst, AlsaFmtCap *src);
          bool       Promote(AlsaFmtCap f);
          bool       LegalPull(AlsaFmtCap *f);
  private:
                     AlsaFormat();
virtual             ~AlsaFormat();
          void       BuildTable();
          void       BuildMatrix();
          void       TestAlsa();
          void       BitsToBytes(AlsaFmtCap *f);
          void       BytesToBits(AlsaFmtCap *f);
  public:
            eCpuEndianness   cpuEndianness;
            AlsaFmtCap      *capsLimited;
            AlsaFmtCap      *capsFull;
            uint             numLimited;
            uint             numFull;
            AlsaFmtCap       caps;   // A Device will have a caps structure that this class informs
  private:
            uint            *bitsLookup;
            uint            *bytesLookup;
     static AlsaFormat      *pAlsaFormat;
  };

class AlsaOpenMode{
  public:
    enum eOpenMode {
      NONBLOCK        = SND_PCM_NONBLOCK,
      ASYNC           = SND_PCM_ASYNC,
      NO_AUTO_RESAMP  = SND_PCM_NO_AUTO_RESAMPLE,
      NO_AUTO_CHAN    = SND_PCM_NO_AUTO_CHANNELS,
      NO_AUTO_FMT     = SND_PCM_NO_AUTO_FORMAT,
      NO_SOFT_VOL     = SND_PCM_NO_SOFTVOL
    };
          AlsaOpenMode   ();
         ~AlsaOpenMode   ();
    void  SetBitField    (int  i_bitField    ) {bitField     = i_bitField    ; BitsToBools();}
    void  SetNonBlock    (bool i_nonBlock    ) {nonBlock     = i_nonBlock    ; BoolsToBits();}
    void  SetAsync       (bool i_async       ) {async        = i_async       ; BoolsToBits();}
    void  SetNoAutoResamp(bool i_noAutoResamp) {noAutoResamp = i_noAutoResamp; BoolsToBits();}
    void  SetNoAutoChan  (bool i_noAutoChan  ) {noAutoChan   = i_noAutoChan  ; BoolsToBits();}
    void  SetNoAutoFmt   (bool i_noAutoFmt   ) {noAutoFmt    = i_noAutoFmt   ; BoolsToBits();}
    void  SetNoSoftVol   (bool i_noSoftVol   ) {noSoftVol    = i_noSoftVol   ; BoolsToBits();}
    int   GetBitField    (void               ) {return bitField                             ;}
    bool  GetNonBlock    (void               ) {return nonBlock                             ;}
    bool  GetAsync       (void               ) {return async                                ;}
    bool  GetNoAutoResamp(void               ) {return noAutoResamp                         ;}
    bool  GetNoAutoChan  (void               ) {return noAutoChan                           ;}
    bool  GetNoAutoFmt   (void               ) {return noAutoFmt                            ;}
    bool  GetNoSoftVol   (void               ) {return noSoftVol                            ;}
  private:
    void  BitsToBools    (void);
    void  BoolsToBits    (void);
    int   bitField;
    bool  nonBlock;
    bool  async;
    bool  noAutoResamp;
    bool  noAutoChan;
    bool  noAutoFmt;
    bool  noSoftVol;
  };


#endif /* ALSASTREAM_H_ */
