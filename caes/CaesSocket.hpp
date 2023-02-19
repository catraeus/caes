
// $Id: CaesSocket.hpp 162 2019-04-26 22:33:30Z duncang $

//=================================================================================================
// Original File Name : SockTop.hpp
// Original Author    : duncang
// Creation Date      : 2015-05-25
// Copyright          : Copyright © 2015 by Catraeus and Duncan Gray
//
// Description        :
//    The top level socket class.  From which servers and clients are derived.
//
//    Found the starting point in http://www.tldp.org/LDP/LG/issue74/tougher.html
//      in 2002 for crying out loud and it still works.
//      Thank you Rob Tougher
//
//=================================================================================================

#ifndef __SOCK_TOP_HPP_
#define __SOCK_TOP_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

class SockTop {
  protected:
    enum eConst {
      MAX_HOST_NAME  =   200,
      MAX_CONN       =     5,
      MAX_RECV       =   500,
      MAX_LINE       = 32768
      };
  public:
                    SockTop();
    virtual        ~SockTop();
  private:
  public:

    // Socket initialization
  protected:
    bool Socket      (                  )      ; // client.private server.private
  public:

    // Data Transimission
    bool Send        (const char *i_msg)      ; // client.public
    int  Recv        (      char *o_msg) const; // client.public

    // How to live life
    void SetNonBlock (const bool i_nonBlock                       );

    // Common Status
    bool IsValid     (void) const {return FD != -1;}
    bool IsAOK       (void) const {return isAOK;}; // client.public

  protected:
    char        *msgSend;
    char        *msgRecv;
    int          FD;
    int          portNo;
    sockaddr_in  theAddr;
    bool         isAOK;
  private:
  };


#endif // __SOCK_TOP_HPP_
