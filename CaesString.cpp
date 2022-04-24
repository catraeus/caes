

//=================================================================================================
// Original File Name : CaesString.cpp
// Original Author    : catraeus
// Creation Date      : Apr 17, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/*
   String Functions that do really cool things.
*/
//
//=================================================================================================
/*
This file is part of Catraeus Libraries.

Catraeus Libraries is free software: you can redistribute it and/or modify it under the terms of
  the GNU Lesser General Public License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

Catraeus Libraries is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
  the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with dmmsnoop.
  If not, see <https://www.gnu.org/licenses/>.
*/
//=================================================================================================

#include <string.h>
#include <stdio.h>
#include "CaesTypes.hpp"
#include "CaesString.hpp"
#include <math.h>
#include <errno.h>

const static char *oomPrefix    = (const char *)"afpnum.kMGTP";      // atto, femto yada yada yada Peta
const static char *oomHexLookup = (const char *)"0123456789ABCDEF";  // Fast and furious, no c format overhead

enum  eSciEng        {ESE_ENG,   ESE_SCI                       };
enum  eStrEngState   {ESES_MANT, ESES_EXP, ESES_DONE, ESES_BAD };

int   ParseEngMark          (char i_c); // Forward Reference so it wouldn't be available to the outside world.

void  ToUpper               (char *i_string) {
  if(i_string == 0)
    return;
  if(strlen(i_string) == 0)
    return;
  char *pSrc = i_string;
  while(*pSrc != '\0') {
    if(*pSrc >= 'a' && *pSrc <= 'z')
      *pSrc += 'A' - 'a';
    pSrc++;
    }
  return;
  }
void  ToLower               (char *i_string) {
  if(i_string == 0)
    return;
  if(strlen(i_string) == 0)
    return;
  char *pSrc = i_string;
  while(*pSrc != '\0') {
    if(*pSrc >= 'A' && *pSrc <= 'Z')
      *pSrc += 'a' - 'A';
    pSrc++;
    }
  return;
  }
void  ToUpperBlock          (char *i_string, ullong i_n) {
  ullong i;
  if(i_n == 0)
    return;
  char *pSrc = i_string;
  i = 0;
  while(i < i_n) {
    if(*pSrc >= 'a' && *pSrc <= 'z')
      *pSrc += 'A' - 'a';
    pSrc++;
    i++;
    }
  return;
  }
void  ToLowerBlock          (char *i_string, ullong i_n) {
  ullong i;
  if(i_n == 0)
    return;
  char *pSrc = i_string;
  i = 0;
  while(i < i_n) {
    if(*pSrc >= 'A' && *pSrc <= 'Z')
      *pSrc += 'a' - 'A';
    pSrc++;
    i++;
    }
  return;
  }
void  CleanupFileName       (char *io_path) {
  char *pSrc;
  char *pDst;
  bool  cleaning;

  pSrc = io_path;
  pDst = io_path;
  while(*pSrc != '\0') {
    if( *pSrc == '\"' ||
        *pSrc == '\'' ||
        *pSrc == '`' ||
        ((*pSrc == '/') && (*(pSrc + 1) == '/')) ||
        0 ) {
      pSrc++;
      cleaning = true;
      }
    else {
      if(cleaning)
        *(pDst++) = *(pSrc++);
      else {
        pDst++;
        pSrc++;
        }
      }
    }
  *pDst = '\0';
  return;
  }
void  CleanupTrailingWhite  (char *io_str) {
  char *pSrc;

  pSrc = &(io_str[strlen(io_str) - 1]);
  while(
        (
         (*pSrc == '\0') ||
         (*pSrc == '\t') ||
         (*pSrc == '\r') ||
         (*pSrc == '\n') ||
         (*pSrc ==  ' ')
        ) &&
       (pSrc != io_str)
      )
    pSrc--;
  pSrc++;
  *pSrc = '\0';

  return;
  }
void  PathStringSplit       (char *i_path, char ***o_dirs, bool *o_isRootLevel, ulong *o_depth) {
  char   tStr[256];
  char  *pSrc;
  char  *pDst;
  char **pList;

  *o_dirs = new char *[255];
  *o_dirs[0] = 0;
  pList = &(*o_dirs[0]);

  pSrc = i_path;

  if(*pSrc == '/') {
    *o_isRootLevel = true;
    }
  else
    *o_isRootLevel = false;

  *o_depth = 0;
  while(*pSrc != '\0') {
    if(*pSrc == '/')
      while(*pSrc == '/')
        pSrc++;
    pDst = tStr;
    while((*pSrc != '\0') && (*pSrc != '/')) {
      *(pDst++) = *(pSrc++);
      }
    *pDst = '\0';
    ulong l = strlen(tStr);
    if(l > 0) {
      *pList = new char[l + 1];
      strcpy(*pList, tStr);
      pList++;
      (*o_depth)++;
      }
    }

  return;
  }
bool  PathExtractFileName   (char *i_path, char *o_fileName) {
  bool  success;
  char *pSrc;
  char *pDst;

  success = false;
  if(o_fileName == 0)
    return false;
  success = false;
  if(strlen(i_path) == 0) {
    *o_fileName = '\0';
    return success;
    }
  if(i_path[strlen(i_path)-1] == '/') {
    *o_fileName = '\0';
    return success;
    }

  success = true;
  pSrc = i_path;
  pDst = o_fileName;
  while(*pSrc != '\0') {
    while((*pSrc != '/') && (*pSrc != '\0')) {
      *(pDst++) = *(pSrc++);
      }
    *pDst = '\0';
    pDst = o_fileName;
    if(*pSrc == '/') {
      pSrc++;
      }
    }
  return success;
  }
bool  PathExtractExt        (char *i_path, char *o_ext) {
	  bool  success;
	  char *pSrc;
	  char *pDst;

	  pDst = o_ext;
	  *pDst = '\0'; // for protection
	  success = false;
	  if(o_ext == 0)
	    return false;
	  success = false;
	  if(strlen(i_path) == 0) {
	    return success;
	    }
	  if(i_path[strlen(i_path)-1] == '/') {
	    return success;
	    }

	  success = true;
	  pSrc = &(i_path[strlen(i_path) - 1]);
	  while((*pSrc != '.') && (pSrc > i_path))
		pSrc--;
	  if(*pSrc != '.')
		return success;
	  pSrc++;
	  while(*pSrc != '\0')
	    *(pDst++) = *(pSrc++);
	  *pDst = '\0';
	  return success;
  }
bool  PathExtractPath       (char *i_path, char *o_path) {
  bool success;
  char *pDst;

  success = false;
  if(o_path == 0)
    return success;
  success = true;
  strcpy(o_path, i_path);
  if(strlen(i_path) == 0)
    return success;
  if(i_path[strlen(i_path)-1] == '/')
    return success;

  pDst = &o_path[strlen(o_path)-1];
  while(*pDst != '/')
    pDst--;
  while(*pDst == '/')
    pDst--;
  pDst++;
  *pDst = '\0';
  return success;
  }
bool  PathExtractLastDir    (char *i_path, char *o_dir) {
  bool    success;
  char   *pTmp;
  ullong  nClip;

  success = true;
  if(strlen(i_path) == 0) {
    *o_dir = '\0';
    success = false;
    return success;
  }
  pTmp = &(i_path[strlen(i_path) - 1]); // start just before the \0
  nClip = 0;
  while((*pTmp == '/') && (pTmp >= i_path)) { // Strip trailing directory /
    pTmp--;
    nClip++;
  }
  if(pTmp < i_path) {// guards against dir= '/'
    *o_dir = '\0';
    success = false;
    return success;
  }
  while((*pTmp != '/') && (pTmp >= i_path)) {
    pTmp--;
  }
  pTmp++;
  strcpy(o_dir, pTmp);
  o_dir[strlen(pTmp) - nClip] = '\0'; // strip away those ransid trailing '/'
  return success;
}
bool  PathExtractBase       (char *i_path, char *o_base) {
  llong nStart;
  llong nStop;
  llong i;


  if(o_base == NULL)
    return false;
  *o_base = '\0';
  if(strlen(i_path) == 0)
    return true;

  nStop = strlen(i_path) - 1;
  while((i_path[nStop] != '/') && (i_path[nStop] != '.') && (nStop > 0))
    nStop--;
  if(nStop == 0) {
    if(i_path[nStop] == '.') // The whole damned thing is an extension (aka hidden file)
      strcpy(o_base, &(i_path[nStop + 1])); // return the unhidden file name.
    else if(i_path[nStop] == '/') // There is no extension on a root level FQ pathed file name
      strcpy(o_base, &(i_path[nStop])); // return the dug out file name
    else // There is no "/" and there is no "." so the whole thing is a file without extension.
      strcpy(o_base, &(i_path[nStop])); // return the dug out file name
    return true;
  }

  // so here we fall through.  It is either a "." or a "/"
  if(i_path[nStop] == '/') { // There is no extension on an otherwise longish pathed file name
    strcpy(o_base, &(i_path[nStop + 1])); // return the unpathed file name.
    return true;
  }
  // We aren't at 0, so we must have a "." ... the extension, lets go deeper
  nStart = nStop;
  while((i_path[nStart] != '/') && (nStart >= 0))
    nStart--;
  if(nStart == 0) {
    if(i_path[nStart] == '/') // There is no extension on a root level FQ pathed file name
      nStart++; // prep for the slosh dug out file name
    // the else is implied, the file has no leading path so just start where this while ended
  }
  else // We MUST be at a '/'
    nStart++;
  i = 0;
  while(nStart < nStop)
    o_base[i++] = i_path[nStart++];
  o_base[i] = '\0';

  return true;
  }
void  PathNameCleanup       (char *io_path) {
  char *pSrc;
  char *pDst;
  bool  cleaning;

  pSrc = io_path;
  pDst = io_path;
  while(*pSrc != '\0') {
    if( *pSrc == '\"' ||
        *pSrc == '\'' ||
        *pSrc == '`' ||
        ((*pSrc == '/') && (*(pSrc + 1) == '/')) ||
        0 ) {
      pSrc++;
      cleaning = true;
      }
    else {
      if(cleaning)
        *(pDst++) = *(pSrc++);
      else {
        pDst++;
        pSrc++;
        }
      }
    }
  *pDst = '\0';
  return;
  }
void  SecToHMS              (char *o_tStr, double i_t, bool i_padHours) {
  int    hr;
  int    min;
  int    sec;
  double frac;
  char   tStr[1024];

  sec = floor(i_t);
  frac = i_t - (double)sec;
  min = sec;
  sec %= 60;
  min /= 60;
  hr = min;
  min %= 60;
  hr  /= 60;

  o_tStr[0] = '\0';
  if((hr > 0) || i_padHours)
    sprintf(o_tStr, "%02d:", hr);
  if((min > 0) || i_padHours) {
    sprintf(tStr, "%02d:", min);
    strcat(o_tStr, tStr);
    }
  frac += sec;
  if(sec < 10) {
    strcat(o_tStr, "0");
    sprintf(tStr, "%5.4f", frac);
    }
  else
    sprintf(tStr, "%6.4f", frac);
  strcat(o_tStr, tStr);
  return;
  }
int   HMStoSec              (double *o_T,      char   *i_tStr) {
  /*
  Rule 1:  there can be only numbers, : and .
  Rule 2:  the . has to be the last non number and only one of them
  Process:
    Protect string with "0" initial fill.
    Accumulate sscanf float string from end and find last non-num
      If none OR non :
        mark done for next loop
      else
        Discard last :
      accumulate sec
    Accumulate sscanf int string from end and find last non-num
      if none OR non :
        mark done for next loop
      else
        discard last :
      accumulate minutes = 60 sec per.
    Accumulte sscanf int string from end and find last non-num
      accumulate hours = 3600 per sec
    DONE
  */
  int    N;
  char  *pSrc;
  double T;
  char   tStr[256];

  //========= Setup and protection
  strcpy(tStr, i_tStr);
  if(strlen(tStr) == 0)
    return 0;
  pSrc = &(tStr[strlen(tStr) - 1]);

  //========= Search backward to first non-N/.  ====== SECONDS ======
  while(     ( (pSrc >= tStr) && (*pSrc >= '0') && (*pSrc <= '9') )       ||      (*pSrc == '.')           ) {
    pSrc--;
    }
  pSrc++;
  if(strlen(pSrc) == 0) { // No seconds after the last non-num/:
    *o_T = 0.0;
    return 0; // even worse, since there was nothing, we didn't find anything even close to qualifying
    }
  sscanf(pSrc, "%lf", &T);
  *o_T = T;
  if(T >= 60.0) { // BARF!  Seconds are modulo 60.
    *o_T = 0.0;
    return 0;
    }
  pSrc--;
  if((*pSrc != ':') || (pSrc <= tStr))
    return 1; // but now we did find seconds, maybe with garbage before it, maybe only a float number
  *pSrc = '\0'; // null terminate the next number
  pSrc--;  // move to the presumed next number

  //========= Search backward to first non-N/.  ====== MINUTES ======
  // get past the : if it's a : and
  while( (pSrc >= tStr) && (*pSrc >= '0') && (*pSrc <= '9') ) {
    pSrc--;
    }
  pSrc++;
  if(strlen(pSrc) == 0) { // No seconds after the last non-num/: ... even works past beginning.
    N  = 0;
    }
  else
    sscanf(pSrc, "%d", &N);
  if(N >= 60) { // BARF!  Minutes are modulo 60.
    *o_T = 0.0;
    return 0;
    }
  T += 60.0 * (double)N;
  *o_T = T;
  pSrc--;
  if((*pSrc != ':') || (pSrc <= tStr))
    return 1; // but now we did find minutes, maybe with garbage before it, we're done
  *pSrc = '\0'; // null terminate the next number
  pSrc--;  // move to the presumed next number




  //========= Search backward to first non-N/.  ====== HOURS ======
  // get past the : if it's a : and
  while( (pSrc >= tStr) && (*pSrc >= '0') && (*pSrc <= '9') ) {
    pSrc--;
    }
  pSrc++;
  if(strlen(pSrc) == 0) { // No seconds after the last non-num/: ... even works past beginning.
    N  = 0;
    }
  else
    sscanf(pSrc, "%d", &N);
  if(N >= 60) { // BARF!  Minutes are modulo 60.
    *o_T = 0.0;
    return 0;
    }
  T += 3600.0 * (double)N;
  *o_T = T;
// We're Done no matter what floobydust is left
  return 1;
  }
void  ErrNoToString         (int i_errno, char *o_string) {
  switch(i_errno) {
    case 0                    : sprintf(o_string, "No error found."); break;
    case EPERM                : sprintf(o_string, "Operation not permitted."); break;
    case ENOENT               : sprintf(o_string, "No such file or directory."); break;
    case ESRCH                : sprintf(o_string, "No such process."); break;
    case EINTR                : sprintf(o_string, "Interrupted system call."); break;
    case EIO                  : sprintf(o_string, "I/O error."); break;
    case ENXIO                : sprintf(o_string, "No such device or address."); break;
    case E2BIG                : sprintf(o_string, "Argument list too long."); break;
    case ENOEXEC              : sprintf(o_string, "Exec format error."); break;
    case EBADF                : sprintf(o_string, "Bad file number."); break;
    case ECHILD               : sprintf(o_string, "No child processes."); break;
    case EAGAIN               : sprintf(o_string, "Either you must try again or Operation would block."); break;
    case ENOMEM               : sprintf(o_string, "Out of memory."); break;
    case EACCES               : sprintf(o_string, "Permission denied."); break;
    case EFAULT               : sprintf(o_string, "Bad address."); break;
    case ENOTBLK              : sprintf(o_string, "Block device required."); break;
    case EBUSY                : sprintf(o_string, "Device or resource busy."); break;
    case EEXIST               : sprintf(o_string, "File exists."); break;
    case EXDEV                : sprintf(o_string, "Cross-device link."); break;
    case ENODEV               : sprintf(o_string, "No such device."); break;
    case ENOTDIR              : sprintf(o_string, "Not a directory."); break;
    case EISDIR               : sprintf(o_string, "Is a directory."); break;
    case EINVAL               : sprintf(o_string, "Invalid argument."); break;
    case ENFILE               : sprintf(o_string, "File table overflow."); break;
    case EMFILE               : sprintf(o_string, "Too many open files."); break;
    case ENOTTY               : sprintf(o_string, "Not a typewriter."); break;
    case ETXTBSY              : sprintf(o_string, "Text file being accessed is already busy running something."); break;
    case EFBIG                : sprintf(o_string, "File too large."); break;
    case ENOSPC               : sprintf(o_string, "No space left on device."); break;
    case ESPIPE               : sprintf(o_string, "Illegal seek."); break;
    case EROFS                : sprintf(o_string, "Write access of a read-only location."); break;
    case EMLINK               : sprintf(o_string, "Too many links."); break;
    case EPIPE                : sprintf(o_string, "Broken pipe."); break;
    case EDOM                 : sprintf(o_string, "Math argument out of domain of func."); break;
    case ERANGE               : sprintf(o_string, "Math result not representable."); break;
    case EDEADLK              : sprintf(o_string, "Resource deadlock would occur."); break;
  //case EDEADLOCK            : sprintf(o_string, "Resource deadlock would occur."); break;
    case ENAMETOOLONG         : sprintf(o_string, "File name too long."); break;
    case ENOLCK               : sprintf(o_string, "No record locks available."); break;
    case ENOSYS               : sprintf(o_string, "Function not implemented."); break;
    case ENOTEMPTY            : sprintf(o_string, "Directory not empty."); break;
    case ELOOP                : sprintf(o_string, "Too many symbolic links encountered."); break;
  //case EWOULDBLOCK          : sprintf(o_string, "Operation would block."); break; // Same as EAGAIN
    case ENOMSG               : sprintf(o_string, "No message of desired type."); break;
    case EIDRM                : sprintf(o_string, "Identifier removed."); break;
    case ECHRNG               : sprintf(o_string, "Channel number out of range."); break;
    case EL2NSYNC             : sprintf(o_string, "Level 2 not synchronized."); break;
    case EL3HLT               : sprintf(o_string, "Level 3 halted."); break;
    case EL3RST               : sprintf(o_string, "Level 3 reset."); break;
    case ELNRNG               : sprintf(o_string, "Link number out of range."); break;
    case EUNATCH              : sprintf(o_string, "Protocol driver not attached."); break;
    case ENOCSI               : sprintf(o_string, "No CSI structure available."); break;
    case EL2HLT               : sprintf(o_string, "Level 2 halted."); break;
    case EBADE                : sprintf(o_string, "Invalid exchange."); break;
    case EBADR                : sprintf(o_string, "Invalid request descriptor."); break;
    case EXFULL               : sprintf(o_string, "Exchange full."); break;
    case ENOANO               : sprintf(o_string, "No anode."); break;
    case EBADRQC              : sprintf(o_string, "Invalid request code."); break;
    case EBADSLT              : sprintf(o_string, "Invalid slot."); break;
    case EBFONT               : sprintf(o_string, "Bad font file format."); break;
    case ENOSTR               : sprintf(o_string, "Device not a stream."); break;
    case ENODATA              : sprintf(o_string, "No data available."); break;
    case ETIME                : sprintf(o_string, "Timer expired."); break;
    case ENOSR                : sprintf(o_string, "Out of streams resources."); break;
    case ENONET               : sprintf(o_string, "Machine is not on the network."); break;
    case ENOPKG               : sprintf(o_string, "Package not installed."); break;
    case EREMOTE              : sprintf(o_string, "Object is remote."); break;
    case ENOLINK              : sprintf(o_string, "Link has been severed."); break;
    case EADV                 : sprintf(o_string, "Advertise error."); break;
    case ESRMNT               : sprintf(o_string, "Srmount error."); break;
    case ECOMM                : sprintf(o_string, "Communication error on send."); break;
    case EPROTO               : sprintf(o_string, "Protocol error."); break;
    case EMULTIHOP            : sprintf(o_string, "Multihop attempted."); break;
    case EDOTDOT              : sprintf(o_string, "RFS specific error."); break;
    case EBADMSG              : sprintf(o_string, "Not a data message."); break;
    case EOVERFLOW            : sprintf(o_string, "Value too large for defined data type."); break;
    case ENOTUNIQ             : sprintf(o_string, "Name not unique on network."); break;
    case EBADFD               : sprintf(o_string, "File descriptor in bad state."); break;
    case EREMCHG              : sprintf(o_string, "Remote address changed."); break;
    case ELIBACC              : sprintf(o_string, "Can not access a needed shared library."); break;
    case ELIBBAD              : sprintf(o_string, "Accessing a corrupted shared library."); break;
    case ELIBSCN              : sprintf(o_string, ".lib section in a.out corrupted."); break;
    case ELIBMAX              : sprintf(o_string, "Attempting to link in too many shared libraries."); break;
    case ELIBEXEC             : sprintf(o_string, "Cannot exec a shared library directly."); break;
    case EILSEQ               : sprintf(o_string, "Illegal byte sequence."); break;
    case ERESTART             : sprintf(o_string, "Interrupted system call should be restarted."); break;
    case ESTRPIPE             : sprintf(o_string, "Streams pipe error."); break;
    case EUSERS               : sprintf(o_string, "Too many users."); break;
    case ENOTSOCK             : sprintf(o_string, "Socket operation on non-socket."); break;
    case EDESTADDRREQ         : sprintf(o_string, "Destination address required."); break;
    case EMSGSIZE             : sprintf(o_string, "Message too long."); break;
    case EPROTOTYPE           : sprintf(o_string, "Protocol wrong type for socket."); break;
    case ENOPROTOOPT          : sprintf(o_string, "Protocol not available."); break;
    case EPROTONOSUPPORT      : sprintf(o_string, "Protocol not supported."); break;
    case ESOCKTNOSUPPORT      : sprintf(o_string, "Socket type not supported."); break;
    case EOPNOTSUPP           : sprintf(o_string, "Operation not supported on transport endpoint."); break;
    case EPFNOSUPPORT         : sprintf(o_string, "Protocol family not supported."); break;
    case EAFNOSUPPORT         : sprintf(o_string, "Address family not supported by protocol."); break;
    case EADDRINUSE           : sprintf(o_string, "Address already in use."); break;
    case EADDRNOTAVAIL        : sprintf(o_string, "Cannot assign requested address."); break;
    case ENETDOWN             : sprintf(o_string, "Network is down."); break;
    case ENETUNREACH          : sprintf(o_string, "Network is unreachable."); break;
    case ENETRESET            : sprintf(o_string, "Network dropped connection because of reset."); break;
    case ECONNABORTED         : sprintf(o_string, "Software caused connection abort."); break;
    case ECONNRESET           : sprintf(o_string, "Connection reset by peer."); break;
    case ENOBUFS              : sprintf(o_string, "No buffer space available."); break;
    case EISCONN              : sprintf(o_string, "Transport endpoint is already connected."); break;
    case ENOTCONN             : sprintf(o_string, "Transport endpoint is not connected."); break;
    case ESHUTDOWN            : sprintf(o_string, "Cannot send after transport endpoint shutdown."); break;
    case ETOOMANYREFS         : sprintf(o_string, "Too many references: cannot splice."); break;
    case ETIMEDOUT            : sprintf(o_string, "Connection timed out."); break;
    case ECONNREFUSED         : sprintf(o_string, "Connection refused."); break;
    case EHOSTDOWN            : sprintf(o_string, "Host is down."); break;
    case EHOSTUNREACH         : sprintf(o_string, "No route to host."); break;
    case EALREADY             : sprintf(o_string, "Operation already in progress."); break;
    case EINPROGRESS          : sprintf(o_string, "Operation now in progress."); break;
    case ESTALE               : sprintf(o_string, "Stale NFS file handle."); break;
    case EUCLEAN              : sprintf(o_string, "Structure needs cleaning."); break;
    case ENOTNAM              : sprintf(o_string, "Not a XENIX named type file."); break;
    case ENAVAIL              : sprintf(o_string, "No XENIX semaphores available."); break;
    case EISNAM               : sprintf(o_string, "Is a named type file."); break;
    case EREMOTEIO            : sprintf(o_string, "Remote I/O error."); break;
    case EDQUOT               : sprintf(o_string, "Quota exceeded."); break;
    case ENOMEDIUM            : sprintf(o_string, "No medium found."); break;
    case EMEDIUMTYPE          : sprintf(o_string, "Wrong medium type."); break;
    case ECANCELED            : sprintf(o_string, "Operation Canceled."); break;
    case ENOKEY               : sprintf(o_string, "Required key not available."); break;
    case EKEYEXPIRED          : sprintf(o_string, "Key has expired."); break;
    case EKEYREVOKED          : sprintf(o_string, "Key has been revoked."); break;
    case EKEYREJECTED         : sprintf(o_string, "Key was rejected by service."); break;
    case EOWNERDEAD           : sprintf(o_string, "Owner died."); break;
    case ENOTRECOVERABLE      : sprintf(o_string, "State not recoverable."); break;
    case ERFKILL              : sprintf(o_string, "Operation not possible due to RF-kill."); break;
    default                   : sprintf(o_string, "Damn! Where did that errno come from: %d", errno); break;
    }
  return;
  }
void  StripTrailingNP       (char *i_s) {
  char *p;

  p = &(i_s[strlen(i_s)]);
  while((p >= i_s) && ((*p < '!') || (*p > '~'))) {
    p--;
    }
  p++;
  *p = '\0';
  return;
  }
void  StripLeadingNP        (char *i_s) {
  char *pSrc;
  char *pDst;
  char *pEnd;

  pEnd = &(i_s[strlen(i_s)]);
  if(pEnd == i_s)
    return;
  pSrc = i_s;
  pDst = i_s;
  while((pSrc < pEnd) && ((*pSrc < '!') || (*pSrc > '~'))) {
    pSrc++;
    }
  if(pSrc == i_s)
    return;

  while(pSrc < pEnd) {
    *(pDst++) = *(pSrc++);
    }
  *pDst = '\0';
  return;
  }
char *FindAnyCharList       (char *i_s, char *i_f) {
  char *pFound;
  char *pRef;
  bool  oops;

  if(*i_s == '\0')
    return 0;
  if(*i_f == '\0')
    return 0;
  pFound = i_s; // Will be the pointer to the found occurrence.
  oops = false;
  while(!oops) {
    pRef  = i_f;      // The recycling checker in the ref string.
    while(!oops) {
      if(*pRef == '\0')
        oops = true; // artificially break out of loop, but check down there
      else {
        if(*pRef == *pFound)
          return pFound;
        pRef++;
        }
      }
    if(*pRef  == '\0')
      oops = false;
    pFound++;
    if(*pFound == '\0') {
      return 0;
      }
    }
  return(0);
  }
void  StripAllNP            (char *io_s) {
  char *pSrc;
  char *pDst;
  char *pEnd;

  pEnd = &(io_s[strlen(io_s)]);
  if(pEnd == io_s)
    return;
  pSrc = io_s;
  pDst = io_s;
  while(pSrc < pEnd) {
    if((*pSrc >= '!') && (*pSrc <= '~')) {
      char c;
      c = *pSrc;
      *(pDst++) = c;
      }
    pSrc++;
    }
  *pDst = '\0';
  return;
  }
void  CompactAllNP          (char *io_s) {
  char *pSrc;
  char *pDst;
  char *pEnd;
  bool  gotOne;

  pEnd = &(io_s[strlen(io_s)]);
  if(pEnd == io_s)
    return;
  pSrc = io_s;
  pDst = io_s;
  gotOne = false;
  while(pSrc < pEnd) {
    if((*pSrc >= '!') && (*pSrc <= '~')) {
      char c;
      c = *pSrc;
      *(pDst++) = c;
      gotOne = false;
      }
    else {
      if(!gotOne)
        *(pDst++) = ' ';
      gotOne = true;
    }
    pSrc++;
    }
  *pDst = '\0';
  return;
  }
void  IntWithComma          (char *o_tStr, llong  i_i) {
  char  tStr[1024];
  long  len;
  char *pSrc;
  char *pDst;
  long  i;

  i = 0;
  pDst = o_tStr;
  pSrc = tStr;
  if(i_i < 0) {
    *(pDst++) = '-';
    i_i *= -1;
    }
  sprintf(tStr, "%lld", i_i);
  len = strlen(tStr);
  *(pDst++) = *(pSrc++);
  i++;
  while(i < len) {
    if(((len - i) % 3) == 0)
      *(pDst++) = ',';
    *(pDst++) = *(pSrc++);
    i++;
    }
  *(pDst++) = '\0';
  return;
  }
void  IntWithComma          (char *o_tStr, double i_f) {
  llong kk = (llong)i_f;
  IntWithComma(o_tStr, kk);
  return;
  }
void  IntWithComma          (char *o_tStr, float  i_f) {
  double dd;
  dd = (double)i_f;
  IntWithComma(o_tStr, dd);
  return;
  }
void  IntWithComma          (char *o_tStr, int i_i) {
  long long int kk = (long long int)i_i;
  IntWithComma(o_tStr, kk);
  return;
  }
void  IntWithComma          (char *o_tStr, uint i_u) {
  long long int kk = (long long int)i_u;
  IntWithComma(o_tStr, kk);
  return;
  }
void  IntWithComma          (char *o_tStr, ullong i_i) {
  llong ll;
  ll = (llong)i_i;
  IntWithComma(o_tStr, ll);
  return;
  }
void  HexWith0x             (char *o_tStr, uint i_u, llong i_places) {
  int b;
  uint dex;

  for(dex=0; ((dex < i_places) && (dex < sizeof(uint))); dex++) {
    b = i_u & 0x0f;
    i_u >>= 4;
    o_tStr[i_places + 1 - dex] = oomHexLookup[b];
  }
  o_tStr[0] = '0';
  o_tStr[1] = 'x';
  o_tStr[i_places + 2] = '\0';
  return;
}
//Convert
void  EngString             (char *o_tStr, double i_f, llong i_places,  char *i_units) {
  llong millenium;
  char fStr[256];
  char sStr[2];

  if(i_f == 0.0) {
    sprintf(fStr, "%%.%lldlf %s", i_places, i_units);
    sprintf(o_tStr, fStr, 0.0);
    return;
    }

  if(i_f < 0.0) {
    sStr[0] = '-';
    sStr[1] = '\0';
    i_f *= -1.0;
    }
  else {
    sStr[0] = '\0';
    }

  millenium = 0;
  while((i_f < 0.999999999999) && (millenium > -18)) {
    i_f *= 1000.0;
    millenium -= 3;
    }
  while((i_f > 999.999999999) && (millenium < 15)) {
    i_f /= 1000.0;
    millenium += 3;
    }

  if((millenium == 15) && (i_f >= 1000.0)) {
    o_tStr[0] = '\0';
    strcat(o_tStr, sStr);
    strcat(o_tStr, "big");
    }
  else if((millenium == -18) &&(i_f < 1.0)) {
    o_tStr[0] = '\0';
    strcat(o_tStr, sStr);
    strcat(o_tStr, "small");
    }
  else {
    millenium += 18;
    millenium /= 3;
         if(i_f > 99.99999999)
      i_places -= 3;
    else if(i_f > 9.99999999)
      i_places -= 2;
    else
      i_places -= 1;
    if(millenium == 6)
      sprintf(fStr, "%s%%.%lldlf %s", sStr, i_places, i_units);
    else
      sprintf(fStr, "%s%%.%lldlf %c%s", sStr, i_places, oomPrefix[millenium], i_units);
    sprintf(o_tStr, fStr, i_f);
    }
  return;
  }
bool  StringEng             (char *i_tStr, double *o_f) {
  eSciEng      sciEng   = ESE_ENG;
  eStrEngState state    = ESES_MANT;
  bool         gotMantPeriod = false;
  bool         gotEngMark    = false;
  double       dMant    = 1.0;
  int          iExp     = 0;
  char         sMant[256];
  char         sExp[256];
  char        *pSrc;
  char        *pDst;

  pSrc = i_tStr;
  StripAllNP(pSrc);
  sMant[0] = '\0';
  sExp[0]  = '\0';

  //There should only be 0 or 1 decimal point.
  //There can only be 2 sign signifiers if it's sci.
  //There can only be 1 sign signifier if it's eng.
  //Case Matters
  //if it has an e it can't have any other characters
  //the oomPrefix can imply a decimal point.
  //1. check for leading sign signifier.
  pDst = sMant;
  if(strlen(pSrc) == 0)
    return false;
  while((state != ESES_DONE) && (state != ESES_BAD)) {
    if(((*pSrc <= '9') && (*pSrc >= '0')) || (*pSrc == '-') || (*pSrc == '+')) {
      *(pDst++) = *(pSrc++);
      }
    else {
      if(state == ESES_EXP) {
        *pDst = '\0';
        state = ESES_DONE;
        }
      else {// we were in the mantissa, let's see what this non-number thing is.
        if((*pSrc == 'e') || (*pSrc == 'E')) {
          if(gotEngMark) {
            state = ESES_DONE;
            }
          else{
            state = ESES_EXP;
            sciEng = ESE_SCI;
            pSrc++;
            *pDst = '\0';
            pDst = sExp;
            }
          }
        else if(*pSrc == '.') {
          if(gotMantPeriod) {
            *pDst = '\0';
            state = ESES_DONE;
            }
          else  {
            *(pDst++) = *(pSrc++);
            gotMantPeriod = true;
            }
          }
        else if(ParseEngMark(*pSrc) < 99) {
          iExp = ParseEngMark(*pSrc);
          gotMantPeriod = true;
          gotEngMark = true;
          sciEng = ESE_ENG;
          *(pDst++) = '.';
          pSrc++;
          }
        else {
          state = ESES_DONE;
          }
        }
      }
    }
  *pDst = '\0';
  sscanf(sMant, "%lf", &dMant);
  if(sciEng == ESE_SCI)
    sscanf(sExp, "%d", &iExp);
  *o_f = dMant * pow(10.0, ((double)iExp));
  return state != ESES_BAD;
  }
int   ParseEngMark          (char i_c) {
  int engExp = -18;
  bool gotIt = false;
  char *pSrc = (char *)oomPrefix;

  while(!gotIt && (*pSrc != '\0')) {
    if(*(pSrc++) == i_c) {
      gotIt = true;
      }
    else {
      engExp += 3;
      }
    }
  if(!gotIt)
    engExp = 99;
  return engExp;
  }
void  RatioString           (char *o_tStr, double i_f) {
  if(i_f == 1.0) {
    sprintf(o_tStr, "1:1");
    }
  else if(i_f > 1.0) {
    sprintf(o_tStr, "%f", i_f);
    strcat(o_tStr, ":1");
    }
  else {
    char s[256];
    strcpy(o_tStr, "1:");
    sprintf(s, "%f", i_f);
    strcat(o_tStr, s);
    }
  return;
  }
void  FillJust              (char *io_tStr, ulong i_len) {
  if(strlen(io_tStr) >= i_len)
      return;
  char *pSrc;
  char *pDst;

  pSrc = &(io_tStr[strlen(io_tStr)]);
  pDst = &(io_tStr[i_len]);
  while(pSrc >= io_tStr)
    *(pDst--) = *(pSrc--);
  while(pDst >= io_tStr)
    *(pDst--) = ' ';
  return;
  }
bool  IsLLongDec            (char *i_s) {
  bool     result;
  char    *p;

  p      = i_s;
  StripLeadingNP(i_s);
  StripTrailingNP(i_s);
  if(*p == '-')
    p++;
  result = false;
  if((strlen(p) > 0) && (strlen(p) < 19)) {
    result = true;
    while(*p != '\0') {
      if((*p < '0') || (*p > '9')) {
        result = false;
        p = &(p[strlen(p)]);
        }
      p++;
      }
    }

  return result;
  }
bool  IsDoubleFixed         (char *i_s) {
  bool     result;
  char    *p;
  bool     foundDot;

  p      = i_s;
  StripLeadingNP(i_s);
  StripTrailingNP(i_s);
  if(*p == '-')
    p++;
  result = false;
  foundDot = false;
  if((strlen(p) > 0) && (strlen(p) < 36)) {
    result = true;
    while(*p != '\0') {
      if((*p == '.') && (!foundDot))
        foundDot = true;
      else if((*p < '0') || (*p > '9')) {
        result = false;
        p = &(p[strlen(p)]);
        }
      p++;
      }
    }

  return result;
  }

