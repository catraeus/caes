
// $Id: AlsaCard.cpp 228 2022-12-25 04:52:32Z duncang $

//=================================================================================================
// Original File Name : AlsaCard.cpp
// Original Author    : duncang
// Creation Date      : 2010-12-18
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//     One layer down.
//
//=================================================================================================

#include <stdio.h>
#include <caes/Alsa/AlsaCard.hpp>
//#include "../_PrjGlbl.hpp"
#include <caes/CaesTypes.hpp>

      AlsaCard::AlsaCard(ullong i_cardNo) : cardNo(i_cardNo) {

  cardNmHinted       = new char[MAX_NAME_LEN];
  cardNmLng          = new char[MAX_NAME_LEN];
  ifcs               = new AlsaInterface *[AlsaIfTypes::numTypes];
  for(ullong ifcDex = 0; ifcDex < AlsaIfTypes::numTypes; ifcDex++)
    ifcs[ifcDex] = NULL;

  Rescan();

  return;
}
      AlsaCard::~AlsaCard() {
  Wipe();
  delete(cardNmLng);
  delete(cardNmHinted);
  delete(ifcs);
  return;
  }
void  AlsaCard::Wipe(void) { // Bop down the hierarchy to delete everything.
  for(ullong ifcDex=0; ifcDex<AlsaIfTypes::numTypes; ifcDex++) {
    if(ifcs[ifcDex] != NULL)
      delete ifcs[ifcDex];
    ifcs[ifcDex] = NULL;
  }

  // IMPORTANT cardNo must not be zeroed.
  numIfcs = 0;
  ifcNo   = 0;
  theIfc  = ifcs[ifcNo];

  devNo   = 0;
  numDevs = 0;
  theDev  = NULL;

  cardNmLng[0]    = '\0';
  cardNmHinted[0] = '\0';

  present = false;

  return;
}






void  AlsaCard::Rescan(void) {
  AlsaInterface *tIfc;
  int            iErr;
  char          *tStr;
  char          *pStr;

  tStr           = new char[MAX_NAME_LEN];

  Wipe();
  iErr           = snd_card_get_name(cardNo, &pStr);
  present        = (iErr == 0);
  if(!present) {
    delete tStr;
    return;
  }

  strcpy(cardNmLng, pStr);

  numIfcs       = AlsaIfTypes::numTypes; // MAGICK kind of, this is fixed in code, no factory in ALSA and appears magically there.
  for(ullong ifcDex=0; ifcDex<numIfcs; ifcDex++) {
    tIfc = new AlsaInterface(cardNo, ifcDex);
    ifcs[ifcDex] = tIfc;
  }
  iErr = snd_card_get_name(cardNo, &cardNmHinted);
  (void)iErr;
  SetIfc(AlsaIfTypes::EAIT_PCM); // Start life with the PCM interfaces.

#ifdef PRINT_VERBOSE
  fprintf(stdout, "AlsaCard[%02d]: short: %12s     long:  %24s\n", cardNo, cardNmHinted, cardNmLng);
#endif
  return;
  delete tStr;
}







AlsaInterface  *AlsaCard::SetIfc(ullong i_ifcNo) {
  if(numIfcs == 0)
    i_ifcNo = 0;
  else if(i_ifcNo > numIfcs - 1)
    i_ifcNo = numIfcs - 1;
  ifcNo   = i_ifcNo;
  theIfc  = ifcs[ifcNo];

  if(numIfcs > 0) // IMPORTANT Always guard against no interfaces when working with devices.
    SetDev(theIfc->GetDevNo());
  return theIfc;
}

ullong        AlsaCard::SetDev(ullong i_devNo) {
  if(numIfcs == 0)
    devNo = 0;
  theIfc->SetDev(i_devNo);
  devNo   = theIfc->GetDevNo(); // IMPORTANT Always get back the validated devNo. AND This is our index, not the ALSA hint DEV number
  theDev  = theIfc->GetDev();
  numDevs = theIfc->GetNumDevs();
  return devNo;
}



void AlsaCard::Print(void) {
  fprintf(stdout, "Card-------[%02lld] cardNmLng: .%s.\n", cardNo, cardNmLng); fflush(stdout);
  for(ullong ifcDex=0; ifcDex<numIfcs; ifcDex++) {
    ifcs[ifcDex]->Print();
  }
  return;
}
