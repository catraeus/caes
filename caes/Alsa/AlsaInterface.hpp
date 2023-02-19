
// $Id: AlsaInterface.hpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaInterface.h
// Original Author    : duncang
// Creation Date      : 2010-12-18
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//     Third level down  ALSA -> Card -> Interface
//     Contains, generally, a large number of devices.  BUT, only a few of those devices are
//     "useful" in a given context.  SO, (for a given interface type) ... only one is going to be
//     marked as useful.  That mark is in the device as a "desired" bool. and desiredness can
//     be renegotiated on the fly.
//
//=================================================================================================

#ifndef __ALSAINTERFACE_H_
#define __ALSAINTERFACE_H_
#include <alsa/asoundlib.h>
#include <string>
#include "AlsaDevice.hpp"

class AlsaInterface {
  public:
                        AlsaInterface (ullong i_cardNo, ullong i_ifcNo);
    virtual            ~AlsaInterface (void         );

            char       *GetPcmSpec   (void) {return theDev->GetPcmSpec();};
            char       *GetName      (void) {return ifcName;};

            ullong      SetDev       (ullong i_devNo);
            ullong      GetDevNo     (void) {return devNo;};
            AlsaDevice *GetDev       (void) {return theDev;};
            AlsaDevice *GetDev       (ullong i_devNo);
            ullong      GetNumDevs   (void) {return numDevs;};
            bool        IsPresent    (void) {return present;};


            void        Rescan        (void         );

            void        Print         (void         );
  private:
            void        Wipe          (void         );
            void        ParseHint     (char **i_hint);
  private:
    ullong         cardNo; // IMPORTANT ALSA is an always-global thing, so we always have to ask with card number included.

    ullong         ifcNo;  // MAGICK there are enums in AlsaIfTypes that must always correlate to whatever ifcNo we are.
    char          *ifcName;

    AlsaDevice   **devs;    // We are the container.
    AlsaDevice    *theDev;
    ullong         numDevs;
    ullong         devNo;

    bool           present;

};

#endif /* __ALSAINTERFACE_H_ */
