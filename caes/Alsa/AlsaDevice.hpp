
// $Id: AlsaDevice.hpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaDevice.h
// Original Author    : duncang
// Creation Date      : 2010-12-18
// Copyright          : Copyright © 2011 - 2022 by Catraeus and Duncan Gray
//
// Description:
//     Fourth level down ... Alsa -> Card -> Interface -> Device
/*
   IMPORTANT to note that devices are what humans think of.  There are a few indexings that we need
             to track:
             The index that ALSA has called DEV in the hint.
             The index due to the order that the device was found in getting the hint string array.
             The index due to the order way up in the app.  This needs more 'splainin'

Humans probably only want a limited number of NAME card-types returned.  This is from a long list
  in

*/
//
//=================================================================================================

#ifndef ALSADEVICE_H_
#define ALSADEVICE_H_
#include <caes/CaesMath.hpp>
#include <caes/CaesTypes.hpp>
#include <caes/Alsa/AlsaFormat.hpp>
#include <alsa/asoundlib.h>

class AlsaDevice {
  public:
             AlsaDevice     (ullong  i_cardNo, ullong i_ifNo, ullong i_devNo, char *i_hint);
             AlsaDevice     (void);
    virtual ~AlsaDevice     (void);

    ullong   GetDevNo       (void) {return devNo      ;};

    char    *GetHintALSA    (void) {return hintClean  ;};
    char    *GetAACardNm    (void) {return AACardNm   ;};
    ullong   GetAADevNo     (void) {return AADevNo    ;};
    bool     HasAADevNo     (void) {return hasAADevNo ;};
    char    *GetAADevNm     (void) {return AADevNm    ;};
    char    *GetAADesc      (void) {return AADesc     ;};
    char    *GetAAIoId      (void) {return AAIoId     ;};

    bool     CanTX          (void) {return canTX      ;};
    bool     CanRX          (void) {return canRX      ;};
    bool     IsBusy         (void) {return busy       ;};
    bool     HasErr         (void) {return devErr     ;};

    char    *GetPcmSpec     (void) {return pcmSpec    ;};

    ullong   GetFSs         (ullong  **o_rates  );

    ullong   GetFmtList     (char   ***o_fmtStrs);
    void     SetFmt         (ullong    i_fmtCurr);

    void     Print          (void);

  private:
    void     Rescan         (char *i_hint);
    void     Wipe           (void);
    void     FindCaps       (void);
    void     FindFormats    (snd_pcm_t *i_pcmIf, snd_pcm_hw_params_t *i_hwParams);
    void     FindFSs        (snd_pcm_t *i_pcmIf, snd_pcm_hw_params_t *i_hwParams);
    void     CleanHint      (void);
    char    *GetPast        (char *i_test, char *i_pSrc);
    char    *CopyUntil      (char *i_test, char *i_pSrc, char *i_pDst);

  private:
    ullong                      cardNo;          // The inserted module number as brought up by the kernel
    ullong                      ifNo;            // same as the enum in alsa/control.h
    ullong                      devNo;          // The Device index that we have made during creation

    char                       *hintClean;       // The hint with return, tab ... etc. ... removed
    char                       *hintALSA;        // The hint straight from ALSA

    char                       *AADevNm;         // Called NAME in the hint
    char                       *AACardNm;        // Called CARD in the hint, might not be there especially if not a PCM
    ullong                      AADevNo;         // Called DEV in the hint

    char                       *AADesc;          // Called DESC in the hint
    char                       *AAIoId;          // Called IOID in hint, might not be there

    ullong                     *FSs;
    ullong                      numFSs;
    ullong                      FSCurr;

    AlsaFormat::AlsaFmtCap      caps;
    snd_pcm_format_t           *fmts;
    char                      **fmtStrs;
    ullong                      numFmts;
    ullong                      fmtCurr;

    char                       *pcmSpec;

    bool                        hasAADevNo;
    bool                        present;
    bool                        busy;
    bool                        devErr;
    bool                        canTX;
    bool                        canRX;
  };

#endif /* ALSADEVICE_H_ */

