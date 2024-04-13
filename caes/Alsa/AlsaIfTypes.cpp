
// $Id: AlsaIfTypes.cpp 228 2022-12-25 04:52:32Z duncang $

//=================================================================================================
// Original File Name : AlsaIfTypes.cpp
// Original Author    : duncang
// Creation Date      : Dec 18, 2010
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//=================================================================================================

#include "AlsaIfTypes.hpp"
#include <string.h>

//==== I have no idea where this list comes from except that the ALSA source (v1.2.6.1) INSTALL

char * const AlsaIfTypes::typeNames[] =  { // MAGICK  These strings are magic fixed values in the code, no factory or introspection.
  (char *)"card"    ,
  (char *)"hwdep"   ,
  (char *)"ctl"     ,
  (char *)"pcm"     ,
  (char *)"rawmidi" ,
  (char *)"timer"   ,
  (char *)"seq"
};

//AlsaIfTypes::AlsaIfTypes() {}
//AlsaIfTypes::~AlsaIfTypes() {}
