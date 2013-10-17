/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef WRAPPEDCHEBYCHEV3
#define WRAPPEDCHEBYCHEV3

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class WrappedChebychev3 : public RooAbsPdf {
public:
  WrappedChebychev3() {} ; 
  WrappedChebychev3(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _p0,
	      RooAbsReal& _p1,
	      RooAbsReal& _p2,
	      RooAbsReal& _p3,
	      RooAbsReal& _p4);
  WrappedChebychev3(const WrappedChebychev3& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new WrappedChebychev3(*this,newname); }
  inline virtual ~WrappedChebychev3() { }

protected:

  RooRealProxy x ;
  RooRealProxy p0 ;
  RooRealProxy p1 ;
  RooRealProxy p2 ;
  RooRealProxy p3 ;
  RooRealProxy p4 ;
  
  Double_t evaluate() const ;

private:

  ClassDef(WrappedChebychev3,1) // Your description goes here...
};
 
#endif
