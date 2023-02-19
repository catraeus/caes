
// $Id: Alsa.hpp 1 2014-10-15 04:10:40Z duncang $

//=================================================================================================
// Original File Name : Alsa.h
// Original Author    : duncang
// Creation Date      : 2010-12-17
// Copyright          : Copyright © 2011 - 2022 by Catraeus and Duncan Gray
//
// Description        :
//     The root of a Fixed Clade format, Taxonomical Hierarchical Tree.  Say that 3 times fast!
//
//=================================================================================================

#ifndef __CAES_ALSA_H_
#define __CAES_ALSA_H_
#include <sigc++/sigc++.h>
#include <alsa/asoundlib.h>
#include "AlsaCard.hpp"

//  ALSA has, looking down at its feet: cards  <<= devices <<= elements <<= interfaces.

class Alsa : public sigc::trackable  {
  public:
                    Alsa              (void           );
    virtual        ~Alsa              (void           );

    void            Rescan            (void           );

    void            SetCard           (ullong i_cardNo);
    ullong          GetNoCards        (void           ) {return numCards;};
    char           *GetCardNm         (void           ) {return theCard->GetCardNm();};
    AlsaCard       *GetCard           (void           ) {return theCard;};
    char           *GetAACardNm       (void           ) {return theDev->GetAACardNm();}; // The ALSA Hint name of the card.

    void            SetIfc            (ullong i_ifcNo );
    ullong          GetIfcNo          (void           ) {return theCard->GetIfcNo();};
    char           *GetIfcName        (void           ) {return theIfc->GetName();};
    ullong          GetNumIfcs        (void           ) {return theCard->GetNumIfcs();};
    AlsaInterface  *GetIfc            (void           ) {return theIfc;};
    bool            IsIfcPresent      (void           ) {return theIfc->IsPresent();};

    void            SetDev            (ullong i_devDex);
    ullong          GetNumDevs        (void           ) {return theIfc->GetNumDevs();};
    ullong          GetDevNo          (void           ) {return theDev->GetDevNo();};
    AlsaDevice     *GetDev            (void           ) {return theDev;};
    char           *GetAADevNm        (void           ) {return theDev->GetAADevNm();}; // The ALSA Hint name
    ullong          GetAADevNo        (void           ) {return theDev->GetAADevNo();};
    bool            HasAADevNo        (void           ) {return theDev->HasAADevNo();};
    char           *GetAAIoId         (void           ) {return theDev->GetAAIoId();};  // From the hint, this is the only clue of input v output capability
    bool            DevCanTX          (void           ) {return theDev->CanTX();};
    bool            DevCanRX          (void           ) {return theDev->CanRX();};
    bool            DevIsBusy         (void           ) {return theDev->IsBusy();};
    bool            DevHasErr         (void           ) {return theDev->HasErr();};

    ullong          GetFSs            (ullong **o_FSs ) {return theDev->GetFSs(o_FSs);};
    char           *GetPcmSpec        (void           ) {return theCard->GetPcmSpec();};
//    void     SetFS             (ullong i_FSDex ) {theDev->SetFS(i_FSDex); return;};

    void            Print             (void           );
    void            PrintPcmSpecList  (void           );


    char           *GetVersion        (void           ) {return version;};
  private:
    void            Wipe              (void           );

  private:
    char           *version;     // The version of ALSA that has been found.

    AlsaCard      **cards;       // An array of pointer to card classes. We are the container, so the list.
    AlsaCard       *theCard;     // The card that is currently doing stuff
    ullong          numCards;    // So you can point to 'em without trashing the end of the array.
    ullong          cardNo;      // The card that is currently doing stuff

    AlsaInterface  *theIfc;      // The card is the container, so we only have this for convenience to prevent -> cascade hell
    ullong          numIfcs;
    ullong          ifcNo;

    AlsaDevice     *theDev;      // Likewise
    ullong          numDevs;
    ullong          devNo;

  private:
    bool            present;     // There is an ALSA system driver loaded.
  };

#endif /* __CAES_ALSA_H_ */
//_________________________________________________________________________________________________
