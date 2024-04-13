
// $Id: AlsaFormatCaps.hpp 226 2022-12-11 22:05:17Z duncang $

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
#include "AlsaFormat.hpp"
  capsLimited = new AlsaFormat::AlsaFmtCap[64];
  uint i=0; // WARNING this array of structs must stay in lock step with the assigned int of the enum _snd_pcm_format
//                    _snd_pcm_format                                                                                        in alsa/pcm.h
//                    |                                     eAlsaDataType                                                    in AlsaFormat.hpp
//                    |                                     |           eAlsaDataNBits                                       in AlsaFormat.hpp
//                    |                                     |           |        eAlsaDataNBytes                             in AlsaFormat.hpp
//                    |                                     |           |        |       eAlsaDataJust                       in AlsaFormat.hpp
//                    |                                     |           |        |       |          eAlsaDataEndian          in AlsaFormat.hpp
//                    |                                     |           |        |       |          |       const strings    in AlsaFormatStrs.hpp
//                    |                                     |           |        |       |          |       |
  capsLimited[i++] = {SND_PCM_FORMAT_S8                   , ADT_SIGN  , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_S8                   };
  capsLimited[i++] = {SND_PCM_FORMAT_U8                   , ADT_UNS   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_U8                   };
  capsLimited[i++] = {SND_PCM_FORMAT_S16_LE               , ADT_SIGN  , ADI_16 , ADY_2 , ADJ_LOW  , ADE_LE ,STRFMT_S16_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S16_BE               , ADT_SIGN  , ADI_16 , ADY_2 , ADJ_LOW  , ADE_BE ,STRFMT_S16_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U16_LE               , ADT_UNS   , ADI_16 , ADY_2 , ADJ_LOW  , ADE_LE ,STRFMT_U16_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U16_BE               , ADT_UNS   , ADI_16 , ADY_2 , ADJ_LOW  , ADE_BE ,STRFMT_U16_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S24_LE               , ADT_SIGN  , ADI_24 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_S24_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S24_BE               , ADT_SIGN  , ADI_24 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_S24_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U24_LE               , ADT_UNS   , ADI_24 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_U24_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U24_BE               , ADT_UNS   , ADI_24 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_U24_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S32_LE               , ADT_SIGN  , ADI_32 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_S32_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S32_BE               , ADT_SIGN  , ADI_32 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_S32_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U32_LE               , ADT_UNS   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_U32_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U32_BE               , ADT_UNS   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_U32_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_FLOAT_LE             , ADT_EXP   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_FLOAT_LE             };
  capsLimited[i++] = {SND_PCM_FORMAT_FLOAT_BE             , ADT_EXP   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_FLOAT_BE             };
  capsLimited[i++] = {SND_PCM_FORMAT_FLOAT64_LE           , ADT_EXP   , ADI_64 , ADY_8 , ADJ_LOW  , ADE_LE ,STRFMT_FLOAT64_LE           };
  capsLimited[i++] = {SND_PCM_FORMAT_FLOAT64_BE           , ADT_EXP   , ADI_64 , ADY_8 , ADJ_LOW  , ADE_BE ,STRFMT_FLOAT64_BE           };
  capsLimited[i++] = {SND_PCM_FORMAT_IEC958_SUBFRAME_LE   , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_IEC958_SUBFRAME_LE   };
  capsLimited[i++] = {SND_PCM_FORMAT_IEC958_SUBFRAME_BE   , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_BE ,STRFMT_IEC958_SUBFRAME_BE   };
  capsLimited[i++] = {SND_PCM_FORMAT_MU_LAW               , ADT_NL    , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_MU_LAW               };
  capsLimited[i++] = {SND_PCM_FORMAT_A_LAW                , ADT_NL    , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_A_LAW                };
  capsLimited[i++] = {SND_PCM_FORMAT_IMA_ADPCM            , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_IMA_ADPCM            };
  capsLimited[i++] = {SND_PCM_FORMAT_MPEG                 , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_MPEG                 };
  capsLimited[i++] = {SND_PCM_FORMAT_GSM                  , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_GSM                  };
  capsLimited[i++] = {SND_PCM_FORMAT_S20_LE               , ADT_SIGN  , ADI_20 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_S20_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_S20_BE               , ADT_SIGN  , ADI_20 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_S20_BE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U20_LE               , ADT_UNS   , ADI_20 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_U20_LE               };
  capsLimited[i++] = {SND_PCM_FORMAT_U20_BE               , ADT_UNS   , ADI_20 , ADY_4 , ADJ_LOW  , ADE_BE ,STRFMT_U20_BE               };
  capsLimited[i++] = {(snd_pcm_format_t)29                , ADT_UNS   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_MISSING_29           };
  capsLimited[i++] = {(snd_pcm_format_t)30                , ADT_UNS   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_MISSING_30           };
  capsLimited[i++] = {SND_PCM_FORMAT_SPECIAL              , ADT_UNS   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_SPECIAL              };
  capsLimited[i++] = {SND_PCM_FORMAT_S24_3LE              , ADT_SIGN  , ADI_24 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_S24_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_S24_3BE              , ADT_SIGN  , ADI_24 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_S24_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U24_3LE              , ADT_UNS   , ADI_24 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_U24_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U24_3BE              , ADT_UNS   , ADI_24 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_U24_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_S20_3LE              , ADT_SIGN  , ADI_20 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_S20_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_S20_3BE              , ADT_SIGN  , ADI_20 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_S20_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U20_3LE              , ADT_UNS   , ADI_20 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_U20_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U20_3BE              , ADT_UNS   , ADI_20 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_U20_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_S18_3LE              , ADT_SIGN  , ADI_18 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_S18_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_S18_3BE              , ADT_SIGN  , ADI_18 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_S18_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U18_3LE              , ADT_UNS   , ADI_18 , ADY_3 , ADJ_LOW  , ADE_LE ,STRFMT_U18_3LE              };
  capsLimited[i++] = {SND_PCM_FORMAT_U18_3BE              , ADT_UNS   , ADI_18 , ADY_3 , ADJ_LOW  , ADE_BE ,STRFMT_U18_3BE              };
  capsLimited[i++] = {SND_PCM_FORMAT_G723_24              , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_G723_24              };
  capsLimited[i++] = {SND_PCM_FORMAT_G723_24_1B           , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_G723_24_1B           };
  capsLimited[i++] = {SND_PCM_FORMAT_G723_40              , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_G723_40              };
  capsLimited[i++] = {SND_PCM_FORMAT_G723_40_1B           , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_G723_40_1B           };
  capsLimited[i++] = {SND_PCM_FORMAT_DSD_U8               , ADT_VAR   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_DSD_U8               };
  capsLimited[i++] = {SND_PCM_FORMAT_DSD_U16_LE           , ADT_VAR   , ADI_16 , ADY_2 , ADJ_LOW  , ADE_LE ,STRFMT_DSD_U16_LE           };
  capsLimited[i++] = {SND_PCM_FORMAT_DSD_U32_LE           , ADT_VAR   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_DSD_U32_LE           };
  capsLimited[i++] = {SND_PCM_FORMAT_DSD_U16_BE           , ADT_VAR   , ADI_16 , ADY_2 , ADJ_LOW  , ADE_LE ,STRFMT_DSD_U16_BE           };
  capsLimited[i++] = {SND_PCM_FORMAT_DSD_U32_BE           , ADT_VAR   , ADI_32 , ADY_4 , ADJ_LOW  , ADE_LE ,STRFMT_DSD_U32_BE           };
//capsLimited[i++] = {SND_PCM_FORMAT_UNKNOWN              , ADT_UNS   , ADI_08 , ADY_1 , ADJ_LOW  , ADE_LE ,STRFMT_UNKNOWN              };
  numLimited = i;

