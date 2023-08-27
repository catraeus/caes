
// $Id: A $

//=================================================================================================
// Original File Name : CaesString.hpp
// Original Author    : catraeus
// Creation Date      : Apr 17, 2022
// Copyright          : Copyright © 2022 by Catraeus and Duncan Gray
//
// Description        :
/*
   String Functions that, yes, are all over standard libraries, but these aren't swiss army
     knives, they just work fast and are missing your favorite back road nicities.
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

#ifndef _CAEV_STRING_HPP_
#define _CAEV_STRING_HPP_
#include "CaesTypes.hpp"

enum eJust{EJ_LEFT, EJ_CENTER, EJ_RIGHT};

void    ToUpper             (char   *i_string                                                        );
void    ToLower             (char   *i_string                                                        );
void    ToUpperBlock        (char   *i_block,       ullong  i_n                                      );
void    ToLowerBlock        (char   *i_block,       ullong  i_n                                      );

void    StripNPTrail        (char   *io_s                                                            );
void    StripNPLead         (char   *io_s                                                            );
void    StripNPAll          (char   *io_s                                                            );
void    CompactAllNP        (char   *io_s                                                            );
char   *FindAnyCharList     (char   *i_s,           char   *i_f                                      );
void    PadSpaceLead        (char   *io_tStr,       ullong   i_len                                    );
void    PadSpaceTrail       (char   *io_tStr,       ullong   i_len                                    );

void    CleanupFileName     (char   *io_path                                                         );
void    PathStringSplit     (char   *i_path,        char ***o_dirs, bool *o_isRootLevel, ullong *o_depth);
bool    PathExtractFileName (char   *i_path,        char   *o_fileName                               );
bool    PathExtractExt      (char   *i_path,        char   *o_ext                                    );
bool    PathExtractBase     (char   *i_path,        char   *o_base                                   );
bool    PathExtractPath     (char   *i_path,        char   *o_path                                   );
bool    PathExtractLastDir  (char   *i_path,        char   *o_dir                                    );
void    PathNameCleanup     (char   *io_path                                                         );

void    SecToHMS            (char   *o_tStr,        double  i_t, bool i_padHours                     );
int     HMStoSec            (double *o_t,           char   *i_tStr                                   );

void    IntWithComma        (char   *o_tStr,        int     i_i                                      );
void    IntWithComma        (char   *o_tStr,        uint    i_i                                      );
void    IntWithComma        (char   *o_tStr,        llong   i_i                                      );
void    IntWithComma        (char   *o_tStr,        ullong  i_i                                      );
void    IntWithComma        (char   *o_tStr,        float   i_f                                      );
void    IntWithComma        (char   *o_tStr,        double  i_f                                      );
void    HexWith0x           (char   *o_tStr,        uint    i_i, llong i_sigFigs                     );
void    EngString           (char   *o_tStr,        double  i_f, llong i_sigFigs, char *i_units      );
bool    StringLLong         (char   *i_tStr,        llong  *o_ll                                     );
bool    StringEng           (char   *i_tStr,        double *o_f                                      );
void    RatioString         (char   *o_tStr,        double  i_f                                      );
void    RatioString         (char   *o_tStr,        double  i_f, llong digits                        );
void    ErrNoToString       (int     i_errno,       char   *o_string                                 );

double  Gridder125          (double i_range,        ullong  i_count,        bool i_minVmax           );

bool    IsLLongDec          (char   *i_s                                                             );
bool    IsDoubleFixed       (char   *i_s                                                             );

#endif
