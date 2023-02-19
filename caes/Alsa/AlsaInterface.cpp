
// $Id: AlsaInterface.cpp 231 2022-12-29 02:47:03Z duncang $

//=================================================================================================
// Original File Name : AlsaInterface.cpp
// Original Author    : duncang
// Creation Date      : 2010-12-18
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//     Third level down
//
//=================================================================================================

#include "AlsaInterface.hpp"
#include "AlsaIfTypes.hpp"
#include <string.h>
#include <malloc.h>

     AlsaInterface::AlsaInterface(ullong i_cardNo, ullong i_ifcNo)
: cardNo(i_cardNo)
, ifcNo(i_ifcNo) {
  ifcName    = AlsaIfTypes::typeNames[ifcNo];
  devs       = new AlsaDevice *[MAX_NAME_LEN];
  for(ullong devDex=0; devDex<MAX_NAME_LEN; devDex++)
    devs[devDex] = NULL;
  numDevs    = 0;

  Rescan();
  return;
}
     AlsaInterface::~AlsaInterface() {
  Wipe();
  delete(devs);
  }
void AlsaInterface::Wipe(void) {
  for(ullong devDex=0; devDex<MAX_NAME_LEN; devDex++) {
    if(devs[devDex] != NULL)
      delete devs[devDex];
    devs[devDex] = NULL;
    }
  // WARNING, Wipe mustn't kill cardNo or ifcNo
  numDevs    =  0;
  devNo      =  0;
  theDev     =  devs[devNo];
  present    =  false;
  return;
}

void AlsaInterface::Rescan( void ) {
  char    **tHints;
  char     *tHint;
  bool      keepOn;


  // This is a weird one ... Maybe I'll have to FIXME it.  pcm has the HINT but I don't really know for other Ifc types.

  Wipe();
  tHints = NULL;
  //  We get devices by asking about the hints, WTF   Yes, it really is.
  snd_device_name_hint((int)cardNo, AlsaIfTypes::typeNames[ifcNo], (void ***)&tHints); // cardNo must be int, never uint ... etc.
  if(*tHints == NULL) {
    fprintf(stdout, "HINTLESS  Card: %Lu  Ifc: %s\n", cardNo, AlsaIfTypes::typeNames[ifcNo]); fflush(stdout);
  }
  else {
    fprintf(stdout, "HINTFULL  Card: %Lu  Ifc: %s\n", cardNo, AlsaIfTypes::typeNames[ifcNo]); fflush(stdout);
  }
  keepOn  = true;
  present = false;
  numDevs = 0;
  while(keepOn) {
    if(tHints[numDevs] != NULL) {
      tHint = tHints[numDevs];
      devs[numDevs] = new AlsaDevice(cardNo, ifcNo, numDevs, tHint);
      numDevs++;
    }
    else {
      keepOn = false;
    }
  }
  if(numDevs > 0)
    present = true;
  snd_device_name_free_hint((void **)tHints);

  SetDev(0);

#ifdef PRINT_VERBOSE
  fprintf(stdout, "         ifNo[%02d]: ifcName: %12s     numIfs:  %2d\n", ifcNo, ifcName, numDevs);
  for(uint dD=0; dD<numDevs; dD++) {
    fprintf(stdout, "            %12s dev[%02d] hint: %s\n", ifcName, devNo, tStr[devNo]);
    devs[dD]->Print();
  }
#endif

  return;
}

ullong AlsaInterface::SetDev(ullong i_devNo) {

  if(numDevs == 0) {
    i_devNo = 0;
    devNo   = 0;
    theDev   = NULL;
  }
  else {
    if(i_devNo > numDevs - 1)
      devNo = numDevs - 1;
    devNo   = i_devNo;
    theDev   = devs[devNo];
  }
  return devNo;
}
AlsaDevice *AlsaInterface::GetDev(ullong i_devNo) {
  if(numDevs == 0)
    return NULL;
  else if(i_devNo > numDevs - 1)
    i_devNo = numDevs - 1;
  return devs[i_devNo];
}
void AlsaInterface::Print(void) {
  fprintf(stdout, "Interface--[%02lld]  .%s.\n", ifcNo, ifcName); fflush(stdout);
  for(ullong devDex=0; devDex < numDevs; devDex++) {
    devs[devDex]->Print();
  }
  return;
}




