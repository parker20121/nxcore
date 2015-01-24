#ifndef NxCoreAPI_h
#define NxCoreAPI_h
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	CONTENTS COPYRIGHT 2003-2008(c) Nanex, LLC, Winnetka, IL. ALL RIGHTS RESERVED.					  //  
//	YOU MAY NOT TRANSMIT THE CONTENTS OF THIS FILE WITHOUT EXPRESS WRITTEN PERMISSION FROM NANEX,LLC. //
//	!!! THIS FILE CONTAINS CONFIDENTIAL INFORMATION !!!		SUPPORT: email: nxcore@nanex.net		  //
////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
///////////	SET TAB SPACE to 4 for optimal viewing.	/////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
////////////////    NxCoreAPI Version 2.1	/////////////////////////
/////////////////////////////////////////////////////////////////////

/********************************************************************
Outline:

  1. call NxCoreProcessTape with a pointer to your callback function.
  2. Process trade,quote and other messages in your callback function.

  myFunction()
  {
	NxCoreProcessTape("",0,0,0,myCallbackFunction);	 
  }

  myCallbackFunction(NxCoreSystem*,NxCoreMessage*)
  {
	switch(pNxCoreMessage->MessageType) {
	case NxMSG_STATUS:		  break;  // 25ms clock update or status change.
	case NxMSG_EXGQUOTE:	  break;  // Regional/BBO quote information.
	case NxMSG_MMQUOTE:		  break;  // Market Maker (Level 2 quotes).
	case NxMSG_TRADE:		  break;  // Last Sale/Time and Sales.	
	case NxMSG_CATEGORY:	  break;  // Fundamentals,open interest, dividends/splits
	case NxMSG_SYMBOLCHANGE:  break;  // Symbol addition, deletion, and modifications
	case NxMSG_SYMBOLSPIN:	  break;  // Scheduled spin at start of tape or on demand.
	}
	return NxCALLBACKRETURN_CONTINUE;
  }

NxCoreSystem	  	  Contains system information: version, date, time, status, etc.
NxCoreMessage		  Contains trading information.
	1. coreHeader	  Symbol identification.
	2. coreData		  Message data, contains one of:
	  a. ExgQuote		
	  b. MMQuote		
	  c. Trade			
	  d. Category	
	  e. SymbolChange	
	  f. SymbolSpin		
********************************************************************/
/*
Header changes
--------------
version	 date		  note
-------------------------------
2.3.198  2007.04.16	  added additional alignment fields for consistent structure sizes for packing sizes 1,2,4,8 and 16. NxCoreAPI.dll is compiled with packing set at 8 bytes.

*/

/////////////////////////////////////////////////////////////////////
//NxDate.DSTIndicator  Daylight Savings Time Indicator (DST) types.//
/////////////////////////////////////////////////////////////////////
#define NxDATE_DST_UNKNOWN	0	  // DST is unknown.
#define NxDATE_DST_ON		1	  // DST is on. From 1st Sunday (2am) in April, to Last Sunday (2am) in October, clock is ahead 1 hour from Standard Time.
#define NxDATE_DST_OFF		2	  // DST is on. From Last Sunday (2am) in October, to 1st Sunday (2am) in April, clock is at Standard Time.
#define NxDATE_DST_ONTODAY	3	  // DST on at 2am today. Must be 1st Sunday in April
#define NxDATE_DST_OFFTODAY	4	  // DST off at 2am today. Must be last Sunday in October

/////////////////////////////////////////////////////////////////////
/////////// NxDate.DayOfWeek  Day of week ///////////////////////////
/////////////////////////////////////////////////////////////////////
#define NxDOW_SUNDAY		0
#define NxDOW_MONDAY		1
#define NxDOW_TUESDAY		2
#define NxDOW_WEDNESDAY		3
#define NxDOW_THURSDAY		4
#define NxDOW_FRIDAY		5
#define NxDOW_SATURDAY		6

/////////////////////////////////////////////////////////////////////
////////////////////////// NxDate ///////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxDate {// 16 bytes. 
  unsigned int 		NDays;			// julian date. Number of days since Jan 1, 1883.
  unsigned short	Year;			// current year. 1999,2000,2001,2002,2003..
  unsigned char		Month;			// month of year.  1-12 (note this is not zero based)
  unsigned char		Day;			// day of month. 1-31 (not zero based)
  unsigned char     DSTIndicator;	// see NxDATE_DST_
  unsigned char     DayOfWeek;		// see NxDOW_
  unsigned short    DayOfYear;
  unsigned char		alignment[4];	// aligns NxDate to 16 bytes.
};

/////////////////////////////////////////////////////////////////////
////////////////////////// NxTime ///////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxTime {// 16 bytes.   
  unsigned int		MsOfDay;		// number of milliseconds since start of day -- midnight
  unsigned short    Millisecond;	// millisecond of second 0-999
  unsigned char		Second;			// second of minute 0-59
  unsigned char		Minute;			// minute of hour 0-59	
  unsigned char		Hour;			// hour of day. 0-23
  char				TimeZone;		// +/- hours to GMT.
  unsigned short	MsResolution;
  unsigned char		alignment[4];	// aligns NxTime to 16 bytes.
};

/////////////////////////////////////////////////////////////////////
///////////////////////// NxString //////////////////////////////////
/////////////////////////////////////////////////////////////////////
// NxString contains two 32-bit integer members that you can assign to each NxString, a unique "Atom" assigned by NxCoreAPI, and a zero terminated ascii string.
struct NxString {
  int				UserData1;	// first  32-bit user data space. Valid for 
  int				UserData2;	// second 32-bit user data space.
  unsigned short	Atom;		// unique/sequential/nonzero/constant 16 bit number assigned to the String. 
  char				String[1];	// array is acually [strlen(String)+1] and directly follows Atom in memory. (note String in pnxsDateAndStrike in NxOptionHdr is NOT null terminated -- it is exactly 2 bytes)
};
// Example Memory layout for NxString with UserData1=1,UserData2=2,Atom=3,and String = "eMSFT":
//	UserData1	dd	0x01000000	  ; 4 bytes for UserData1 
//  UserData2	dd  0x02000000	  ; 4 bytes for UserData2
//	Atom		dw	0x0300		  ; 2 bytes for Atom
//  String      db  'eMSFT',0	  ; 6 bytes. 5 for 'eMSFT' plus 1 for null terminator -- note: for option contracts, 
//											  the pnxsDateAndStrike member is a NxString containing 2 alpha characters 
//											  (one for the expiration date code, and one for the strike code) and does NOT
//											  include a null terminator.

#define NxATOM_MAX_16BIT	65535

/////////////////////////////////////////////////////////////////////
//////// NxCoreSystem.ClockUpdateInterval definitions ///////////////
/////////////////////////////////////////////////////////////////////
#define NxCLOCK_NOCHANGE				  0	  // clock has not changed since last message.
#define NxCLOCK_CLOCK					  1	  // NxCoreSystem.nxTime.Millisecond is longest interval that changed. 
#define NxCLOCK_SECOND					  2	  // NxCoreSystem.nxTime.Second is longest interval that changed. 
#define NxCLOCK_MINUTE					  3	  // NxCoreSystem.nxTime.Minute is longest interval that changed. 
#define NxCLOCK_HOUR					  4	  // NxCoreSystem.nxTime.Hour is longest interval that changed. 

/////////////////////////////////////////////////////////////////////
/////////  NxCoreSystem.Status definitions //////////////////////////
/////////////////////////////////////////////////////////////////////
#define NxCORESTATUS_RUNNING	  		  0 // system is running.
#define NxCORESTATUS_INITIALIZING		  1 // first message sent when processing a tape.
#define NxCORESTATUS_COMPLETE			  2 // last message sent when processing a tape, unless user aborts by returning a non-zero value (NxCALLBACKRETURN_STOP) from callback.
#define NxCORESTATUS_SYNCHRONIZING		  3 // synchronization reset detected in the tape, continuing processing
#define NxCORESTATUS_ERROR				  4 // an error has occurred and processing halted. NxCORESTATUS_COMPLETE message will follow
#define NxCORESTATUS_WAITFORCOREACCESS	  5 // NxCoreAPI.dll is waiting for NxCore Access to connect.
#define NxCORESTATUS_RESTARTING_TAPE	  6 // NxCoreAPI.dll is restarting the same tape from the beginning at users request
#define NxCORESTATUS_LOADED_STATE		  7 // NxCoreAPI.dll initialized from state
#define NxCORESTATUS_SAVING_STATE		  8 // NxCoreAPI.dll will capture the state after this message
#define NxCORESTATUS_SYMBOLSPIN			  9 // signals system symbol spin state: StatusData == 0 when starting, 1 when complete
#define NxCORESTATUS_TAPEOPEN			  10 // signals system when tape header has been read, tape definition string and date are available for the first time.

/////////////////////////////////////////////////////////////////////
//////// StatusData member may be set to one of the following ///////
/////////////////////////////////////////////////////////////////////
// when NxCoreSystem.Status == NxCORESTATUS_RUNNING
  #define NxCSRUNMODE_SRC_DISK_FILE			0 // processing from a tape on disk
  #define NxCSRUNMODE_SRC_ACCESS_FILE		1 // processing from NxCoreAccess's file -- dll has not yet reached memory buffers
  #define NxCSRUNMODE_SRC_ACCESS_MEMORY		2 // processing from NxCoreAccess's memory buffers

  #define NxCSRUNMODE_SRC_ACCESS_MB_OLDEST	2 // processing oldest of NxCoreAccess's memory buffers
  #define NxCSRUNMODE_SRC_ACCESS_MB_LOWER	3 // processing older half of NxCoreAccess's memory buffers   
  #define NxCSRUNMODE_SRC_ACCESS_MB_UPPER	4 // processing most recent half of NxCoreAccess's memory buffers
  #define NxCSRUNMODE_SRC_ACCESS_MB_SECOND	5 // processing 2nd most recent.
  #define NxCSRUNMODE_SRC_ACCESS_MB_CURRENT	6 // processing most recent of NxCoreAccess's memory buffers

// when NxCoreSystem.Status == NxCORESTATUS_SAVING_STATE
  #define NxCSSAVESTATE_CAPTURE			 0		  // the core state will be captured after you return from this callback
  #define NxCSSAVESTATE_COMPLETE		 1		  // the save core state operation is complete
  #define NxCSSAVESTATE_ERR_CREATEFILE	-1		  // failed to open the specified tape filename
  #define NxCSSAVESTATE_ERR_DISKSPACE	-2		  // not enough disk space to complete the operation
  #define NxCSSAVESTATE_ERR_MEMORY	    -3		  // insufficient memory to complete the operation

// when NxCoreSystem.Status == NxCORESTATUS_SYMBOLSPIN			  
  #define NxCSSYMBOLSPIN_STARTING 		 0		  // the system symbol spin is starting -- note -- status will return NxCORESTATUS_RUNNING after this message
  #define NxCSSYMBOLSPIN_COMPLETE		 1		  // the system symbol spin has completed

//////////////////////////////////////////////
///////////	     New in Ver 2.0	   ///////////
///////////   NxCore Access Status  //////////
//////////////////////////////////////////////
struct NxAccessStatus {
  unsigned int	  Version;
  int			  AcctExpireDays;
  unsigned char	  Status;
  unsigned char	  AttnLevel;
  unsigned char	  StatusMCS;	  // when a multicast client -- shows status of NxCore Access Multicast Server
  unsigned char	  AttnLevelMCS;
  unsigned short  UDPPingTime;		  // round-trip in milliseconds
  unsigned short  UDPPktLossPct;	  // # packet resends per 1,000
  int			  NxClockDiff;		  
  unsigned int	  NxClockReceived;
  int			  SysClockDrift;	
  unsigned int	  SecondsRunning;		  // count of seconds NxCore Access has been running
  unsigned int	  RecvBytes;	  // recent bandwidth in bytes/second (divide by 10000 to get kbps
  unsigned int	  SendBytes;	  // recent bandwidth in bytes/second (divide by 10000 to get kbps
  unsigned short  MCClients;
  unsigned short  MCCLicenses;
  unsigned char	  ChatMsgIndc;
};

/////////////////////////////////////////////////////////////////////
///////////////////////// NxCoreSystem //////////////////////////////
/////////////////////////////////////////////////////////////////////
//NxCoreSystem is first parameter passed to user callback function: int UserCallBackFunction(const NxCoreSystem*,const NxCoreMessage*);
struct NxCoreSystem { // 76 bytes
  int				UserData;		// UserData value passed to NxCoreProcessTape 
  int				DLLVersion;		// Version of NxCoreAPI.dll
  NxDate			nxDate;			// date of tape.
  NxTime			nxTime;			// time of tape. This is the subsecond NxClock 
  int 				ClockUpdateInterval; // indicates changes to nxTime
  int				Status;			// processing status. see NxCORESTATUS_
  int				StatusData;		// additional information depending on Status
  const char*		StatusDisplay;	// text display combining Status and StatusData	
  const char*		TapeDisplay;	// text from the header of the tape.
  void*				Module;			// module of the dll for the calling process.
  void*				ThreadInstance;	// thread instance identifier
  const char*		PermissionSet;	// text of PermissionSet "AA", etc.
  NxAccessStatus*	pNxAStatus;		// Ver 2.0 and higher (NxCoreAccess and NxCoreAPI.dll): during real-time processing, status of NxCoreAccess. Updated once per second.
};

/////////////////////////////////////////////////////////////////////
/////////////////////// NxCoreData Members///////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/////////  NxCoreQuote.NasdaqBidTick definitions  ///////////////////
/////////////////////////////////////////////////////////////////////
#define NxQTE_NASDBIDTICK_NA	  0	 // bid tick is not applicable for symbol.
#define NxQTE_NASDBIDTICK_UP	  1  // set when bid tick is up.
#define NxQTE_NASDBIDTICK_DN	  2  // set when bid tick is down.
#define NxQTE_NASDBIDTICK_NO	  3  // NASDAQ stock that does not carry a bid tick (small caps, etc)

/////////////////////////////////////////////////////////////////////
////////// NxCoreQuote.Refresh definitions //////////////////////////
/////////////////////////////////////////////////////////////////////
#define NxQTE_REFRESH_REALTIME	  0	 // realtime quote, not a refresh
#define NxQTE_REFRESH_SOURCE      1	 // The source of the data has refreshed quote. Only seen for Market Makers in morning reset
#define NxQTE_REFRESH_MISMATCH    2	 // A mismatch detected in NxCore processors from last received quote information. 
#define NxQTE_REFRESH_SYNCHRONIZE 3	 // NxCore processors are resending last known quotes to ensure synchronization. Rare.

/////////////////////////////////////////////////////////////////////
////////////////// NxCoreQuote //////////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreQuote {//40 bytes. 
  int				AskSize;		// Ask size 
  int				BidSize;		// Bid size 
  int				AskSizeChange;
  int				BidSizeChange;
  int				AskPrice;		// ask Price (nxPriceType)
  int				BidPrice;		// bid Price (nxPriceType)
  int				AskPriceChange;
  int				BidPriceChange;
  unsigned char		PriceType;	// lAskPrice and lBidPrice always have the same price type.
  unsigned char		Refresh;		// set to non-zero if this message is a refresh as opposed to real-time.
  unsigned char		QuoteCondition;	// quote condition assigned by exchange. Use NxCoreGetDefinedString(NxST_QUOTECONDITION,QuoteCondition) for text name
  unsigned char		NasdaqBidTick;	// for nasdaq stocks. nasdaqBidTick.
  unsigned char		alignment[4];
};

/////////////////////////////////////////////////////////////////////
////////  NxCoreExgQuote.BBOChangeFlags definitions.  ///////////////
/////////////////////////////////////////////////////////////////////
//None or more maybe set. Allows for fast testing for changes
#define NxBBOCHANGE_BIDEXG	  0x01	// BestBidExg  != PrevBestBidExg
#define NxBBOCHANGE_BIDPRICE  0x02	// BestBidPriceChange != 0
#define NxBBOCHANGE_BIDSIZE	  0x04	// BestBidSizeChange != 0
#define NxBBOCHANGE_ASKEXG	  0x10  // BestAskExg  != PrevBestAskExg
#define NxBBOCHANGE_ASKPRICE  0x20	// BestAskPriceChange != 0
#define NxBBOCHANGE_ASKSIZE	  0x40	// BestAskSizeChange != 0

struct NxCoreExgQuote {// 88 bytes.
  NxCoreQuote		coreQuote;// 40 bytes.
  int				BestAskPrice;
  int				BestBidPrice;
  int				BestAskPriceChange;
  int				BestBidPriceChange;
  int				BestAskSize;
  int				BestBidSize;
  int				BestAskSizeChange;
  int				BestBidSizeChange;
  unsigned short	BestAskExg;
  unsigned short	BestBidExg;
  unsigned short    PrevBestAskExg;
  unsigned short    PrevBestBidExg;
  unsigned char		BestAskCondition;
  unsigned char		BestBidCondition;
  unsigned char     BBOChangeFlags;
  unsigned char		ClosingQuoteFlag;
  unsigned char		alignment[4];	   // alignment 
};

/////////////////////////////////////////////////////////////////////
////////// NxCoreMMQuote.MarketMakerType definitions ////////////////
/////////////////////////////////////////////////////////////////////
#define NxMMT_UNKNOWN		0	// not set, not applicable
#define NxMMT_REGULAR	    1	// regular market maker -- not primary, not passive.
#define NxMMT_PRIMARY	    2	// primary market maker.
#define NxMMT_PASSIVE	    3	// passive market maker.

/////////////////////////////////////////////////////////////////////
////////// NxCoreMMQuote.QuoteType definitions //////////////////////
/////////////////////////////////////////////////////////////////////
#define NxMMQT_UNKNOWN		0
#define NxMMQT_REGULAR		1
#define NxMMQT_PASSIVE		2  // passive quote. (not same thing as MMT_PASSIVE which is a type of market maker)
#define NxMMQT_SYNDICATE	3  // Syndicate		BID ONLY
#define NxMMQT_PRESYNDICATE 4  // Presyndicate	BID ONLY
#define NxMMQT_PENALTY		5  // Penalty.		BID ONLY

struct NxCoreMMQuote {// 48 bytes
  NxCoreQuote		coreQuote;	//40 bytes.
  NxString*			pnxStringMarketMaker;
  unsigned char		MarketMakerType;
  unsigned char		QuoteType;
  unsigned char		alignment[2]; // 2.3.198, 2007.04.16
};

/////////////////////////////////////////////////////////////////////
//////////////  NxCoreTrade.PriceFlags definitions.  ////////////////
/////////////////////////////////////////////////////////////////////
#define NxTPF_SETLAST			  0x01  // Update the 'last' price field with the trade price.
#define NxTPF_SETHIGH			  0x02  // Update the session high price.
#define NxTPF_SETLOW			  0x04  // Update the session low price.
#define NxTPF_SETOPEN			  0x08  // Indicates trade report is a type of opening report. For snapshot indicies, this is the "open" field. See TradeConditions for the types that update this flag.
#define NxTPF_EXGINSERT			  0x10	// Trade report was inserted, not real-time. Often follows EXGCANCEL for trade report corrections.
#define NxTPF_EXGCANCEL			  0x20	// Cancel message. The data in this trade report reflects the state of the report when first sent, including the SETLAST/increment volume, etc flags. 

//added 2.3.50
#define NxTPF_SETTLEMENT		  0x40	// price is settlement
/////////////////////////////////////////////////////////////////////
/////////////  NxCoreTrade.ConditionFlags definitions.  /////////////
/////////////////////////////////////////////////////////////////////
/// The trade condition (NxCoreTrade.TradeCondition) determines   ///
/// whether a trade price updates a session high, low, or last    ///
/// prices according to exchange rules. These rules are condensed ///	 		
/// into ConditionFlags.										  ///
/////////////////////////////////////////////////////////////////////
#define NxTCF_NOLAST			  0x01	// not eligible to update last price.
#define NxTCF_NOHIGH			  0x02	// not eligible to update high price.
#define NxTCF_NOLOW				  0x04	// not eligible to update low price.

/////////////////////////////////////////////////////////////////////
////////////// NxCoreTrade.VolumeType definitions.  /////////////////
/////////////////////////////////////////////////////////////////////
#define NxTVT_INCRVOL				0 // incremental volume. UNSIGNED, increment tick volume and total volume. Note: dwSize maybe zero -- which updates the tick Volume and leaves total volume unchanged
#define NxTVT_NONINCRVOL			1 // non-incremental volume. Rarely used outside of indexes. Intraday and Open detail in NYSE stocks.
#define NxTVT_TOTALVOL				2 // dwSize *is* the total volume -- used mainly in a few indexes
#define NxTVT_TOTALVOLx100			3 // dwSize *is* the total volume/100. multiply by 100 for current total volume. RARE -- in DOW indexes

/////////////////////////////////////////////////////////////////////
////////////// NxCoreTrade.nxAnalysis.SigHiLoType definitions.  /////
/////////////////////////////////////////////////////////////////////
#define NxRTA_SIGHL_EQ				0
#define NxRTA_SIGHL_LOWEST			1
#define NxRTA_SIGHL_LOW				2
#define NxRTA_SIGHL_HIGH			3
#define NxRTA_SIGHL_HIGHEST			4

/////////////////////////////////////////////////////////////////////
///////// NxCoreTrade.nxAnalysis.QteMatchTypeRGN and		   //////
///////// NxCoreTrade.nxAnalysis.QteMatchTypeBBO definitions.  //////
/////////////////////////////////////////////////////////////////////
#define NxRTA_QMTYPE_NONE			0	// no recent quotes.
#define NxRTA_QMTYPE_BID			1	// at bid or higher.
#define NxRTA_QMTYPE_ASK			2	// at ask or lower
#define NxRTA_QMTYPE_INSIDE			3	// exactly between bid/ask. 0 dist means locked.
#define NxRTA_QMTYPE_BELOWBID		4	// lower than bid
#define NxRTA_QMTYPE_ABOVEASK		5	// higher than ask

/////////////////////////////////////////////////////////////////////
///////// NxCoreTrade.nxAnalysis.QteMatchFlagsRGN and		    /////
///////// NxCoreTrade.nxAnalysis.QteMatchFlagsBBO definitions.	/////
/////////////////////////////////////////////////////////////////////
#define NxRTA_QTEMATCHFLAG_OLDER	0x01  // matched a quote older than a previously matched quote, possible indication of staleness
#define NxRTA_QTEMATCHFLAG_CROSSED	0x02	

/////////////////////////////////////////////////////////////////////
/////////////////// NxCoreTrade.nxAnalysis  /////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxCTAnalysis { // 24 bytes. 
  int				FilterThreshold;		// threshold for filter analysis
  unsigned char		Filtered;
  unsigned char		FilterLevel;
  unsigned char		SigHiLoType;
  unsigned char     alignment;
  unsigned int		SigHiLoSeconds;
  int				QteMatchDistanceRGN;
  int				QteMatchDistanceBBO;
  unsigned char		QteMatchTypeRGN;
  unsigned char		QteMatchTypeBBO;
  unsigned char		QteMatchFlagsRGN;
  unsigned char		QteMatchFlagsBBO;
};

/////////////////////////////////////////////////////////////////////
/////////////////////  NxCoreTrade //////////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreTrade {  // 88 bytes.
  int				Price;			// price of trade report. Note: There is also a "Last" field in this structure. Not all Prices update the "Last". FormT (after hours) trades are one example.
  unsigned char		PriceType;		// Applies to Price,Open,High,Low,Last,Tick,Net Change,nxAnalysis.FilterThreshold,nxAnalysis.QteMatchDistanceRGN/BBO fields in this record.  
  unsigned char		PriceFlags;		// flags indicating if trade is new high/low, etc. see defines beginning with NxTPF_ .
  unsigned char		TradeCondition;	// trade condition code assigned by exchange. Use NxCoreGetDefinedString(NxST_TRADECONDITION,TradeCondition) for text name
  unsigned char		ConditionFlags;	// converts TradeCondition (and BATE when present) to set eligibility flags (defines begin with NxTCF_ )
  unsigned char		VolumeType;		// indicates how the Size field changes (or not) the TotalVolume/TickVolume fields (defines begin with NxTVT_ )
  unsigned char		BATECode;		// 'B'id,'A'sk, or 'T'rade indicator for some commodities. ('E'xception not used -- historical artifact).
  unsigned char		alignment[2];
  unsigned int		Size;
  unsigned int		ExgSequence;	// original exchange sequence number.
  unsigned int		RecordsBack;	// for inserts and deletes, indicates the number of records from the last record received the cancel/insert applies.
  NxCTAnalysis		nxAnalysis;
  unsigned __int64	TotalVolume;
  unsigned int		TickVolume;
  int				Open;
  int				High;
  int				Low;
  int				Last;
  int				Tick;
  int				NetChange;
  unsigned char		alignment2[4];	// 2.3.198, 2007.04.16
};

/////////////////////////////////////////////////////////////////////
////////////  NxCategoryField.FieldType definitions. ////////////////
/////////////////////////////////////////////////////////////////////
#define NxCFT_UNKNOWN	  0
#define NxCFT_64BIT		  1    
#define NxCFT_32BIT		  2
#define NxCFT_STRINGZ	  3
#define NxCFT_DOUBLE	  4
#define NxCFT_PRICE		  5
#define NxCFT_DATE 		  6
#define NxCFT_TIME		  7
#define NxCFT_NxSTRING	  8
#define NxCFT_STRING_IDX  9
#define NxCFT_STRING_MAP 10

struct NxPrice {  // 8 bytes.
  int				Price;
  unsigned char		PriceType;
};

struct StringTableItem {
  unsigned int		ixTable;
  unsigned int		idString;
};

struct NxCategoryField {// 32 bytes
  const char*		FieldName;  
  const char*		FieldInfo;  

  unsigned char		Set;
  unsigned char		FieldType;
  unsigned char		alignment[6];
  union	{			// field is ONE of the following depending on 'catFieldType' member.
	__int64			i64Bit;			  // 1: NxCFT_64BIT
	int				i32Bit;			  // 2: NxCFT_32BIT
	const char*		StringZ;		  // 3: NxCFT_STRINGZ
	double			Double;			  // 4: NxCFT_DOUBLE
	NxPrice			nxPrice;		  // 5: NxCFT_PRICE
	NxDate			nxDate;			  // 6: NxCFT_DATE
	NxTime			nxTime;			  // 7: NxCFT_TIME
	NxString*		pnxString;		  // 8: NxCFT_NxSTRING
	StringTableItem	stringTableItem;  // 9: NxCFT_STRING_IDX, 10:NxCFT_STRING_MAP
  }					data;
};

struct NxCoreCategory {	// 12 bytes
  NxString*			pnxStringCategory;
  NxCategoryField*	pnxFields;
  unsigned short	NFields;
  unsigned char		alignment[2]; // 2.3.198, 2007.04.16
};

struct NxOptionHdr {// 48 bytes
  NxString*			pnxsDateAndStrike;	// the string member contains 2 alpha characters (expire code and strike code) -- and is NOT null terminated.
  NxString*			pnxsUnderlying;		// This is the same pointer as pnxStringSymbol in the underlying symbol (unless Atom == 0, which is the case if you are not subscribed to the underlying's exchange).

  NxString*			pnxsSeriesChain;	// All option series in the chain share the same pointer to this string.

  NxString*			pnxsSpecialSettle;  // All option series with the same special settlement share the same pointer to this string
  unsigned short    exgUnderlying;		// exchange code of the underlying symbol
  unsigned short	contractUnit;		// default is 100 (shares,units, etc). 
  int				strikePrice;		// OPRA sourced strike price. implied 3 decimal places. i.e.	35000 == 35.000 == 35
  int				strikePriceEst;		// if non-zero, the nxCore server includes the estimated strike price for this option contract.
  unsigned char		PutCall;			// the value 1 == put, 0 == call.
  unsigned char		expirationCycle;	// 1 == January cycle, 2 == February cycle, 3 == March cycle.
  unsigned char		oicStrikeAge;		// number from 0-7. Incremented each trading day the strikePrice is received from OPRA.
  unsigned char		nxStrikeMatch;		// number from 0-15 indicating the likelihood that the strikePrice is correct.
  NxDate			nxExpirationDate;	// The date this option contract expires.
};


/////////////////////////////////////////////////////////////////////
/////////////  NxCoreSymbolChange.Status values /////////////////////
/////////////////////////////////////////////////////////////////////
#define NxSS_ADD	0	//	symbol has been added effective immediately. 
#define NxSS_DEL	1	//	symbol marked for deletion at end of session.
#define NxSS_MOD	2	//  symbol changed, or moved from one ListedExg to another. pnxsSymbolOld, pnxOptionHdrOld, and ListedExgOld are set.

struct NxCoreSymbolChange { // 16 bytes
  unsigned char		Status;	  // NxSS_ADD,NxSS_DEL,NxSS_MOD
  char				StatusChar;	  // 'A'dd,'D'elete,'M'odify. 
  unsigned short	alignment;
  NxString*			pnxsSymbolOld;
  NxOptionHdr*		pnxOptionHdrOld;  // non-zero for option contract changes 
  unsigned short	ListedExgOld;
  unsigned char		alignment2[2];	// 2.3.198, 2007.04.16
};

// Every SymbolSpin  contains the current iteration zero-based count (index) and the total number of symbols that will be iterated.
// only options contracts use the "sub" count and limit fields.
struct NxCoreSymbolSpin { // 20 bytes
  unsigned int		SpinID;			// NxCoreAPI.dll sets this to 0 for the initial automatic SymbolSpin at the start of each tape. The value passed to the SymbolIterate function for the spinID will appear in this data member.
  unsigned int		IterLimit;		// for(IterCount = 0; IterCount < IterLimit; ++IterCount)
  unsigned int		SubIterLimit;	//	  for(SubIterCount = 0; SubIterCount < SubIterLimit; ++SubIterCount) => for option contracts within one option series. 
  unsigned int		IterCount;		
  unsigned int		SubIterCount;
};

#define NxSPIN_ALL_EXGS		((unsigned short)-1)
#define NxSPIN_NON_OPTIONS	0
#define NxSPIN_OPTIONS		(0x00000001)
/////////////////////////////////////////////////////////////////////
///////////////  NxCoreMessage.MessageType  /////////////////////////
/////////////////////////////////////////////////////////////////////
#define NxMSG_STATUS		0
#define NxMSG_EXGQUOTE		1
#define NxMSG_MMQUOTE		2
#define NxMSG_TRADE			3
#define NxMSG_CATEGORY		4
#define NxMSG_SYMBOLCHANGE	5 
#define NxMSG_SYMBOLSPIN	6 

union NxCoreData {	// 88 bytes
  NxCoreExgQuote	 ExgQuote;
  NxCoreMMQuote		 MMQuote;
  NxCoreTrade		 Trade;
  NxCoreCategory	 Category;
  NxCoreSymbolChange SymbolChange;
  NxCoreSymbolSpin	 SymbolSpin;
};

/////////////////////////////////////////////////////////////////////
///////////////////////// NxCoreHeader //////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreHeader {// 48 bytes
  NxString*			pnxStringSymbol;
  NxOptionHdr*		pnxOptionHdr;
  NxDate			nxSessionDate;	
  NxTime			nxExgTimestamp;	
  unsigned short	ListedExg;		  // Listed or Primary Exchange. Use NxCoreGetDefinedString(NxST_EXCHANGE,ListedExg) for text name 
  unsigned short	ReportingExg;	  // Exchange sending quote/trade. Use NxCoreGetDefinedString(NxST_EXCHANGE,ReportingExg) for text name 
  unsigned char		SessionID;	
  unsigned char		alignment;
  unsigned short    PermissionID;	  // identifies the permissions (exchanges,etc.) in the tape. Use NxCoreGetDefinedString(NxST_PERMID,PermissionID) for text name 
};


/////////////////////////////////////////////////////////////////////
///////////////////////// NxCoreMessage//////////////////////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreMessage { // 144 bytes
  NxCoreHeader		 coreHeader;
  NxCoreData		 coreData;
  unsigned int		 MessageType;
  unsigned char		 alignment[4];	// 2.3.198, 2007.04.16
};


/*******************************************************************/
/******************** NxCoreState data *****************************/
/*******************************************************************/

struct NxCoreStateExgQuote {  // 20 bytes
  int				  AskPrice;		
  int				  BidPrice;		
  int				  AskSize;		
  int				  BidSize;		
  unsigned short	  ReportingExg;
  unsigned char		  QuoteCondition;	// quote condition		
  unsigned char		  alignment[1];	  // 2.3.198, 2007.04.16 
};

struct NxCoreStateMMQuote {	// 24 byres
  int				  AskPrice;		
  int				  BidPrice;		
  int				  AskSize;		
  int				  BidSize;		
  NxString*			  pnxStringMarketMaker;
  unsigned char		  MarketMakerType;
  unsigned char		  QuoteType;
  unsigned char		  alignment[2];	// 2.3.198, 2007.04.16 
};

struct NxCoreStateExgQuotes { // 248 bytes
  unsigned short	  StateQuoteCount;
  unsigned char		  PriceType;		
  unsigned char		  NasdaqBidTick;	// for nasdaq stocks. nasdaqBidTick.
  unsigned short	  BestAskExg;
  unsigned short	  BestBidExg;
  NxCoreStateExgQuote StateExgQuotes[12];
};


struct NxCoreStateMMQuotes {  // 12292 bytes
  unsigned short	  StateQuoteCount;
  unsigned char		  PriceType;		
  unsigned char		  NasdaqBidTick;	// for nasdaq stocks. nasdaqBidTick.
  NxCoreStateMMQuote  StateMMQuotes[512];
};

struct NxCoreStateTrade {  // 48 bytes
  unsigned __int64	  TotalVolume;
  unsigned int		  TickVolume;
  unsigned char		  PriceType;
  unsigned char		  PriceFlags;
  unsigned char		  ConditionFlags;
  unsigned char		  VolumeType;
  int				  Open;
  int				  High;
  int				  Low;
  int				  Last;
  int				  NetChange;
  int				  Price;
  int				  Threshold;
  int				  Tick;
};


///////////////////// New state //////////////////////////////////////

// used in stateType parameter to NxCoreGetStateData
#define NxSTATETYPE_OHLCTRADE  1
#define NxSTATETYPE_EXGQUOTES  2
#define NxSTATETYPE_MMQUOTES   3	// set param1 to the Reporting Exchange of the MMQuotes you want


struct NxCoreStateOHLCTrade {  
  unsigned __int64	  TotalVolume;
  unsigned int		  TickVolume;
  unsigned char		  PriceType;
  unsigned char		  PriceFlags;
  unsigned char		  ConditionFlags;
  unsigned char		  VolumeType;
  int				  Open;
  int				  High;
  int				  Low;
  int				  Last;
  int				  NetChange;
  int				  Price;
  int				  Threshold;
  int				  Tick;
  int				  TradeSize;
};

/////////////////////////////////////////////////////////////////////
/////////// NxCoreTapeFile used with NxCoreListTapes  ///////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreTapeFile {
  char				PathnameStrZ[260];
  int				PathnameLen;
  int				FilenameLen;
  unsigned int		FileAttributes;
  unsigned __int64	FileSize;

  NxDate			TapeDate; 
  char				PermSet[8];
};

#define NxLTF_SEARCH_LOCAL		  0
#define NxLTF_SEARCH_NETWORK	  0x00000001
#define NxLTF_SEARCH_REMOVEABLE	  0x00000002


/////////////////////////////////////////////////////////////////////
///////// NxCoreAPIDLLFile used with NxCoreListAPIDlls //////////////
/////////////////////////////////////////////////////////////////////
struct NxCoreAPIDLLFile {
  char				PathnameStrZ[260];
  int				PathnameLen;
  int				FilenameLen;
  unsigned int		FileAttributes;
  unsigned __int64	FileSize;

  NxDate			BuildDate; 
  NxTime			BuildTime;
  unsigned char		verMajor;
  unsigned char		verMinor;
  unsigned short	verBuild;
};


/////////////////////////////////////////////////////////////////////
///////// String Table Indexes for NxCoreGetDefinedString	/////////
/////////////////////////////////////////////////////////////////////
#define NxST_PERMID					   2
#define NxST_EXCHANGE				   3
#define NxST_TRADECONDITION			   4
#define NxST_QUOTECONDITION			   5
#define NxST_CATEGORY				   6
#define NxST_HALTSTATUS				   7
#define NxST_HALTREASONTYPE			   8
#define NxST_OPENINDICATIONTYPE		   9
#define NxST_ORDERIMBALANCETYPE		  10
#define NxST_TRADEREPORTREASON		  11
#define NxST_EXGCORRECTIONMAP		  12
#define NxST_SYMBOLCHANGETYPE		  13
#define NxST_OHLCFLAGS				  14
#define NxST_EXDIVIDENDATTRIB		  15
#define NxST_OPRAEXCHANGELIST		  16

#define NxST_NASDBIDTICK			  17
#define NxST_MARKETMAKERTYPE		  18
#define NxST_MMQUOTETYPE			  19
#define NxST_REFRESHTYPE			  20		  
#define NxST_EXPIRATIONCYCLE		  21
#define NxST_VOLUMETYPE				  22
#define NxST_QTEMATCHTYPE			  23
#define NxST_QTEMATCHFLAGS			  24
#define NxST_SIGHIGHLOWTYPE			  25
#define NxST_BBOCHANGEFLAGS			  26
#define NxST_TRADECONDFLAGS			  27
#define	NxST_PRICEFLAGS				  28

#define NxST_SETTLEFLAGMAP			  29

#define NxST_SYMBOLFNCLSTATUSMAP	  30
#define NxST_EQUITYCLASSIFICATIONS	  31

#define NxST_SYMBOLTYPES			  32

/////////////////////////////////////////////////////////////////////
/////////////// controlFlags for NxCoreProcessTape //////////////////
/////////////////////////////////////////////////////////////////////
#define NxCF_EXCLUDE_QUOTES		  0x00000001	// exclude Exchange Quotes
#define NxCF_EXCLUDE_QUOTES2	  0x00000002	// exclude MMQuotes
#define NxCF_EXCLUDE_OPRA		  0x00000004	// exclude all option from OPRA (US)
#define NxCF_FAVOR_NBBO			  0x00000008
#define NxCF_MEMORY_ADDRESS		  0x04000000	// if set, memory is allocated (if possible) at the address specified by the 2nd parameter to NxCoreProcessTape (pBaseMemory)
#define NxCF_EXCLUDE_CRC_CHECK	  0x08000000	// exclude crc checking
//////////////////
//// obsolete ////
//////////////////
#define NxCF_CRC_CHECK			  0         	// include additional crc checking -- changed after 1.5 so default was enabled

/////////////////////////////////////////////////////////////////////
///////// control flag for NxCoreSaveState	/////////////////////////
/////////////////////////////////////////////////////////////////////
#define NxSAVESTATE_GRADUALLY	  0				//  Default saves gradually: writes out memory block to file in 1mb increments
#define NxSAVESTATE_ONEPASS		  0x00000001	// save memory state in one pass -- save completes in one pass.
#define NxSAVESTATE_CANCEL		  0x00000002

/////////////////////////////////////////////////////////////////////
// return one of these values from your UserCallback function.	/////
/////////////////////////////////////////////////////////////////////
#define NxCALLBACKRETURN_CONTINUE		  0	// continue normal processing
#define NxCALLBACKRETURN_STOP			  1	// stop processing and return control to function that called NxCoreProcessTape
#define NxCALLBACKRETURN_RESTART		  2	// restart processing from beginning of tape. UserData and strings are preserved.

/////////////////////////////////////////////////////////////////////
/////////// macros used by NxCoreAPIVersion   ///////////////////////
/////////////////////////////////////////////////////////////////////
#define NxCORE_VER_MAJOR(x)	 ((unsigned int)(x) >> 24)
#define NxCORE_VER_MINOR(x)	(((unsigned int)(x) >> 16) & 0x000000ff)
#define NxCORE_VER_BUILD(x)	( (x) & 0x0000ffff )

/////////////////////////////////////////////////////////////////////
//////// error codes returned from NxCoreAPI functions  /////////////
/////////////////////////////////////////////////////////////////////
#define NxAPIERR_NO_ERROR				   0
#define NxAPIERR_USER_STOPPED			   1
#define NxAPIERR_NOT_CALLBACK_THREAD	  -1
#define NxAPIERR_BAD_PARAMETERS			  -2
#define NxAPIERR_EXCEPTION				  -3
#define NxAPIERR_OPEN_TAPE_FILE_FAILED	  -4	
#define NxAPIERR_INITIALIZE_MEMORY_FAILED -5	
#define NxAPIERR_NOLISTED_EXG			  -6  // symbol spin -- listed exchange does not exist in current tape
#define NxAPIERR_NOSYMBOLS_FOR_LSTEXG	  -7  // symbol spin -- no symbols of the type (options/not options) for the exchange specified
#define NxAPIERR_NOSESSION_FOR_SYMBOL	  -8  // symbol spin -- no session or session does not have data type
#define NxAPIERR_NODATATYPE_FOR_SESSION	  -9  // symbol spin -- There's a session, but no trade/quote/mmquote data for session
#define NxAPIERR_NODATA_FOR_REPEXG		  -10 // symbol spin -- MMQuotes. there is session with data, but no entry for the specified reporting exg
#define NxAPIERR_ZEROED_DATA_FOR_SESSION  -11 // symbol spin -- there is a session, but data is all zero
#define NxAPIERR_SAVE_STATE_IN_PROGRESS	  -12
#define NxAPIERR_NOT_SUPPORTED			  -13


/*******************************************************************/
/**** defines to simplify transition from 1.x to 2.0 ***************/
/*******************************************************************/
#define NXCORE_NAG_RENAME_V_1 // defined to include the following:
#ifdef NXCORE_NAG_RENAME_V_1
// defines that rename function from versions before version 2
// these will become obsolete in later versions -- so rename the functions in code when possible
#define NxListTapeFilesCallbackFunction	  NxCoreCallbackTapeList
#define NxStdCallbackFunction			  NxCoreCallback
#define NxCoreAPIListTapeFiles			  NxCoreListTapes
#define NxCoreAPIProcessOneTape			  NxCoreProcessTape
#define NxCoreAPIIterateSymbols			  NxCoreSpinSymbols
#define NxCoreAPIPriceToDouble			  NxCorePriceToDouble
#define NxCoreAPIFormatPrice			  NxCorePriceFormat
#define NxCoreAPIConvertPriceType		  NxCorePriceConvert
#define NxCoreAPIDateFromYMD			  NxCoreDateFromYMD
#define NxCoreAPIDateFromNDays			  NxCoreDateFromNDays
#define NxCoreAPILookupString			  NxCoreGetDefinedString
#define NxCoreAPIStateGetLastTrade		  NxCoreStateGetLastTrade
#define NxCoreAPIStateGetExgQuotes		  NxCoreStateGetExgQuotes
#define NxCoreAPIStateGetMMQuotes		  NxCoreStateGetMMQuotes
#define NxCoreAPIGetTapeName			  NxCoreGetTapeName
#define NxCoreAPISaveTapeState			  NxCoreSaveState
#define NxCoreAPISignalWaitEvent		  NxCoreSignalWaitEvent
#endif

/*******************************************************************/
/******************** Function protoypes ***************************/
/*******************************************************************/

/////////////////////////////////////////////////////////////////////
// function prototypes for callback functions passed as parameters //
/////////////////////////////////////////////////////////////////////
typedef int			  (__stdcall *NxCoreCallback)		  (const NxCoreSystem* pNxCoreSys,const NxCoreMessage* pNxCoreMsg);
typedef int			  (__stdcall *NxCoreCallbackTapeList) (void* pYourParam,const NxCoreTapeFile* pNcTF);
typedef int			  (__stdcall *NxCoreCallbackAPIList)  (void* pYourParam,const NxCoreAPIDLLFile* pNcTF);


/////////////////////////////////////////////////////////////////////
/////////////////// main functions   ////////////////////////////////
/////////////////////////////////////////////////////////////////////
typedef int			  (__stdcall *NxCoreProcessTape)	  (const char* pszFilename,const char* pBaseMemory,unsigned int controlFlags,int UserData,NxCoreCallback stdcallback);
typedef int			  (__stdcall *NxCoreListTapes)		  (unsigned int controlFlags,NxCoreCallbackTapeList stdcallback,void* pYourParam);
typedef int			  (__stdcall *NxCoreListAPIDLLs)	  (unsigned int controlFlags,NxCoreCallbackAPIList stdcallback,void* pUserParam);
typedef int			  (__stdcall *NxCoreSpinSymbols)	  (unsigned short ListedExg,unsigned int controlFlags,unsigned int spinID,NxCoreCallback tempstdcallback/*=0*/,int tempUserData/*=0*/);// specify (unsigned short)-1 for all exchanges
typedef int			  (__stdcall *NxCoreSaveState)		  (const char* szStateFilename,unsigned int controlFlags);
typedef NxCoreCallback(__stdcall *NxCoreSetCallback)	  (NxCoreCallback stdcallbackNew);

/////////////////////////////////////////////////////////////////////
///// state functions -- in memory values as of last update  ////////
/////////////////////////////////////////////////////////////////////
typedef int			  (__stdcall *NxCoreStateGetLastTrade)(NxCoreStateTrade*	  pStateTrade,	  NxString* pnxsSymOrCtc/*=0*/);
typedef int			  (__stdcall *NxCoreStateGetExgQuotes)(NxCoreStateExgQuotes*  pStateExgQuotes,NxString* pnxsSymOrCtc/*=0*/);
typedef int			  (__stdcall *NxCoreStateGetMMQuotes) (unsigned short ReportingExg,NxCoreStateMMQuotes* pStateMMQuotes,NxString* pnxsSymOrCtc/*=0*/);

typedef int			  (__stdcall *NxCoreGetStateData)	  (char* pBuffer,int bufferSize,int stateType,int param1,int param2,NxString* pnxsSymOrCtc);



/////////////////////////////////////////////////////////////////////
// conversion functions: can be called from any thread //////////////
/////////////////////////////////////////////////////////////////////
typedef int			  (__stdcall *NxCorePriceConvert)	  (int lPrice,unsigned char PriceType,unsigned char PriceTypeNew);
typedef int			  (__stdcall *NxCorePriceFormat)	  (char* szBuff,int lPrice,unsigned char PriceType,int expandWidth/*=0*/,bool bCommas/*=false*/);
typedef double		  (__stdcall *NxCorePriceToDouble)	  (int lPrice,unsigned char PriceType);
typedef void		  (__stdcall *NxCoreDateFromNDays)	  (NxDate* pnxDate);
typedef void		  (__stdcall *NxCoreDateFromYMD)	  (NxDate* pnxDate);

/////////////////////////////////////////////////////////////////////
//////////////////// miscellaneous functions ////////////////////////
/////////////////////////////////////////////////////////////////////
typedef unsigned int  (__stdcall *NxCoreAPIVersion)		  (void);
typedef const char*	  (__stdcall *NxCoreGetDefinedString) (int ixTable,int ixString);
typedef int			  (__stdcall *NxCoreGetTapeName)	  (char* szBuffTapename,int nBufferBytes);
typedef int 		  (__stdcall *NxCoreSignalWaitEvent)  (const NxCoreSystem* pNxCoreSystem,bool bAlwaysSignal);
typedef int			  (__stdcall *NxCoreGetSymbolAtom32)  (void);

/////////////////////////////////////////////////////////////////////
/////////////////// Exported Function Names	/////////////////////////
/////////////////////////////////////////////////////////////////////
#define cszNxCoreAPIVersion			"sNxCoreAPIVersion"			
#define cszNxCoreProcessTape		"sNxCoreProcessTape"			// xNxProcessOneTape
#define cszNxCoreListTapes			"sNxCoreListTapes"				// xNxListTapes
#define cszNxCoreListAPIDLLs		"sNxCoreListAPIDLLs"
#define cszNxCoreSpinSymbols		"sNxCoreSpinSymbols"			// "xNxIterateSymbols"
#define cszNxCoreSaveState			"sNxCoreSaveState"
#define cszNxCoreStateGetExgQuotes	"sNxCoreStateGetExgQuotes"		// "xNxStateGetExgQuotes"
#define cszNxCoreStateGetMMQuotes	"sNxCoreStateGetMMQuotes"		// "xNxStateGetMMQuotes"
#define cszNxCoreStateGetLastTrade	"sNxCoreStateGetLastTrade"		// "xNxStateGetLastTrade"
#define cszNxCoreGetStateData		"sNxCoreGetStateData"			
#define cszNxCorePriceConvert		"sNxCorePriceConvert"			// "xNxConvertPriceType"
#define cszNxCorePriceFormat		"sNxCorePriceFormat"			// "xNxFormatPrice"
#define cszNxCorePriceToDouble		"sNxCorePriceToDouble"			// "xNxPriceToDouble"
#define cszNxCoreDateFromNDays		"sNxCoreDateFromNDays"			// "xNxCoreDateFromNDays"
#define cszNxCoreDateFromYMD		"sNxCoreDateFromYMD"			// "xNxCoreDateFromYMD"
#define cszNxCoreGetDefinedString	"sNxCoreGetDefinedString"		// "xNxLookupString"
#define cszNxCoreGetTapeName		"sNxCoreGetTapeName"			// "xNxCoreGetTapeName"
#define cszNxCoreSignalWaitEvent	"sNxCoreSignalWaitEvent"		// "xNxCoreSignalWaitEvent"
#define cszNxCoreSetCallback		"sNxCoreSetCallback"		
#define cszNxCoreGetSymbolAtom32	"sNxCoreGetSymbolAtom32"


#endif	//NxCoreAPI_h
