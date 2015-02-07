#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "NxCoreAPI.h"
#include "NxCoreAPI_class.h"

using namespace std;

static NxCoreClass nxCoreClass;

struct OUTPUT { // Declare OUTPUT struct type
	// Trades
	// Trades
	char tradedir[_MAX_DIR];
	char tradefname[_MAX_FNAME];
	// Exchange Quotes
	char exgquotedir[_MAX_DIR];
	char exgquotefname[_MAX_FNAME];
	// MarketMaker Quotes
	char mmquotedir[_MAX_DIR];
	char mmquotefname[_MAX_FNAME];
	// Symbol Change
	char symboldir[_MAX_DIR];
	char symbolfname[_MAX_FNAME];
};

struct OUTPUTFILES {
	ofstream tradeStream;
	ofstream qExgStream;
	ofstream qMmStream;
	ofstream symStream;
};
OUTPUTFILES outFiles = {};

void getSymbol(const NxCoreMessage* pNxCoreMsg, char *Symbol)
{
	// Is this a valid option?
	if ((pNxCoreMsg->coreHeader.pnxStringSymbol->String[0] == 'o') && (pNxCoreMsg->coreHeader.pnxOptionHdr))
	{
		// If pnxsDateAndStrike->String[1] == ' ', then this symbol is in new OSI format.
		if (pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[1] == ' ')
		{
			sprintf(Symbol, "%s,(opt_new),%04d-%02d-%02d,%c,%08d",
				pNxCoreMsg->coreHeader.pnxStringSymbol->String,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Year,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Month,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Day,
				(pNxCoreMsg->coreHeader.pnxOptionHdr->PutCall == 0) ? 'C' : 'P',
				pNxCoreMsg->coreHeader.pnxOptionHdr->strikePrice);
		}
		// Otherwise the symbol is in old OPRA format.
		else
		{
			sprintf(Symbol, "%s,(opt_old),%c,%c",
				pNxCoreMsg->coreHeader.pnxStringSymbol->String,
				pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[0],
				pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[1]);
		}
	}
	// Not an option, just copy the symbol
	else
	{
		sprintf(Symbol, "%s,(non_opt)", pNxCoreMsg->coreHeader.pnxStringSymbol->String);
	}
}
inline const char* symbol(char* sym, NxString* nx, NxOptionHdr* oh)
{
	// If an option
	if (oh)
	{
		// If pnxsDateAndStrike->String[1] == ' ', then this symbol is in new OSI format. 	
		if (oh->pnxsDateAndStrike->String[1] == ' ')
		{
			sprintf(sym, "%s,(opt_new),%04d-%02d-%02d,%c,%08d",
				nx->String,
				oh->nxExpirationDate.Year,
				oh->nxExpirationDate.Month,
				oh->nxExpirationDate.Day,
				(oh->PutCall == 0) ? 'C' : 'P',
				oh->strikePrice);
		}
		// Otherwise the symbol is in old OPRA format.
		else
		{
			sprintf(sym, "%s,(opt_old),%c,%c",
				nx->String,
				oh->pnxsDateAndStrike->String[0],
				oh->pnxsDateAndStrike->String[1]);
		}
	}
	// otherwise a non-option
	else
	{
		sprintf(sym, "%s,(non_opt)", nx->String);
	}

	return sym;
}
int processNxCoreTrade(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	const NxCoreTrade& nt = pNxCoreMsg->coreData.Trade;
	const NxCoreHeader& ch = pNxCoreMsg->coreHeader;
	const NxTime&       t = pNxCoreSys->nxTime;
	const NxDate& d = pNxCoreSys->nxDate;
	char symbol[128];
	getSymbol(pNxCoreMsg, symbol);

	//Output to file
	// Columns: System Date, System Time, System Time Zone, DST Indicator, Number of Days since 1883-01-01,
	//			Day of Week, Day of Year, nxSessionDate, Session DST Indicator, 
	//			Session Number of Days since 1883-01-01, Session Day of Week, Session Day of Year,
	//			nxExgTimeStamp, Exchange Timestamp Time Zone, Symbol, Listed Exchange Index, Listed Exchange,
	//			Reporting Exchange Index, Reporting Exchange, Session ID, Trade Price Flags, Trade ConditionFlag,
	//			Trade Condition Index, Trade Condition, Trade Volume Type, Trade BATE Code, Trade Size,
	//			Trade ExgSequence, Trade RecordsBack, Trade Total Volume, Trade Tick Volume, Trade Price,
	//			Trade Open, Trade High, Trade Low, Trade Last, Trade Tick, Trade Net Change,
	//			Analysis Filter Threshold, Analysis Filtered Bool, Analysis Filter Level, Analysis SigHiLo Type,
	//			Analysis SigHiLo Seconds, Analysis QteMatchDistanceRGN, Analysis QteMatchDistanceBBO,
	//			Analysis QteMatchFlagsBBO, Analysis QteMatchFlagRGN, Analysis QteMatchTypeBBO,
	//			Analysis QteMatchTypeRGN
	cout << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	cout << setfill('0') << setw(2) << (int)d.Month << "-";
	cout << setfill('0') << setw(2) << (int)d.Day << "\t";
	cout << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	cout << setfill('0') << setw(2) << (int)t.Minute << ":";
	cout << setfill('0') << setw(2) << (int)t.Second << ".";
	cout << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	cout << (int)t.TimeZone << "\t";							// System Time Zone
	cout << (int)d.DSTIndicator << "\t";						// DST Indicator
	cout << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	cout << (int)d.DayOfWeek << "\t";							// Day of Week
	cout << (int)d.DayOfYear << "\t";							// Day of Year
	cout << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	cout << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	cout << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	cout << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	cout << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	cout << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	cout << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	cout << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	cout << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	cout << symbol << "\t";										// Symbol
	cout << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	cout << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	cout << (int)ch.SessionID << "\t";							// Session ID
	cout << (int)nt.PriceFlags << "\t";							// Trade Price Flags
	cout << (int)nt.ConditionFlags << "\t";						// Trade ConditionFlag
	cout << (int)nt.TradeCondition << "\t";						// Trade Condition Index
	//cout << nxCoreClass.GetDefinedString(NxST_TRADECONDITION, nt.TradeCondition) << "\t";	// Trade Condition
	cout << (int)nt.VolumeType << "\t";							// Trade Volume Type
	cout << nt.BATECode << "\t";								// Trade BATE Code 
	cout << nt.Size << "\t";									// Trade Size
	cout << nt.ExgSequence << "\t";								// Trade ExgSequence
	cout << nt.RecordsBack << "\t";								// Trade RecordsBack
	cout << nt.TotalVolume << "\t";								// Trade Total Volume
	cout << nt.TickVolume << "\t";								// Trade Tick Volume
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Price, nt.PriceType) << "\t";	// Trade Price
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Open, nt.PriceType) << "\t";	// Trade Open
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.High, nt.PriceType) << "\t";	// Trade High
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Low, nt.PriceType) << "\t";	// Trade Low
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Last, nt.PriceType) << "\t";	// Trade Last
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Tick, nt.PriceType) << "\t";	// Trade Tick
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.NetChange, nt.PriceType) << "\t";	// Trade Net Change
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.nxAnalysis.FilterThreshold, nt.PriceType) << "\t";	// Analysis Filter Threshold
	cout << (int)nt.nxAnalysis.Filtered << "\t";				// Analysis Filtered Bool
	cout << (int)nt.nxAnalysis.FilterLevel << "\t";				// Analysis Filter Level
	cout << (int)nt.nxAnalysis.SigHiLoType << "\t";				// Analysis SigHiLo Type
	cout << nt.nxAnalysis.SigHiLoSeconds << "\t";				// Analysis SigHiLo Seconds
	cout << nt.nxAnalysis.QteMatchDistanceRGN << "\t";			// Analysis QteMatchDistanceRGN
	cout << nt.nxAnalysis.QteMatchDistanceBBO << "\t";			// Analysis QteMatchDistanceBBO
	cout << (int)nt.nxAnalysis.QteMatchFlagsBBO << "\t";		// Analysis QteMatchFlagsBBO
	cout << (int)nt.nxAnalysis.QteMatchFlagsRGN << "\t";		// Analysis QteMatchFlagRGN
	cout << (int)nt.nxAnalysis.QteMatchTypeBBO << "\t";			// Analysis QteMatchTypeBBO
	cout << (int)nt.nxAnalysis.QteMatchTypeRGN << "\n";	       // Analysis QteMatchTypeRGN

	return NxCALLBACKRETURN_CONTINUE;
}
int processNxCoreExgQuote(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	const NxCoreHeader&   ch = pNxCoreMsg->coreHeader;
	const NxCoreExgQuote& eq = pNxCoreMsg->coreData.ExgQuote;
	const NxCoreQuote&    cq = eq.coreQuote;
	const NxTime& t = pNxCoreSys->nxTime;
	const NxDate& d = pNxCoreSys->nxDate;
	char symbol[128];
	getSymbol(pNxCoreMsg, symbol);

	// Output
	cout << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	cout << setfill('0') << setw(2) << (int)d.Month << "-";
	cout << setfill('0') << setw(2) << (int)d.Day << "\t";
	cout << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	cout << setfill('0') << setw(2) << (int)t.Minute << ":";
	cout << setfill('0') << setw(2) << (int)t.Second << ".";
	cout << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	cout << (int)t.TimeZone << "\t";							// System Time Zone
	cout << (int)d.DSTIndicator << "\t";						// DST Indicator
	cout << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	cout << (int)d.DayOfWeek << "\t";							// Day of Week
	cout << (int)d.DayOfYear << "\t";							// Day of Year
	cout << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	cout << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	cout << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	cout << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	cout << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	cout << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	cout << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	cout << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	cout << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	cout << symbol << "\t";										// Symbol
	cout << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	cout << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	cout << (int)ch.SessionID << "\t";							// Session ID
	//cout << (int)cq.AskPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPrice, cq.PriceType) << "\t";
	//cout << (int)cq.AskPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPriceChange, cq.PriceType) << "\t";
	cout << (int)cq.AskSize << "\t";
	cout << (int)cq.AskSizeChange << "\t";
	//cout << (int)cq.BidPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPrice, cq.PriceType) << "\t";
	//cout << (int)cq.BidPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPriceChange, cq.PriceType) << "\t";
	cout << (int)cq.BidSize << "\t";
	cout << (int)cq.BidSizeChange << "\t";
	cout << (int)cq.NasdaqBidTick << "\t";
	cout << (int)cq.PriceType << "\t";
	cout << (int)cq.QuoteCondition << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	cout << (int)cq.Refresh << "\t";
	cout << (int)eq.BBOChangeFlags << "\t";
	cout << (int)eq.BestAskCondition << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	cout << (int)eq.BestAskExg << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.BestAskExg) << "\t";
	//cout << (int)eq.BestAskPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestAskPrice, cq.PriceType) << "\t";
	//cout << (int)eq.BestAskPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestAskPriceChange, cq.PriceType) << "\t";
	cout << (int)eq.BestAskSize << "\t";
	cout << (int)eq.BestAskSizeChange << "\t";
	cout << (int)eq.BestBidCondition << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, eq.BestBidCondition) << "\t";
	cout << (int)eq.BestBidExg << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.BestBidExg) << "\t";
	//cout << (int)eq.BestBidPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestBidPrice, cq.PriceType) << "\t";
	//cout << (int)eq.BestBidPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestBidPriceChange, cq.PriceType) << "\t";
	cout << (int)eq.BestBidSize << "\t";
	cout << (int)eq.BestBidSizeChange << "\t";
	cout << (int)eq.ClosingQuoteFlag << "\t";
	cout << (int)eq.PrevBestAskExg << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.PrevBestAskExg) << "\t";
	cout << (int)eq.PrevBestBidExg << "\n";
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.PrevBestBidExg) << "\n";

	return NxCALLBACKRETURN_CONTINUE;
}
int processNxCoreMMQuote(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	const NxCoreHeader& ch = pNxCoreMsg->coreHeader;
	const NxCoreMMQuote& mm = pNxCoreMsg->coreData.MMQuote;
	const NxCoreQuote& cq = mm.coreQuote;
	const NxTime& t = pNxCoreSys->nxTime;
	const NxDate& d = pNxCoreSys->nxDate;
	char symbol[128];
	getSymbol(pNxCoreMsg, symbol);
	char buf[1024] = { 0 };
	char* p = buf;

	// Output
	cout << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	cout << setfill('0') << setw(2) << (int)d.Month << "-";
	cout << setfill('0') << setw(2) << (int)d.Day << "\t";
	cout << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	cout << setfill('0') << setw(2) << (int)t.Minute << ":";
	cout << setfill('0') << setw(2) << (int)t.Second << ".";
	cout << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	cout << (int)t.TimeZone << "\t";							// System Time Zone
	cout << (int)d.DSTIndicator << "\t";						// DST Indicator
	cout << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	cout << (int)d.DayOfWeek << "\t";							// Day of Week
	cout << (int)d.DayOfYear << "\t";							// Day of Year
	cout << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	cout << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	cout << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	cout << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	cout << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	cout << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	cout << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	cout << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	cout << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	cout << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	cout << symbol << "\t";										// Symbol
	cout << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	cout << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//cout << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	cout << (int)ch.SessionID << "\t";							// Session ID
	//cout << (int)cq.AskPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPrice, cq.PriceType) << "\t";
	//cout << (int)cq.AskPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPriceChange, cq.PriceType) << "\t";
	cout << (int)cq.AskSize << "\t";
	cout << (int)cq.AskSizeChange << "\t";
	//cout << (int)cq.BidPrice << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPrice, cq.PriceType) << "\t";
	//cout << (int)cq.BidPriceChange << "\t";
	cout << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPriceChange, cq.PriceType) << "\t";
	cout << (int)cq.BidSize << "\t";
	cout << (int)cq.BidSizeChange << "\t";
	cout << (int)cq.NasdaqBidTick << "\t";
	cout << (int)cq.PriceType << "\t";
	cout << (int)cq.QuoteCondition << "\t";
	//cout << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	cout << (int)cq.Refresh << "\t";
	cout << (int)mm.MarketMakerType << "\t";
	cout << mm.pnxStringMarketMaker->String << "\t";
	cout << (int)mm.QuoteType << "\n";

	return NxCALLBACKRETURN_CONTINUE;
}
int processNxCoreSymbolChange(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	const NxCoreSymbolChange& sc = pNxCoreMsg->coreData.SymbolChange;
	const NxCoreHeader&       ch = pNxCoreMsg->coreHeader;
	const NxDate& d = pNxCoreSys->nxDate;
	char symN[128];
	char symO[128];
	char outString[250];
	switch (sc.Status)
	{
	case NxSS_ADD:
	{
		sprintf(outString, "%.2d:%.2d:%.2d.%.3d\tADD\t%s\t%ld",
			(int)pNxCoreSys->nxTime.Hour,
			(int)pNxCoreSys->nxTime.Minute,
			(int)pNxCoreSys->nxTime.Second,
			(int)pNxCoreSys->nxTime.Millisecond,
			symbol(symN, ch.pnxStringSymbol, ch.pnxOptionHdr), ch.ListedExg);

		break;
	}
	case NxSS_DEL:
	{
		sprintf(outString, "%.2d:%.2d:%.2d.%.3d\tDEL\t%s\t%ld\n",
			(int)pNxCoreSys->nxTime.Hour,
			(int)pNxCoreSys->nxTime.Minute,
			(int)pNxCoreSys->nxTime.Second,
			(int)pNxCoreSys->nxTime.Millisecond,
			symbol(symO, ch.pnxStringSymbol, ch.pnxOptionHdr), ch.ListedExg);
		break;
	}
	case NxSS_MOD:
	{
		sprintf(outString, "%.2d:%.2d:%.2d.%.3d\tMOD\t%s\t%ld\t%s\t%ld\n",
			(int)pNxCoreSys->nxTime.Hour,
			(int)pNxCoreSys->nxTime.Minute,
			(int)pNxCoreSys->nxTime.Second,
			(int)pNxCoreSys->nxTime.Millisecond,
			symbol(symN, sc.pnxsSymbolOld, sc.pnxOptionHdrOld), sc.ListedExgOld,
			symbol(symO, ch.pnxStringSymbol, ch.pnxOptionHdr), ch.ListedExg);
		break;
	}
	}
	cout << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	cout << setfill('0') << setw(2) << (int)d.Month << "-";
	cout << setfill('0') << setw(2) << (int)d.Day << "\t";
	cout << outString << "\n";
	return NxCALLBACKRETURN_CONTINUE;
}
int __stdcall OnNxCoreCallback(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	switch (pNxCoreMsg->MessageType) {
	case NxMSG_STATUS:       break;
	case NxMSG_EXGQUOTE:     return processNxCoreExgQuote(pNxCoreSys, pNxCoreMsg);
	case NxMSG_MMQUOTE:      return processNxCoreMMQuote(pNxCoreSys, pNxCoreMsg);
	case NxMSG_TRADE:        return processNxCoreTrade(pNxCoreSys, pNxCoreMsg);
	case NxMSG_CATEGORY:     break;
	case NxMSG_SYMBOLCHANGE: return processNxCoreSymbolChange(pNxCoreSys, pNxCoreMsg);
	case NxMSG_SYMBOLSPIN:   break;
	}
	return NxCALLBACKRETURN_CONTINUE;
}

int main(int argc, char** argv)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char outputdir[_MAX_DIR];
	OUTPUT outputInfo;

	if (!nxCoreClass.LoadNxCore("NxCoreAPI.dll") &&
		!nxCoreClass.LoadNxCore("C:\\Program Files\\Nanex\\NxCoreAPI\\NxCoreAPI.dll"))
	{
		fprintf(stderr, "Can't find NxCoreAPI.dll\n");
		return -1;
	}

	// Create Output Directories
	//_splitpath(argv[1], drive, dir, fname, ext);
	//sprintf(outputdir, "%s%s\\processed\\", drive, dir);
	//sprintf(outputInfo.tradedir, "%s\\trade", outputdir);
	//sprintf(outputInfo.exgquotedir, "%s\\exgquote", outputdir);
	//sprintf(outputInfo.mmquotedir, "%s\\mmquote", outputdir);
	//sprintf(outputInfo.symboldir, "%s\\symbolchange", outputdir);

	//
	// Leave these commented out since they overwrite the permissions
	// if the directories already exist. We'll assume they're already
	// created by the bash shell scripts.
	//
	//_mkdir(outputdir);
	//_mkdir(outputInfo.tradedir);
	//_mkdir(outputInfo.exgquotedir);
	//_mkdir(outputInfo.mmquotedir);
	//_mkdir(outputInfo.symboldir);

	// Create Output File Names
	//sprintf(outputInfo.tradefname, "%s\\%s_TRADES.csv", outputInfo.tradedir, fname);
	//sprintf(outputInfo.exgquotefname, "%s\\%s_EXGQUOTE.csv", outputInfo.exgquotedir, fname);
	//sprintf(outputInfo.mmquotefname, "%s\\%s_MMQUOTE.csv", outputInfo.mmquotedir, fname);
	//sprintf(outputInfo.symbolfname, "%s\\%s_SYMBOLCHANGE.csv", outputInfo.symboldir, fname);

	// Open File Handles
	//outFiles.tradeStream.open(outputInfo.tradefname,std::ios::app);
	//outFiles.qExgStream.open(outputInfo.exgquotefname, std::ios::app);
	//outFiles.qMmStream.open(outputInfo.mmquotefname, std::ios::app);
	//outFiles.symStream.open(outputInfo.symbolfname, std::ios::app);

	printf("Directory: %s%s\tTape: %s%s\n", drive, dir, fname, ext);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK+NxCF_EXCLUDE_QUOTES+NxCF_EXCLUDE_QUOTES2+NxCF_EXCLUDE_OPRA, 0, OnNxCoreCallback);
	nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES + NxCF_EXCLUDE_QUOTES2, 0, OnNxCoreCallback);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES2, 0, OnNxCoreCallback);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES, 0, OnNxCoreCallback);
	// Close File Handles
	//outFiles.tradeStream.close();
	//outFiles.qExgStream.close();
	//outFiles.qMmStream.close();
	//outFiles.symStream.close();

	return 0;
}
