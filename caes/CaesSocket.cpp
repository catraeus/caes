
// $Id: CaesSocket.cpp 162 2019-04-26 22:33:30Z duncang $

//=================================================================================================
// Original File Name : SockTop.cpp
// Original Author    : duncang
// Creation Date      : 2015-05-25
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
// Description        :
//    A class that is intended to derive to a client and a server.
//
//    Found the starting point in http://www.tldp.org/LDP/LG/issue74/tougher.html
//      in 2002 for crying out loud and it still works.
//      Thank you Rob Tougher
//
//=================================================================================================

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "CaesSocket.hpp"

SockTop::SockTop()
: FD(-1)
, isAOK(false) {
  memset(&theAddr,     0,     sizeof(theAddr));
  msgSend = new char[32768];
  msgRecv = new char[32768];
  }
SockTop::~SockTop() {
  if(IsValid())
    ::close(FD);
  }
bool SockTop::Socket() {
  //fprintf(stdout, "SockTop::Create()\n"); fflush(stdout);
  FD = socket(AF_INET, SOCK_STREAM, 0); // domain=internet, type=connection-oriented, protocol= system chooses
  if(!IsValid()) {
    fprintf(stdout, "Could not create socket.\n"); fflush(stdout);
    isAOK = false;
    return false;
    }

  int optName = SO_REUSEADDR; // actually is emun, so this is a chooser, not a field.
  int optVal  = 1;            // actually is bool, so this turns "reuse-address" on.
  int optRtn;
  optRtn = setsockopt(FD, SOL_SOCKET, optName, &optVal, sizeof(optVal));
  if(optRtn == -1) {
    fprintf(stdout, "Could not set reuse-address after creating server socket.\n"); fflush(stdout);
    isAOK = false;
    return false;
    }
  return true;
  }
bool SockTop::Send(const char *i_msg) {
  //fprintf(stdout, "SockTop::Send(%s)\n", i_msg); fflush(stdout);
  int numBytes;

  strcpy(msgSend, i_msg);
  numBytes = send(FD, msgSend, strlen(msgSend) + 1, MSG_NOSIGNAL);
  //fprintf(stdout, "SockTop::Send(%s) numBytes was: %d\n", msgSend, numBytes); fflush(stdout);
  if(numBytes == -1)
    return false;
  return true;
  }
int  SockTop::Recv(char *o_msg) const {
  //fprintf(stdout, "SockTop::Recv()\n"); fflush(stdout);
  int numBytes;

  o_msg[0] = '\0';

  numBytes = recv(FD, o_msg, MAX_RECV, 0);
  if(numBytes == -1) {
    fprintf(stdout, "numBytes = -1   errno == %d : %s in SockTop::Recv()\n", errno, strerror(errno));
    return 0;
    }
  else if(numBytes == 0) {
    fprintf(stdout, "numBytes =  0   errno == %d : %s in SockTop::Recv()\n", errno, strerror(errno));
    return 0;
    }
  //fprintf(stdout, "    SockTop::Recv(%s)\n", o_msg);
  return numBytes;
  }
void SockTop::SetNonBlock(const bool b) {
  //fprintf(stdout, "SockTop::SetNonBlock()\n"); fflush(stdout);
  int opts;
  opts = fcntl(FD, F_GETFL);
  if(opts < 0)
    return;
  if(b)
    opts =(opts |  O_NONBLOCK);
  else
    opts =(opts & ~O_NONBLOCK);
  fcntl(FD, F_SETFL, opts);
  }
