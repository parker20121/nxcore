#ifndef NXCORE_API_CLASS_H
#define NXCORE_API_CLASS_H

// It is not necessary to use this file to write to the NxCore API. It contains a wrapper class that makes it easier
// for some C++ developers. The only include file necessary to interface to NxCore API is NxCoreAPI.h

#include "NxCoreAPI.h"

////////////////////////////////////////////////////
////// NxCoreProcAddr<T> utility function	////////
////////////////////////////////////////////////////
template <class T> T NxCoreProcAddr(HMODULE hLib,char *pfName)
{
  return (T) ::GetProcAddress(hLib,pfName);
}

////////////////////////////////////////////////////
////// NxCoreClass: Wrapper for NxCoreAPI.dll //////
////////////////////////////////////////////////////
struct NxCoreClass	{
private:
  HMODULE						  hLib;
  NxCoreAPIVersion				  pfAPIVersion;
  NxCoreProcessTape				  pfProcessTape;
  NxCoreListTapes				  pfListTapes;
  NxCoreListAPIDLLs				  pfListAPIDLLs;
  NxCoreSpinSymbols				  pfSpinSymbols;
  NxCoreSaveState				  pfSaveState;
  NxCoreGetStateData			  pfGetStateData;
  NxCoreStateGetExgQuotes		  pfStateGetExgQuotes;
  NxCoreStateGetMMQuotes		  pfStateGetMMQuotes;
  NxCoreStateGetLastTrade		  pfStateGetLastTrade;
  NxCorePriceConvert			  pfPriceConvert;
  NxCorePriceFormat				  pfPriceFormat;
  NxCorePriceToDouble			  pfPriceToDouble;
  NxCoreDateFromNDays			  pfDateFromNDays;
  NxCoreDateFromYMD				  pfDateFromYMD;
  NxCoreGetDefinedString		  pfGetDefinedString;
  NxCoreGetTapeName				  pfGetTapeName;
  NxCoreSignalWaitEvent			  pfSignalWaitEvent;
public:
  NxCoreClass()
  {
	memset(this,0,sizeof(*this));
  }

 ~NxCoreClass()
  {
	UnloadNxCore();
  }

 // must call LoadNxCore before calling NxCoreAPI functions.
 // LoadNxCore version of NxCoreAPI if successful, 0 on failure
  unsigned int LoadNxCore(const char* cszNxCoreLib)
  {
	UnloadNxCore();
	hLib = ::LoadLibrary(cszNxCoreLib);
	if( hLib ) {
	  if((pfAPIVersion		  = NxCoreProcAddr<NxCoreAPIVersion>		(hLib,cszNxCoreAPIVersion))			&&
		 (pfProcessTape		  = NxCoreProcAddr<NxCoreProcessTape>		(hLib,cszNxCoreProcessTape))		&&
		 (pfListTapes		  = NxCoreProcAddr<NxCoreListTapes>			(hLib,cszNxCoreListTapes))			&&
		 (pfListAPIDLLs		  = NxCoreProcAddr<NxCoreListAPIDLLs>		(hLib,cszNxCoreListAPIDLLs))		&&
		 (pfSpinSymbols		  = NxCoreProcAddr<NxCoreSpinSymbols>		(hLib,cszNxCoreSpinSymbols))		&&
		 (pfSaveState		  = NxCoreProcAddr<NxCoreSaveState>			(hLib,cszNxCoreSaveState))			&&
		 (pfStateGetExgQuotes = NxCoreProcAddr<NxCoreStateGetExgQuotes> (hLib,cszNxCoreStateGetExgQuotes))	&&
		 (pfStateGetMMQuotes  = NxCoreProcAddr<NxCoreStateGetMMQuotes>  (hLib,cszNxCoreStateGetMMQuotes))	&&
		 (pfStateGetLastTrade = NxCoreProcAddr<NxCoreStateGetLastTrade> (hLib,cszNxCoreStateGetLastTrade))	&&
		 (pfPriceConvert	  = NxCoreProcAddr<NxCorePriceConvert>		(hLib,cszNxCorePriceConvert))		&&
		 (pfPriceFormat		  = NxCoreProcAddr<NxCorePriceFormat>		(hLib,cszNxCorePriceFormat))		&&
		 (pfPriceToDouble	  = NxCoreProcAddr<NxCorePriceToDouble>		(hLib,cszNxCorePriceToDouble))		&&
		 (pfDateFromNDays	  = NxCoreProcAddr<NxCoreDateFromNDays>		(hLib,cszNxCoreDateFromNDays))		&&
		 (pfDateFromYMD		  = NxCoreProcAddr<NxCoreDateFromYMD>		(hLib,cszNxCoreDateFromYMD))		&&
		 (pfGetDefinedString  = NxCoreProcAddr<NxCoreGetDefinedString>  (hLib,cszNxCoreGetDefinedString))	&&
  		 (pfGetTapeName		  = NxCoreProcAddr<NxCoreGetTapeName>		(hLib,cszNxCoreGetTapeName))		&&
		 (pfSignalWaitEvent	  = NxCoreProcAddr<NxCoreSignalWaitEvent>	(hLib,cszNxCoreSignalWaitEvent)) ) {

		pfGetStateData =  NxCoreProcAddr<NxCoreGetStateData>(hLib,cszNxCoreGetStateData);

		return pfAPIVersion();
	  }
	  UnloadNxCore();	// failed to get one or more addresses clean up.
	}
	return 0;
  }

  void UnloadNxCore()
  {
	if( hLib ) 
	  ::FreeLibrary(hLib);
	memset(this,0,sizeof(*this));
  }

  ///// NxCoreAPI Functions available after successful call to LoadNxCore	/////////
  unsigned int APIVersion()
  {
	return pfAPIVersion();
  }

  int ProcessTape(const char* pszFilename,const char* pMemoryAddr,unsigned int controlFlags,int UserData,NxCoreCallback callbackFunction)
  {
    return pfProcessTape(pszFilename,pMemoryAddr,controlFlags,UserData,callbackFunction);
  }

  int ListTapes(unsigned int controlFlags,NxCoreCallbackTapeList stdcallbacklist,void* pYourParam)
  {
    return pfListTapes(controlFlags,stdcallbacklist,pYourParam);
  }

  int ListAPIDLLs(unsigned int controlFlags,NxCoreCallbackAPIList stdcallbacklist,void* pYourParam)
  {
    return pfListAPIDLLs(controlFlags,stdcallbacklist,pYourParam);
  }

  int SpinSymbols(unsigned short ListedExg,unsigned int controlFlags,unsigned int spinID,NxCoreCallback tempCallback=0,int tempUserData=0)
  {
    return pfSpinSymbols(ListedExg,controlFlags,spinID,tempCallback,tempUserData);
  }

  int SaveState(const char* szStateFilename,unsigned int controlFlags)
  {
    return pfSaveState(szStateFilename,controlFlags);
  }

  int GetStateData(char* pBuffer,int bufferSize,int stateType,int param1,int param2,NxString* pnxsSymOrCtc)
  {
    return pfGetStateData(pBuffer,bufferSize,stateType,param1,param2,pnxsSymOrCtc);
  }

  int StateGetExgQuotes(NxCoreStateExgQuotes* pStateExgQuotes,NxString* pnxsSymOrDateStrike=0)
  {
    return pfStateGetExgQuotes(pStateExgQuotes,pnxsSymOrDateStrike);
  }

  int StateGetMMQuotes(unsigned short ReportingExg,NxCoreStateMMQuotes* pStateMMQuotes,NxString* pnxsSymOrDateStrike=0)
  {
    return pfStateGetMMQuotes(ReportingExg,pStateMMQuotes,pnxsSymOrDateStrike);
  }

  int StateGetLastTrade(NxCoreStateTrade* pStateTrade,NxString* pnxsSymOrDateStrike=0)
  {
    return pfStateGetLastTrade(pStateTrade,pnxsSymOrDateStrike);
  }

  int PriceConvert(int lPrice,unsigned char PriceType,unsigned char PriceTypeNew)
  {
    return pfPriceConvert(lPrice,PriceType,PriceTypeNew);
  }

  int PriceFormat(char* szBuff,int lPrice,unsigned char PriceType,int expandWidth = 0,bool bCommas = false)
  {
    return pfPriceFormat(szBuff,lPrice,PriceType,expandWidth,bCommas);
  }

  double PriceToDouble(int lPrice,unsigned char PriceType)
  {
    return pfPriceToDouble(lPrice,PriceType);
  }

  void DateFromNDays(NxDate* pnxDate)
  {
    pfDateFromNDays(pnxDate);
  }

  void DateFromYMD(NxDate* pnxDate)
  {
    pfDateFromYMD(pnxDate);
  }

  const char* GetDefinedString(int ixTable,int ixString)
  {
    return pfGetDefinedString(ixTable,ixString);
  }

  int GetTapeName(char* szBuffTapename,int nBufferBytes)
  {
    return pfGetTapeName(szBuffTapename,nBufferBytes);
  }

  int SignalWaitEvent(const NxCoreSystem* pNxCoreSystem,bool bAlwaysSignal)
  {
	return pfSignalWaitEvent(pNxCoreSystem,bAlwaysSignal);
  }

};

#endif // NXCORE_API_CLASS_H