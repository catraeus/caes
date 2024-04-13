
// $Id: AlsaIfTypes.hpp 227 2022-12-25 00:44:45Z duncang $

//=================================================================================================
// Original File Name : AlsaIfTypes.h
// Original Author    : duncang
// Creation Date      : Dec 18, 2010
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//
//=================================================================================================

#ifndef __ALSAIFTYPES_H_
#define __ALSAIFTYPES_H_
#include <alsa/asoundlib.h>
#include <string>
#include "AlsaDevice.hpp"

struct AlsaIfTypes {
    //AlsaIfTypes();
    //~AlsaIfTypes();
    typedef enum eAlsaIfTypes_e {
      // is in _snd_ctl_elem_iface  from alsa/control.h
      // is in _snd_dev_type        from include/aserver.h in the alsa build structure
      // is in snd_device_name_hint from control/namehint.c where it will return error if not in the list.
      // last checked 2022-12-18
      // MAGICK  These have to lock step to the strings in typeNames[];
      EAIT_CARD     = 0,
      EAIT_HWDEP    = 1,
      EAIT_CTL      = 2,
      EAIT_PCM      = 3,
      EAIT_RAWMIDI  = 4,
      EAIT_TIMER    = 5,
      EAIT_SEQ      = 6,
      EATI_BIGG
    } eAlsaIfTypes;
    typedef enum eAlsaPcmTypes_e {
      // from _snd_pcm_type in alsa/pcm.h Last checked 2022-12-12
      // must correlate to
      EAPT_HW           = SND_PCM_TYPE_HW          ,   // Kernel level PCM
      EAPT_HOOKS        = SND_PCM_TYPE_HOOKS       ,   // Hooked PCM
      EAPT_MULTI        = SND_PCM_TYPE_MULTI       ,   // One or more linked PCM with exclusive access to selected channels
      EAPT_FILE         = SND_PCM_TYPE_FILE        ,   // File writing plugin
      EAPT_NULL         = SND_PCM_TYPE_NULL        ,   // Null endpoint PCM
      EAPT_SHM          = SND_PCM_TYPE_SHM         ,   // Shared memory client PCM
      EAPT_INET         = SND_PCM_TYPE_INET        ,   // INET client PCM (not yet implemented)
      EAPT_COPY         = SND_PCM_TYPE_COPY        ,   // Copying plugin
      EAPT_LINEAR       = SND_PCM_TYPE_LINEAR      ,   // Linear format conversion PCM
      EAPT_ALAW         = SND_PCM_TYPE_ALAW        ,   // A-Law format conversion PCM
      EAPT_MULAW        = SND_PCM_TYPE_MULAW       ,   // Mu-Law format conversion PCM
      EAPT_ADPCM        = SND_PCM_TYPE_ADPCM       ,   // IMA-ADPCM format conversion PCM
      EAPT_RATE         = SND_PCM_TYPE_RATE        ,   // Rate conversion PCM
      EAPT_ROUTE        = SND_PCM_TYPE_ROUTE       ,   // Attenuated static route PCM
      EAPT_PLUG         = SND_PCM_TYPE_PLUG        ,   // Format adjusted PCM
      EAPT_SHARE        = SND_PCM_TYPE_SHARE       ,   // Sharing PCM
      EAPT_METER        = SND_PCM_TYPE_METER       ,   // Meter plugin
      EAPT_MIX          = SND_PCM_TYPE_MIX         ,   // Mixing PCM
      EAPT_DROUTE       = SND_PCM_TYPE_DROUTE      ,   // Attenuated dynamic route PCM (not yet implemented)
      EAPT_LBSERVER     = SND_PCM_TYPE_LBSERVER    ,   // Loopback server plugin (not yet implemented)
      EAPT_LINEAR_FLOAT = SND_PCM_TYPE_LINEAR_FLOAT,   // Linear Integer <-> Linear Float format conversion PCM
      EAPT_LADSPA       = SND_PCM_TYPE_LADSPA      ,   // LADSPA integration plugin
      EAPT_DMIX         = SND_PCM_TYPE_DMIX        ,   // Direct Mixing plugin
      EAPT_JACK         = SND_PCM_TYPE_JACK        ,   // Jack Audio Connection Kit plugin
      EAPT_DSNOOP       = SND_PCM_TYPE_DSNOOP      ,   // Direct Snooping plugin
      EAPT_DSHARE       = SND_PCM_TYPE_DSHARE      ,   // Direct Sharing plugin
      EAPT_IEC958       = SND_PCM_TYPE_IEC958      ,   // IEC958 subframe plugin
      EAPT_SOFTVOL      = SND_PCM_TYPE_SOFTVOL     ,   // Soft volume plugin
      EAPT_IOPLUG       = SND_PCM_TYPE_IOPLUG      ,   // External I/O plugin
      EAPT_EXTPLUG      = SND_PCM_TYPE_EXTPLUG     ,   // External filter plugin
      EAPT_MMAP_EMUL    = SND_PCM_TYPE_MMAP_EMUL       // Mmap-emulation plugin
    } eAlsaPcmTypes;
    static       char *const typeNames[];
    static const int numTypes=EATI_BIGG;
  };

#endif /* __ALSAIFTYPES_H_ */
