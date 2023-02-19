
// $Id: AudioFiler.cpp 191 2020-06-30 02:50:19Z duncang $

//=================================================================================================
// Original File Name : FileWave.cpp
// Original Author    : duncang
// Creation Date      : March 17, 2002
// Copyright          : Copyright © 2002 - 2015 by Catraeus and Duncan Gray
//
// Description        :
/*
    Facilities to do file things.
    Is a manager, and a single file object.
    Is specific to audio world, it will tell you what kind of (audio) file is inside.

    Use Cases:
      A small file is read in completely to a stream image.
      A small file is written completely from a stream image.
      A large file is read in piece-meal.  String-of-pearl like.  and each pearl is discarded after the musician is done.
        An analyzer will do this to provide a summary analysis or shuttle through for views.
      A large file is randomly read, with an arbitrarily sized, located block.
        An analyzer will do this to shuttle through a viewport of it.  SpecAn, Scope, Song Looping.
      All of these are managed by the RiffMgr system.  We don't need residency, start/stop, number types etc.
    Attributes:
      File Control I/F -
        public:
          File Name
          File Size (in bytes)
          File Type (r, w, rw, b, t)
          File State (enum) GetState() {idle, open, closed, non-existant, isDir, inaccessible} some conflation
          Open (locking, no-locking)
          Close()
          Fetch (n bytes from p location)
          Overwrite (n bytes to p location)
          Append (n bytes)
          <callback>Done Working, so the AudioFile object has new information.
        private:
          fd, the ancient posix file descriptor number.
      DOESN't know what the type or organization of the data is.
    Functions:
      Is able to get a contiguous block of num bytes starting at offset and report if it is legal to do so.
    Relationships:
      This object only knows about the header of the audio file formats that it will try to work with.
      It doesn't have any more smarts that.
      Ultimately, the higher levels will need to have their various format types know how to handle files based on headers.

*/
//=================================================================================================



#include "AudioFiler.hpp"

//#include <malloc.h>
//#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <caes/CaesString.hpp>
#include <caes/CaesTypes.hpp>
//=================================================================================================
                    AudioFiler::AudioFiler         ( void        ) {
  BuildEnv();
  BuildSetup();
  Connect();
  Reset(EA_READ);
}
                    AudioFiler::~AudioFiler        ( void        ) {
  if( fileName != NULL ) {
    delete fileName;
    fileName = NULL;
  }
}

void                AudioFiler::BuildEnv           ( void        ) {
  ctAm  = CtlAudMsg::GetInstance();
  return;
}
void                AudioFiler::BuildSetup         ( void        ) {
  fileName        = new char[SIZE_FILE_NAME];
  fileSize        =       0 ;
  fd              =      -1 ;
  CbChangeFile    =    NULL ;
  return;
}
void                AudioFiler::Connect            ( void        ) {
  return;
}


void                AudioFiler::Reset              ( eAccess i_a  ) {

  if(fd >= 0)
    Close();
  fd         =    -1;
  accessType = i_a;

  status            = FST_BAD ;
  fileName[0]       =    '\0';
  fileSize          =      0 ;
  return;
}

AudioFiler::eStatus AudioFiler::SetFileName        ( char *i_fileName, eAccess i_a ) {
  int accessMode;
  int accessAnswer;

  accessType = i_a;

  Reset(i_a);
  //===============READ
  if(accessType == EA_READ) {
    accessMode = R_OK;
    accessAnswer = access(i_fileName, accessMode);
    if(accessAnswer == -1) {
      if(errno == ENOENT) {
        fprintf(stderr, "File doesn't exist ... OOPS\n"); fflush(stdout);
        status = FST_NOENT;
      }
      else {
        fprintf(stdout, "The requested file has problems because .%d. %s.\n", errno, strerror(errno));  fflush(stdout);
        status = FST_BAD;
        }
      }
    else {
      fprintf(stdout, "The requested file is just fine.\n");  fflush(stdout);
      strcpy(fileName, i_fileName);
      status = FST_IDLE;
    }
  }
  //===============WRITE
  else { // Write access politeness
    accessMode = F_OK;
    accessAnswer = access(i_fileName, accessMode);
    if(accessAnswer == -1) {
      if(errno == ENOENT) {
        fprintf(stderr, "File doesn't exist ... COOL\n");  fflush(stdout);
        strcpy(fileName, i_fileName);
        status = FST_NOENT;
      }
      else {
        fprintf(stdout, "The requested file has problems because .%d. %s.\n", errno, strerror(errno));  fflush(stdout);
        status = FST_BAD;
      }
    }
    else {
      accessMode = W_OK;
      accessAnswer = access(i_fileName, accessMode);
      if(accessAnswer == -1) {
        fprintf(stdout, "The requested file has problems because .%d. %s.\n", errno, strerror(errno));  fflush(stdout);
        status = FST_BAD;
        }
      else {
        fprintf(stdout, "File DOES EXIST ... ASK before deleting content for our own purposes!\n");  fflush(stdout);
        strcpy(fileName, i_fileName);
        status = FST_WRITEWARN;
      }
    }
  }
  return GetStatus();
}
void                AudioFiler::AcquireFileSize    ( void             ) {
  struct stat fs;

  fileSize = 0;
  fstat(fd, &fs);
  if(status == FST_BAD)
    return;
  fileSize = (ullong)fs.st_size;
  return;
}


//  ====  Hide the OS particulars from the workers above
void                AudioFiler::Open               ( void             ) {
  struct stat fs;

  if((status == FST_IDLE) || (status == FST_NOENT) || (status == FST_WRITEWARN)) {
    if(accessType == EA_READ) {
      fd = open((const char *)fileName, (int)O_RDONLY);
      if(fd < 0) { // check based on fd number.
        fprintf(stdout, "The input file <%s> has problems being opened: %s\n", fileName, strerror(errno));      fflush(stdout);
        status = FST_BAD;
        return;
        }
      else {
        fprintf(stdout, "The input file opened just fine\n" );      fflush(stdout);
        AcquireFileSize();
        status = FST_OPENRD;
        }

      fstat(fd, &fs);
      if(fs.st_mode & S_IFDIR) { // check if it's a directory
        fprintf(stdout, "Sorry, but directories can't be used for content.\n"); fflush(stdout);
        close(fd);
        fd = -1;
        status = FST_BAD;
        }
      }
    else {
      fd = open((const char *)fileName, (int)(O_CREAT | O_TRUNC | O_WRONLY), (int)0644);
      if(fd < 0) {
        fprintf(stdout, "The output file has problems being opened: %s\n", strerror(errno));      fflush(stdout);
        status = FST_BAD;
        return;
        }
      else {
        fprintf(stdout, "The output file opened just fine\n" );      fflush(stdout);
        status = FST_OPENWR;
        }

      fstat(fd, &fs);
      if(fs.st_mode & S_IFDIR) {
        fprintf(stdout, "Sorry, but directories can't be used for content.\n");      fflush(stdout);
        status = FST_BAD;
        close(fd);
        fd = -1;
        }
      }
  }
  else { // if(accessType == EA_READ) -- Bad enum, deep in c++ Guts we have a problem!
    fprintf(stdout, "unknown file status in AudioFiler::Open\n" );      fflush(stdout);
    status = FST_BAD;
  }
  if(CbChangeFile != 0)
    CbChangeFile->Execute(0);
  return;
  }
bool                AudioFiler::Fetch              ( byte *pImg, ullong i_N, ullong i_off) {
  bool   AOK;
  ullong readBytes;

  readBytes  = (llong)pread(fd, pImg, i_N, i_off);
  AOK = readBytes == i_N;
  return AOK;
}
void                AudioFiler::Pitch              ( byte *i_pImg, ullong i_N ) {
  size_t b;
  b = write(fd, (const void*)i_pImg, (size_t)i_N);
  if(b == 0) return;
  return;
}
void                AudioFiler::Close              ( void        ) {
  if(fd > 2) // MAGIC since an FD for stderr is 2
    close(fd);
  fd = -1;
  return;
}
