
// $Id: CaesApodia.hpp 202 2022-11-19 21:53:43Z duncang $

//=================================================================================================
// Original File Name : Apodia.hpp
// Original Author    : duncang
// Creation Date      : Sep 2, 2012
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
//
// Description        :
//    The one and only
//
//=================================================================================================

#ifndef APODIA_HPP_
#define APODIA_HPP_
#include <caes/CaesMath.hpp>
#include <caes/CaesTypes.hpp>
#include <map>

class Apodia {
  public:
    enum eConst {MAX_WIN_SIZE = 256*1024};
    enum eShape {
      // B-Spline
      EAT_DIRICHLET         ,      // Rectangular
      EAT_BARTLETT          ,
      EAT_WELCH             ,
      EAT_PARZEN            ,
      // Tapered Cosines
      EAT_BARTLETT_HANN     ,
      EAT_TUKEY_HANNING     ,
      // Cosine
      EAT_HANN              ,           /// 0.50 - 0.50*cos(2.0*pi*n / (N-1))
      EAT_HAMMING           ,        /// 0.54 - 0.46*cos(2.0*pi*n / (N-1))
      EAT_NUTTALL           ,
      EAT_BLACKMAN          ,
      EAT_BLACKMAN_NUTTALL  ,
      EAT_BLACKMAN_HARRIS   ,
      EAT_FLAT_TOP_ISO      ,
      EAT_FLAT_TOP_2PT      ,
      EAT_FLAT_TOP_ALT4     ,
      EAT_FLAT_TOP_HP_P301  ,
      EAT_FLAT_TOP_HP4      ,
      EAT_FLAT_TOP_HP_P401  ,
      EAT_FLAT_TOP_RS4      ,
      EAT_FLAT_TOP_SR785    ,
      // Functions
      EAT_DOLPH             ,
      EAT_GAUSS             ,          /// e^(-0.5*(  ((n-(N-1)/2)  /  (sigma*(N-1)/2)  )^2))
      EAT_KAISER            ,    //Io(beta * sqrt(1 - (2x - 1)^2 )) / Io(beta)
      EAT_SLEPIAN           ,    // Eliptic http://www.dsprelated.com/dspbooks/sasp/Slepian_DPSS_Window.html
      EAT_POISSON           ,  //!     e^(-ax)
      EAT_HANN_POISSON      ,  //! 0.5*e^(-ax)*(1 + cos(x))
      // Polynomial + (sometimes) transcendentals
      EAT_CONNES            ,
      EAT_BOHMAN            ,
      EAT_LANCZOS           ,        ///             sinc(2.0*n/(N-1) - 1.0)
      EAT_NUM_SHAPES
    };
  private:
    struct sCosSpec {
      llong     nTerms;
      double    *terms;
    };
    enum eNorm{EN_PEAK, EN_RMS, EN_DC};
  private:
                           Apodia          ( void                );
    virtual               ~Apodia          ( void                );
  public:
    static   Apodia       *GetInstance     ( void                );
             double       *GetWindow       ( void                )    { return window; };
             void          SetShape        ( eShape  i_shape     );
             eShape        GetShape        ( void                )    { return shape;  };
             void          SetNormPeak     ( void                ) {norm = EN_PEAK; return;};
             void          SetNormRMS      ( void                ) {norm = EN_RMS;  return;};
             void          SetNormDC       ( void                ) {norm = EN_DC;   return;};
             void          GetShapeName    ( char   *o_shapeName );
             void          GetShapeName    ( eShape  i_shape,    char *o_shapeName);
             double        GetAlphaRange   ( void                );
             double        GetAlphaRange   ( eShape  i_shape     );
             void          SetAlpha        ( double  i_alpha     );
             double        GetAlpha        ( void                )    { return alpha;             }
             void          SetAnti         ( bool    i_isAnti    )    {  isAnti = i_isAnti;  }
             bool          GetAnti         ( void                )    { return isAnti;};
             void          SetN            ( llong   i_N         );
             llong         GetN            ( void                ) { return N;              };
             llong         GetNmax         ( void                ) { return MAX_WIN_SIZE;   };
             llong         GetCatalogSize  ( void                ) { return shapes.size();  };
             void          ListCatalog     ( char   *o_list      );
             void          DoIt            ( double *i_x,        double *o_y);
             void          DoIt            ( double *io_xy       );
             void          BuildWindow     ( void                );
  private:
             void          Antify          ( void                );
             void          BuildCosines    ( void                );
             void          BuildList       ( void                );

             double        FuncDirichlet   ( double  i_x         );
             double        FuncBartlett    ( double  i_x         );
             double        FuncParzen      ( double  i_x         );

             double        FuncBartlettHann( double  i_x         );
             double        FuncTukey       ( double  i_x         );

             double        FuncCos         ( double  i_x         );

             void          BuildDolph      ( void                    );
             double        FuncGauss       ( double  i_x         );
             double        FuncKaiser      ( double  i_x         );
             double        FuncPoisson     ( double  i_x         );
             double        FuncHannPoisson ( double  i_x         );

             double        FuncWelch       ( double  i_x         );
             double        FuncConnes      ( double  i_x         );
             double        FuncBohman      ( double  i_x         );
             double        FuncLanczos     ( double  i_x         );
             double        (*BuildFunc)    ( double  i_x         );  //!<How we put an arbitrary builder in place, just a thought for now.
  public:
  private:
    static std::map<eShape, char *>                     shapes;
    static std::map<eShape, char *>                     shapeTips;
    static std::map<eShape, double>                     alphaRange;
    static std::map<eShape, char>                       alphaChar;
    static std::map<eShape, double (Apodia::*)(double)> SpecFunc;
    static std::map<eShape, sCosSpec*>                  cosSpec;
           std::map<eShape, double>                     alphas;

           llong     N;
           eShape     shape;
           double     alpha;
           bool       isAnti;
           double    *window;
           bool       dirty;


           eNorm      norm;
           bool       resident;
           bool       isOdd;

    static Apodia    *apod;
  };

#endif /* APODIA_HPP_ */
