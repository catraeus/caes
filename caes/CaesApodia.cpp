
// $Id: CaesApodia.cpp 201 2022-11-19 03:18:24Z duncang $

//=================================================================================================
// Original File Name : Apodia.cpp
// Original Author    : duncang
// Creation Date      : Sep 2, 2012
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//    The one and only
//
//=================================================================================================

#include <string.h>
#include <math.h>
#include <gsl/gsl_sf.h>

#include <caes/CaesApodia.hpp>
#include <caes/CaesFunc.hpp>
#include <caes/CaesMath.hpp>

Apodia *Apodia::apod=NULL;

std::map<Apodia::eShape, char *>                       Apodia::shapes;
std::map<Apodia::eShape, double>                       Apodia::alphaRange;
std::map<Apodia::eShape, char>                         Apodia::alphaChar;
std::map<Apodia::eShape, double (Apodia::*)(double)>   Apodia::SpecFunc;
std::map<Apodia::eShape, Apodia::sCosSpec*>            Apodia::cosSpec;

                      Apodia::Apodia() {
  window      = new double[MAX_WIN_SIZE];;
  N           = 1;
  norm        = EN_DC;
  resident    = false;
  isOdd       = true;
  BuildList();
  BuildCosines();
  SetAnti(false);
  SetShape(Apodia::EAT_DIRICHLET);
  }
                      Apodia::~Apodia()                         {
  }
Apodia               *Apodia::GetInstance(void) {
  if(apod == NULL) apod = new Apodia();
  return apod;
}

void                  Apodia::SetShape(eShape i_shape)      {
  shape = i_shape;
  alpha = alphas[shape];
  SetAlpha(alpha);
  return;
  }
void                  Apodia::GetShapeName(char *o_shapeName)   {
  strcpy(o_shapeName, shapes[shape]);
  return;
  }
void                  Apodia::GetShapeName(eShape i_shape, char *o_shapeName) {
  strcpy(o_shapeName, shapes[i_shape]);
  return;
  }
double                Apodia::GetAlphaRange(void)               { return alphaRange[shape]; }
double                Apodia::GetAlphaRange(eShape i_shape) { return alphaRange[i_shape];    }
void                  Apodia::SetAlpha(double i_alpha) {
  if(i_alpha > alphaRange[shape])
    alpha = alphaRange[shape];
  else if(i_alpha < 0.0)
    alpha = 0.0;
  else
    alpha = i_alpha;
  alphas[shape] = alpha;
  return;
  }
void                  Apodia::SetN(llong i_N)                   {
  if(i_N    == N   )              return;
  if(i_N <=0       )        i_N = 0;
  if(i_N > GetNmax())    i_N = GetNmax();
  N = i_N;
  if(N == 0) return;
  isOdd = (N % (llong)2) == 1;
  return;
  }

void                  Apodia::ListCatalog(char *o_list)         {
  o_list[0] = '\0';
  for(uint i=0; i<shapes.size(); i++) {
    strcat(o_list, shapes[(Apodia::eShape)i]);
    strcat(o_list, "\n");
    }
  return;
  }


void                  Apodia::DoIt(double *i_x, double *o_y)    {
  if(shape == EAT_DIRICHLET) {
    for(llong i=0; i<N; i++)
      o_y[i] = i_x[i];
    }
  else if(N <= 2) {
    for(llong i=0; i<N; i++)
      o_y[i] = i_x[i];
    }
  else {
    for(llong i=0; i<N; i++)
      o_y[i] = i_x[i] * window[i];
    }
  return;
  }
void                  Apodia::DoIt(double *io_xy)               {
  if(shape == EAT_DIRICHLET)
    return;
  else if(N <= 2)
    return;
  else {
    for(llong i=0; i<N; i++)
      io_xy[i] *= window[i];
    }
  return;
  }
void                  Apodia::BuildWindow(void)                 {
  double factor;
  llong flipper;
  double offset;
  double x;
  double y;
  llong  i; // needs to be signed to subtract around the middle.

  if(N < 2) return;
  if(shape == EAT_DOLPH)
    BuildDolph();
  else {
    /*
      Window is a function from 1/2 sample either side of end points of space to middle.
      symmetry guys! <==> y[N - i - 1] = y[i] -- regardless of odd or even
      x is a space in ideal real-domain space from -1.0 to +1.0.
      i is a space in sampled integer domain from 0 to N - 1
      y is the amplitude value at x or i in either space.

      ===== 1 - find the middle in i-space
      I will pretend that the i-space is real instead of integer.
      For a simple case, N = 3, the middle is i = 1.0, which looks like i = (N - 1) / 2
      For a simple case, N = 4, the middle is i = 1.5, which looks like i = (N - 1) / 2
       -- surprise, surprise, it's the same.

      ===== 2 - map i to x
      So the  0.0 ... -1.0 span of the window half-function x-space goes from ((N - 1) / 2) to (   -1/2) in i-space
      And the 0.0 ... +1.0 span of the window half-function x-space goes from ((N - 1) / 2) to (N - 1/2) in i-Space.

      I'll do the first:
      odd1 i = (N - 1) / 2;                      -- Just place the single middle one
      odd2 i = (N - 3) / 2;       i >= 0; i--;   -- Now iterate the rest of the way
      even i =  N      / 2 - 1;   i >= 0; i--;   -- Even doesn't need to treat the middle specially, it never actually hits it.

      ===== 3 Flip the i-domain to another integer.  This is just some flipper - i function.  So what is the value of Flipper?
      This is the integer that gets us from the left-is-zero of the i domain to center-is-zero of the x domain.
      odd,  flipper = (N - 1) / 2
      even, flipper =  N      / 2 - 1
      This puts the flipped integer as zero for the odd ones and 1 for the even ones.

      but this makes an offset in the x domain that is one half of an i for even and 0.0 for odd.

      x = offset + (flipper - i) / ( (N - 1) / 2  + 0.5) ==> oh yeah, look, that's just ==> offset + (flipper - i) * 2 / N;
      I'm gonna rename ( 2 / N ) to call it factor.
      and i_alt = N - i - 1

    */
    factor = 2.0 / (double)(N);
    if(N == 1) {
      window[0] = 1.0;
      return;
      }
    if(isOdd) { // do the very special exact middle one that doesn't get mirrored.
      flipper = (N - 1) / 2;
      offset  = 0.0;
      i = (N - 1) / 2;
      x = offset + (double)(flipper - i) * factor;
      if(SpecFunc[shape] != 0)
        y = (this->*SpecFunc[shape])(x);
      else
        y = 1.0;
      window[i] = y;
      i--;
      }
    else {
      flipper = N / 2 - 1;
      offset = 1.0 / N;
      i = N / 2 - 1;
      }
    while(i>=0) {
      x = offset + (double)(flipper - i) * factor;
      if(SpecFunc[shape] != 0)
        y = (this->*SpecFunc[shape])(x);
      else
        y = 1.0;
      window[i] = y;
      window[N - i - 1] = y;
      i--;
      }
    }
  //DO NOT forget to normalize.  I need to do a big study of frequency response too.
  // I'm reusing factor.
  if(norm == EN_DC) {
    factor = 0.0;
    for(i=0; i<(llong)N; i++ )
      factor += window[i];
    factor /= (double)N;
  }
  else if(norm == EN_RMS) {
    factor = 0.0;
    for(i=0; i<(llong)N; i++ )
      factor += window[i] * window[i]; // squares
    factor /= (double)N;    // mean of the
    factor = sqrt(factor);  // Root of the
  }
  else { // EN_PEAK
    factor = 0.0;
    for(i=0; i<(llong)N; i++ ) {
      if(factor < fabs(window[i]))
        factor = fabs(window[i]);
      }
    }
  factor = 1.0 / factor;
  for(i=0; i<(llong)N; i++ )
    window[i] *= factor;
  if(isAnti)
    Antify();
  return;
  }
void                  Apodia::Antify(void) {
  llong middle = N >> 1;
  double sum;

  window[middle] = 0.0;
  if(!isOdd)
    window[middle - 1] = 0.0;
  sum = 0.0;
  for(llong ii=0; ii<N; ii++) {
    sum += window[ii];
    window[ii] *= -1.0;
    }
  if(isOdd)
    window[middle    ] = sum;
  else {
    window[middle    ] = sum * 0.5;
    window[middle - 1] = sum * 0.5;
    }
  return;
  }
void                  Apodia::BuildCosines(void) {
  sCosSpec *tcsNull;
  sCosSpec *tcs;
  llong     N;
  double   *t;
  tcsNull         = new(sCosSpec);
  tcsNull->nTerms = 0;
  tcsNull->terms  = NULL;

                                                                               cosSpec[EAT_DIRICHLET] = tcsNull;
                                                                               cosSpec[EAT_BARTLETT] = tcsNull;
                                                                               cosSpec[EAT_PARZEN] = tcsNull;

                                                                               cosSpec[EAT_BARTLETT_HANN] = tcsNull;
  //===   Hann                               ======  http://reference.wolfram.com/mathematica/ref/HannWindow.html     =======  DADiSP call it Hanning, but has same coefficients  ==
  N = 2;  t = new double[N];  t[0] = 0.50000000;           t[1] = 0.50000000;        //t[2] = 0.00000000;          t[3] = 0.00000000;            t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_HANN] = tcs;
  //===   Hamming                            ======  http://reference.wolfram.com/mathematica/ref/HammingWindow.html  =======  but DADiSP sez 0.54 and 0.46  =======================
  N = 2;  t = new double[N];  t[0] = 25.0 / 46.0;          t[1] = 21.0 / 46.0;       //t[2] = 0.00000000;          t[3] = 0.00000000;            t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_HAMMING] = tcs;
  //===   Nuttall                            ======  http://reference.wolfram.com/mathematica/ref/NuttallWindow.html  ==============================================================
  N = 4;  t = new double[N];  t[0] =  88942.0 / 250000.0;  t[1] = 121849.0 / 250000.0; t[2] =  36058.0 / 250000.0; t[3] =   3151.0 / 250000.0; //t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_NUTTALL] = tcs;
  //===   Blackman                           ======  http://reference.wolfram.com/mathematica/ref/BlackmanWindow.html  =============================================================
  N = 3;  t = new double[N];  t[0] = 21.0 / 50.0;          t[1] = 25.0 / 50.0;         t[2] =  4.0 / 50.0;       //t[3] = 0.00000000;            t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_BLACKMAN] = tcs;
  //===   Blackman Nuttall                   ======  http://reference.wolfram.com/mathematica/ref/BlackmanNuttallWindow.html  ======================================================
  N = 4;  t = new double[N];  t[0] = 0.36358190;           t[1] = 0.48917750;          t[2] = 0.13659950;          t[3] = 0.01064110;          //t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_BLACKMAN_NUTTALL] = tcs;
  //===   Blackman Harris                    ======  http://reference.wolfram.com/mathematica/ref/BlackmanHarrisWindow.html, DADiSP  ===============================================
  N = 4;  t = new double[N];  t[0] = 0.35875000;           t[1] = 0.48829000;          t[2] = 0.14128000;          t[3] = 0.01168000;          //t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_BLACKMAN_HARRIS] = tcs;
  //===   Flat-top ISO                       ======  Wikipedia  ====================================================================================================================
  N = 5;  t = new double[N];  t[0] = 1.00000000;           t[1] = 1.93300000;          t[2] = 1.28600000;          t[3] = 0.38800000;            t[4] = 0.03220000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_ISO] = tcs;
  //===Flat-top 2-point                      ======  DADiSP  =======================================================================================================================
  N = 3;  t = new double[N];  t[0] = 0.28106390;           t[1] = 0.52089720;          t[2] = 0.19803990;        //t[3] = 0.00000000;          //t[4] = 0.00000000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_2PT] = tcs;
  //===   Flat-top 4-point alternate         ======  Wolfram  ======================================================================================================================
  N = 5;  t = new double[N];  t[0] = 0.215578947;          t[1] = 0.416631580;         t[2] = 0.277263158;         t[3] = 0.083578947;           t[4] = 0.006947368;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_ALT4] = tcs;
  //===   Flat-top hp P301                   ======  DADiSP  =======================================================================================================================
  N = 4;  t = new double[N];  t[0] = 0.999448600;          t[1] = 2.0 * 0.955728;      t[2] = 2.0 * 0.538289;      t[3] = 2.0 * 0.091581;      //t[4] = 0.006947368;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_HP_P301] = tcs;
  //===   Flat-top hp 4-point                ======  DADiSP  =======================================================================================================================
  N = 5;  t = new double[N];  t[0] = 1.000000000;          t[1] = 2.0 * 0.934516;      t[2] = 2.0 * 0.597986;      t[3] = 2.0 * 0.017964;        t[4] = 2.0 * 0.015458;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_HP4] = tcs;
  //===   Flat-top hp P401                   ======  DADiSP  =======================================================================================================================
  N = 6;  t = new double[N];  t[0] = 1.00000000000000;     t[1] = 1.93774046310203;    t[2] = 1.32530734987255;    t[3] = 0.43206975880342;      t[4] = 0.04359135856900;    t[5] = 0.00015175580171;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_HP_P401] = tcs;
  //===   Flat-top Rohde & Schwarz 4-point   ======  DADiSP  =======================================================================================================================
  N = 5;  t = new double[N];  t[0] = 0.1881999;            t[1] = 0.36923;             t[2] = 0.28702;             t[3] = 0.13077;               t[4] = 0.02488;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_RS4] = tcs;
  //===   Flat-top Stanford Research RS785   ======  Wikipedia  ====================================================================================================================
  N = 5;  t = new double[N];  t[0] = 1.00000000;           t[1] = 1.93000000;          t[2] = 1.29000000;          t[3] = 0.38800000;            t[4] = 0.02800000;
  tcs = new(sCosSpec);  tcs->nTerms = N; tcs->terms = t; cosSpec[EAT_FLAT_TOP_SR785] = tcs;

  cosSpec[EAT_HANN_POISSON] = cosSpec[EAT_HANN];
  return;
  }
void                  Apodia::BuildList(void)                   {
  if(shapes.size() == 0) {
    char       *s;
    char        t[256];
    eShape      z;
    strcpy(t, "Dirichlet");        s = new char[sizeof(t)]; strcpy(s, t); z = EAT_DIRICHLET;        shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncDirichlet;     alphas[z] = 0.0;
    strcpy(t, "Bartlett");         s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BARTLETT;         shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncBartlett;      alphas[z] = 0.0;
    strcpy(t, "Welch");            s = new char[sizeof(t)]; strcpy(s, t); z = EAT_WELCH;            shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncWelch;         alphas[z] = 0.0;
    strcpy(t, "Parzen");           s = new char[sizeof(t)]; strcpy(s, t); z = EAT_PARZEN;           shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncParzen;        alphas[z] = 0.0;
    // Cosine Combo
    strcpy(t, "Bartlett-Hann");    s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BARTLETT_HANN;    shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncBartlettHann;  alphas[z] = 0.0;
    strcpy(t, "Tukey-Hanning");    s = new char[sizeof(t)]; strcpy(s, t); z = EAT_TUKEY_HANNING;    shapes[z] = s; alphaRange[z] =  0.99999; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncTukey;         alphas[z] = 0.5;
    // Cosine
    strcpy(t, "Hann");             s = new char[sizeof(t)]; strcpy(s, t); z = EAT_HANN;             shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Hamming");          s = new char[sizeof(t)]; strcpy(s, t); z = EAT_HAMMING;          shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Nuttall");          s = new char[sizeof(t)]; strcpy(s, t); z = EAT_NUTTALL;          shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Blackman");         s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BLACKMAN;         shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Blackman-Nuttall"); s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BLACKMAN_NUTTALL; shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Blackman-Harris");  s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BLACKMAN_HARRIS;  shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top ISO");     s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_ISO;     shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top 2-pt");    s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_2PT;     shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top alt 4-pt");s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_ALT4;    shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top hp P301"); s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_HP_P301; shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top hp 4-pt"); s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_HP4;     shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top hp P401"); s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_HP_P401; shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top R&S 4-pt");s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_RS4;     shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    strcpy(t, "Flat-Top SR785");   s = new char[sizeof(t)]; strcpy(s, t); z = EAT_FLAT_TOP_SR785;   shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncCos;           alphas[z] = 0.0;
    // Functions
    strcpy(t, "Dolph");            s = new char[sizeof(t)]; strcpy(s, t); z = EAT_DOLPH;            shapes[z] = s; alphaRange[z] =  1.0; alphaChar[z] = 'a';                                           alphas[z] = 0.01;
    strcpy(t, "Gauss");            s = new char[sizeof(t)]; strcpy(s, t); z = EAT_GAUSS;            shapes[z] = s; alphaRange[z] = 20.0; alphaChar[z] = 'b'; SpecFunc[z] = &Apodia::FuncGauss;         alphas[z] = 2.0;
    strcpy(t, "Kaiser");           s = new char[sizeof(t)]; strcpy(s, t); z = EAT_KAISER;           shapes[z] = s; alphaRange[z] = 20.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncKaiser;        alphas[z] = 5.0;
    strcpy(t, "Slepian");          s = new char[sizeof(t)]; strcpy(s, t); z = EAT_SLEPIAN;          shapes[z] = s; alphaRange[z] = 20.0; alphaChar[z] = 'a';                                           alphas[z] = 1.0;
    strcpy(t, "Poisson");          s = new char[sizeof(t)]; strcpy(s, t); z = EAT_POISSON;          shapes[z] = s; alphaRange[z] = 20.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncPoisson;       alphas[z] = 3.0;
    strcpy(t, "Hann-Poisson");     s = new char[sizeof(t)]; strcpy(s, t); z = EAT_HANN_POISSON;     shapes[z] = s; alphaRange[z] = 20.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncHannPoisson;   alphas[z] = 3.0;
    // Polynomial + (sometimes) transcendentals.
    strcpy(t, "Connes");           s = new char[sizeof(t)]; strcpy(s, t); z = EAT_CONNES;           shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncConnes;        alphas[z] = 0.0;
    strcpy(t, "Bohman");           s = new char[sizeof(t)]; strcpy(s, t); z = EAT_BOHMAN;           shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncBohman;        alphas[z] = 0.0;
    strcpy(t, "Lanczos");          s = new char[sizeof(t)]; strcpy(s, t); z = EAT_LANCZOS;          shapes[z] = s; alphaRange[z] =  0.0; alphaChar[z] = 'a'; SpecFunc[z] = &Apodia::FuncLanczos;       alphas[z] = 0.0;
    }
  return;
  }

//================================================================
// B-Splines
double                Apodia::FuncDirichlet(double i_x) {
  return 1.0;
  }
double                Apodia::FuncBartlett(double i_x) {
  return 1.0 - i_x;
  //http://reference.wolfram.com/mathematica/ref/BartlettWindow.html
  }
double                Apodia::FuncParzen(double i_x) {
  if(i_x < 0)
    i_x *= -1.0;
  if(i_x < 0.5)
    return 1.0 - 6.0 *i_x*i_x + 6.0 * i_x*i_x*i_x;
  else if(i_x < 1.0)
    return 2.0 * (1.0 - i_x)*(1.0 - i_x)*(1.0 - i_x);
  else
    return 0.0;
  return 0.0;
  }

// so-called tapered cosines
double                Apodia::FuncBartlettHann(double i_x) {
  double dd;
  dd = 31.0 / 50.0 - 12.0 / 50.0 * i_x + 19.0 / 50.0 * cos(PI * i_x);
  return dd;
  }
double                Apodia::FuncTukey(double i_x) {
  double dd;
  if(i_x < alpha)
    dd = 1.0;
  else {
    dd = (cos((i_x - alpha)* PI / (1.0 - alpha)) + 1.0) * 0.5;
    }
  return dd;
  }
// Polynomial (sometimes) with transcendentals etc.
double                Apodia::FuncWelch(double i_x) {
  return 1.0 - i_x * i_x;
  }
double                Apodia::FuncConnes(double i_x) {
  double dd;
  return (1.0 - i_x * i_x ) * (1.0 - i_x * i_x );
  return dd;
  }
double                Apodia::FuncBohman(double i_x) {
  double dd;
  dd = (1.0 - i_x) * cos(PI * i_x) + sin(PI * i_x) / PI;
  return dd;
  }
double                Apodia::FuncLanczos(double i_x) {
  double dd;
  if(i_x < DBL_EPS)
    dd = 1.0;
  else
    dd = sin(i_x * PI) / i_x / PI;
  return dd;
  }

//================================================================
// Cosines
double                Apodia::FuncCos(double i_x) {
  double funcCos;
  sCosSpec *cs;
  cs = cosSpec[shape];
  funcCos = 0.0;
  for(llong i=0; i<cs->nTerms; i++) {
    funcCos += cs->terms[i] * cos((double)i * PI * i_x);
    }
  return funcCos;
  }

// Trans-Transcendentals.
void                  Apodia::BuildDolph(void) {

  llong  lN;
  double  r;
  double  Zo;
  llong  i;
  llong  n;
  llong  LIMIT;
  llong  MIDDLE;
  double *wDC;

  r          = alpha;
  lN         = N;
  wDC        = window;

  for(n=0; n<lN; n++)
    wDC[n]=0;

  Zo  = acosh(1.0 / r);
  Zo *= 1.0 / ((double)(lN - 1));
  Zo  = cosh(Zo);

  MIDDLE = lN >> 1; // mid point if odd, beginning of upper half if even AND the number of points that aren't the middle
  LIMIT = MIDDLE;
  if(!isOdd)
    LIMIT--;

  double KAC;
  double KTK;
  KAC  = 2.0 * PI;
  KAC /= (double)(lN);
  KTK  = PI / (double)lN;
  for(n=0; n<=LIMIT; n++) {
    double wDCn;
    wDCn = 1.0 / r;
    for(i=1; i<MIDDLE; i++) {
      double ArgCos;
      double ArgTk;
      ArgCos  = (double)(i * n);
      ArgCos *= KAC;

      ArgTk = (double)i * KTK;
      ArgTk = Zo * cos(ArgTk);
      wDCn += 2.0 * Tk((lN - 1), ArgTk) * cos(ArgCos);
      }
    wDCn *= 2.0 / (double)lN;
    if(isOdd) {
      wDC[MIDDLE - n] = wDCn; // double-hits on center  but so what?
      wDC[MIDDLE + n] = wDCn;
      }
    else {
      wDC[MIDDLE - n - 1] = wDCn;  // seems like limit should be one less if even and add a variable called middle.
      wDC[MIDDLE + n    ] = wDCn;
      }
    }
  return;
  }
double                Apodia::FuncGauss(double i_x) {
  double dd;
  dd = exp(-alpha * (i_x * i_x));
  return dd;
  }
double                Apodia::FuncKaiser(double i_x) {
  double dd;
  dd = gsl_sf_bessel_I0(alpha * sqrt(1.0 - i_x*i_x)) / gsl_sf_bessel_I0(alpha);
  return dd;
  }
double                Apodia::FuncPoisson(double i_x) {
  double dd;
  dd = exp(-alpha * i_x);
  return dd;
  }
double                Apodia::FuncHannPoisson(double i_x) {
  double dd;
  dd = FuncPoisson(i_x) * FuncCos(i_x);
  return dd;
  }
