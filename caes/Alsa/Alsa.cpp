
// $Id: Alsa.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : Alsa.cpp
// Original Author    : duncang
// Creation Date      : 2010-12-17
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//     The root of a Fixed Clade format, Taxonomical Hierarchical Tree.  Say that 3 times fast!
//
//=================================================================================================


#include <malloc.h>

#include <caes/CaesString.hpp>

#include "Alsa.hpp"
#include "AlsaDevice.hpp"
#include "AlsaIfTypes.hpp"
#include <caes/CaesTypes.hpp>

      Alsa::Alsa() {
  version  = new char(strlen(snd_asoundlib_version()) + 1);
  strcpy(version, snd_asoundlib_version());

  cards    = new AlsaCard *[MAX_NAME_LEN];
  for(ullong cardDex=0; cardDex < MAX_NAME_LEN; cardDex++)
    cards[cardDex] = NULL;
  numCards = 0;
  Rescan();
  return;
}
      Alsa::~Alsa() {
  Wipe();
  delete(version);
  delete(cards);
  fprintf(stdout, "Alsa::~Alsa()\n");
  }
void  Alsa::Wipe(void) {
  for(ullong cardDex=0; cardDex<MAX_NAME_LEN; cardDex++) {
    if(cards[cardDex] != NULL)
      delete cards[cardDex];
    cards[cardDex] = NULL;
    }
  numCards  = 0;
  cardNo    = 0;
  theCard   = NULL;

  theIfc    = NULL;
  numIfcs   = 0;
  ifcNo     = 0;

  theDev    = NULL;
  numDevs   = 0;
  devNo     = 0;

  present   = false;
  return;
}
void  Alsa::Rescan(void) {
  bool      goOn;
  AlsaCard *tCard;

  Wipe();
  goOn     = true;
  numCards = 0;
  while(goOn) {
    tCard = new AlsaCard(numCards);
    if(!tCard->IsPresent()) { // MAGICK The ALSA interface doesn't have gaps in card number presence.
      delete(tCard);
      goOn = false;
    }
    else {
      cards[numCards] = tCard;
      numCards++;
    }
  }
  if(numCards == 0)
    present = false;
  else
    present = true;
  SetCard(0);
  Print();
#ifdef PRINT_VERBOSE
  if(present) {
    for(ullong cardDex = 0; cardDex < numCards; cardDex++) {
      tCard = cards[cardDex];
      fprintf(stdout, "Card[%02d]  --  .%s.\n", cardDex, tCard->cardName); fflush(stdout);
    }
  }
  else {
    fprintf(stdout, "NO CARDS FOUND !\n");
  }
#endif
  return;
}


void  Alsa::SetCard(ullong i_cardNo) {
  if(i_cardNo > numCards - 1)
    i_cardNo  = numCards - 1;
  cardNo      = i_cardNo;
  theCard     = cards[cardNo];

  theIfc      = theCard->GetIfc();
  ifcNo       = theCard->GetIfcNo();
  numIfcs     = theCard->GetNumIfcs();

  theDev      = theIfc->GetDev();
  devNo       = theIfc->GetDevNo();
  numDevs     = theIfc->GetNumDevs();
  return;
}
void  Alsa::SetIfc(ullong i_ifcNo) {
  theIfc      = theCard->SetIfc(i_ifcNo);
  ifcNo       = theCard->GetIfcNo();
  numIfcs     = theCard->GetNumIfcs();

  theDev      = theIfc->GetDev();
  devNo       = theIfc->GetDevNo();
  numDevs     = theIfc->GetNumDevs();
  return;
}

void  Alsa::SetDev(ullong i_devDex) {
  devNo     = theIfc->SetDev(i_devDex);
  theDev    = theIfc->GetDev();
  numDevs   = theIfc->GetNumDevs();
  return;
}



void  Alsa::PrintPcmSpecList(void) {
  ullong ifcDex;
  ullong numDevs;
  char   theName[MAX_LIST_LEN];
  char   tStr[MAX_LIST_LEN];

  ifcDex   = AlsaIfTypes::EAIT_PCM;                       // We only care about the PCM interface since only it can be bit-accurate.
  fprintf(stdout, "PCM Spec List:\n"); fflush(stdout);
  for(ullong cardDex = 0; cardDex < numCards; cardDex++) {
    SetCard(cardDex);
    SetIfc(ifcDex); // Has to be done after SetCard
    numDevs = theIfc->GetNumDevs();
    for(ullong devNo = 0; devNo < numDevs; devNo++) {
      SetDev(devNo);

      sprintf(theName, "cardDex[%02lld]  ", cardDex);
      sprintf(tStr, "ifcNo[%02lld]  ", ifcDex);
      strcat(theName, tStr);
      sprintf(theName, "devNo[%02lld]    ", devNo);
      strcat(theName, tStr);

      strcat(theName, "name ");
      strcpy(tStr, ".");
      strcat(tStr, theDev->GetAADevNm());
      strcat(tStr, ".");
      PadSpaceTrail(tStr, 16);
      strcat(theName, tStr);

      strcat(theName, "shortName ");
      strcpy(tStr, ".");
      strcat(tStr, theCard->GetCardNm());
      strcat(tStr, ".");
      PadSpaceTrail(tStr, 16);
      strcat(theName, tStr);

      strcat(theName, "devNo ");
      sprintf(tStr, "%02lld    ", theDev->GetAADevNo());
      strcat(theName, tStr);

      strcat(theName, " desc1: ");
      strcpy(tStr, ".");
      strcat(tStr, theDev->GetAADesc());
      strcat(tStr, ".");
      PadSpaceTrail(tStr, 36);
      strcat(theName, tStr);

      strcpy(tStr, theDev->GetHintALSA());
      fprintf(stdout, "      %s\n        whole hint ####%s####\n", theName, tStr); fflush(stdout);
      }
    }
  return;
  }
void  Alsa::Print(void) {
//AlsaDevice *td = new AlsaDevice(0, 0, 0, NULL);
//delete td;
  fprintf(stdout, "\n\n\n\n\n\n\n\nAlsa Version: .%s.\n", version); fflush(stdout);
  for(ullong cardDex=0; cardDex<numCards; cardDex++) {
    cards[cardDex]->Print();
  }
#if 0
  //=================================================================================================
    fprintf(stdout, "Inputs:\n");
    for(uint i=0; i<numCards; i++) {
      fprintf(stdout, "  theCard[%d] %s  (human) '%s'\n", i, cards[i]->ifcs[3]->devs[0]->GetCard(), cards[i]->ifcs[3]->devs[0]->GetDesc1());
      for(uint j=0; j<cards[i]->ifcs[3]->numDevs; j++) {
        if(
            (strcmp(cards[i]->ifcs[3]->devs[j]->GetName(), "hw") == 0) &&
            (
              (strcmp(cards[i]->ifcs[3]->devs[j]->GetIoId(), "UNSPECIFIED") == 0) ||
              (strcmp(cards[i]->ifcs[3]->devs[j]->GetIoId(), "Input") == 0)
            )
          ) {
          int devNo;
          devNo = cards[i]->ifcs[3]->devs[j]->GetHDevNo();
          fprintf(stdout, "    pcm hw I/F[%2d] %d  %s\n", j, devNo, cards[i]->ifcs[3]->devs[j]->GetDName());
        }
      }
    }
  //=================================================================================================
    fprintf(stdout, "Outputs:\n");
    for(uint i=0; i<numCards; i++) {
      fprintf(stdout, "  theCard[%d] %s  (human) '%s'\n", i, cards[i]->ifcs[3]->devs[0]->GetCard(), cards[i]->ifcs[3]->devs[0]->GetDesc1());
      for(uint j=0; j<cards[i]->ifcs[3]->numDevs; j++) {
        if(
            (strcmp(cards[i]->ifcs[3]->devs[j]->GetName(), "hw") == 0) &&
            (
              (strcmp(cards[i]->ifcs[3]->devs[j]->GetIoId(), "UNSPECIFIED") == 0) ||
              (strcmp(cards[i]->ifcs[3]->devs[j]->GetIoId(), "Output") == 0)
            )
          ) {
          int devNo;
          devNo = cards[i]->ifcs[3]->devs[j]->GetHDevNo();
          fprintf(stdout, "    pcm hw I/F[%2d] %d  %s\n", j, devNo, cards[i]->ifcs[3]->devs[j]->GetDName());
        }
      }
    }
#endif
  return;
  }
//_________________________________________________________________________________________________
