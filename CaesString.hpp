
// $Id: CaesString.hpp 154 2018-08-14 11:24:57Z duncang $

#ifndef _CAEV_STRING_HPP_
#define _CAEV_STRING_HPP_
#include "CaesTypes.hpp"

enum eJust{EJ_LEFT, EJ_CENTER, EJ_RIGHT};

void ToUpper             (char   *i_string                                                        );
void ToLower             (char   *i_string                                                        );
void ToUpperBlock        (char   *i_block,       ullong  i_n                                      );
void ToLowerBlock        (char   *i_block,       ullong  i_n                                      );
void CleanupFileName     (char   *io_path                                                         );
void CleanupTrailingWhite(char   *io_str                                                          );
void PathStringSplit     (char   *i_path,        char ***o_dirs, bool *o_isRootLevel, ulong *o_depth);
bool PathExtractFileName (char   *i_path,        char   *o_fileName                               );
bool PathExtractExt      (char   *i_path,        char   *o_ext                                    );
bool PathExtractBase     (char   *i_path,        char   *o_base                                   );
bool PathExtractPath     (char   *i_path,        char   *o_path                                   );
bool PathExtractLastDir  (char   *i_path,        char   *o_dir                                    );
void PathNameCleanup     (char   *io_path                                                         );
void SecToHMS            (char   *o_tStr,        double  i_t, bool i_padHours                     );
int  HMStoSec            (double *o_t,           char   *i_tStr                                   );
void IntWithComma        (char   *o_tStr,        int     i_i                                      );
void IntWithComma        (char   *o_tStr,        uint    i_i                                      );
void IntWithComma        (char   *o_tStr,        llong   i_i                                      );
void IntWithComma        (char   *o_tStr,        ullong  i_i                                      );
void IntWithComma        (char   *o_tStr,        float   i_f                                      );
void IntWithComma        (char   *o_tStr,        double  i_f                                      );
void HexWith0x           (char   *o_tStr,        uint    i_i, llong i_places                      );
void EngString           (char   *o_tStr,        double  i_f, llong i_places, char *i_units       );
bool StringEng           (char   *i_tStr,        double *o_f                                      );
void RatioString         (char   *o_tStr,        double  i_f                                      );
void RatioString         (char   *o_tStr,        double  i_f, llong digits                        );
void ErrNoToString       (int     i_errno,       char   *o_string                                 );
void StripTrailingNP     (char   *io_s                                                            );
void StripLeadingNP      (char   *io_s                                                            );
char *FindAnyCharList    (char   *i_s,           char   *i_f                                      );
void StripAllNP          (char   *io_s                                                            );
void CompactAllNP        (char   *io_s                                                            );
void FillJust            (char   *io_tStr,       ulong   i_len                                    );
bool IsLLongDec          (char   *i_s                                                             );
bool IsDoubleFixed       (char   *i_s                                                             );
#endif
