
// $Id: AlsaCard.hpp 228 2022-12-25 04:52:32Z duncang $

//=================================================================================================
// Original File Name : AlsaCard.h
// Original Author    : duncang
// Creation Date      : 2010-12-18
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//     One layer down.
//
//=================================================================================================

#ifndef ALSACARD_H_
#define ALSACARD_H_
#include <sigc++/sigc++.h>
#include <alsa/asoundlib.h>
#include "AlsaInterface.hpp"
#include "AlsaIfTypes.hpp"

class AlsaCard {
  public:
                     AlsaCard         (ullong i_cardNo);
    virtual         ~AlsaCard         (void);
    void             Wipe             (void);

    bool             IsPresent        (void) {return present;};

    char            *GetCardNm        (void) {return cardNmLng;};
    char            *GetPcmSpec       (void) {return theIfc->GetPcmSpec();};

    AlsaInterface   *SetIfc           (ullong i_ifcNo);
    AlsaInterface   *GetIfc           (void) {return theIfc;};
    ullong           GetIfcNo         (void) {return ifcNo;};
    ullong           GetNumIfcs       (void) {return numIfcs;};

    ullong           SetDev           (ullong i_devNo);
    AlsaDevice      *GetDev           (ullong i_devNo);
    ullong           GetDevDex        (void) {return devNo;};
    ullong           GetNunDevs       (void) {return numDevs;};

    void             Rescan           (void);
    void             Print            (void);

  private:
    ullong                       cardNo;
    char                        *cardNmLng;
    char                        *cardNmHinted;

    AlsaInterface              **ifcs;   // We are the container for these
    AlsaInterface               *theIfc;
    ullong                       numIfcs;
    ullong                       ifcNo;

    AlsaDevice                  *theDev;
    ullong                       numDevs;
    ullong                       devNo;

    bool                         present;
  };

#endif /* ALSACARD_H_ */
