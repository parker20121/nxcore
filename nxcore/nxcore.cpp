#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>

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
	outFiles.tradeStream << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	outFiles.tradeStream << setfill('0') << setw(2) << (int)d.Month << "-";
	outFiles.tradeStream << setfill('0') << setw(2) << (int)d.Day << "\t";
	outFiles.tradeStream << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	outFiles.tradeStream << setfill('0') << setw(2) << (int)t.Minute << ":";
	outFiles.tradeStream << setfill('0') << setw(2) << (int)t.Second << ".";
	outFiles.tradeStream << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	outFiles.tradeStream << (int)t.TimeZone << "\t";							// System Time Zone
	outFiles.tradeStream << (int)d.DSTIndicator << "\t";						// DST Indicator
	outFiles.tradeStream << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	outFiles.tradeStream << (int)d.DayOfWeek << "\t";							// Day of Week
	outFiles.tradeStream << (int)d.DayOfYear << "\t";							// Day of Year
	outFiles.tradeStream << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	outFiles.tradeStream << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	outFiles.tradeStream << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	outFiles.tradeStream << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	outFiles.tradeStream << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	outFiles.tradeStream << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	outFiles.tradeStream << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	outFiles.tradeStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	outFiles.tradeStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	outFiles.tradeStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	outFiles.tradeStream << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	outFiles.tradeStream << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	outFiles.tradeStream << symbol << "\t";										// Symbol
	outFiles.tradeStream << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//outFiles.tradeStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	outFiles.tradeStream << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//outFiles.tradeStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	outFiles.tradeStream << (int)ch.SessionID << "\t";							// Session ID
	outFiles.tradeStream << (int)nt.PriceFlags << "\t";							// Trade Price Flags
	outFiles.tradeStream << (int)nt.ConditionFlags << "\t";						// Trade ConditionFlag
	outFiles.tradeStream << (int)nt.TradeCondition << "\t";						// Trade Condition Index
	//outFiles.tradeStream << nxCoreClass.GetDefinedString(NxST_TRADECONDITION, nt.TradeCondition) << "\t";	// Trade Condition
	outFiles.tradeStream << (int)nt.VolumeType << "\t";							// Trade Volume Type
	outFiles.tradeStream << nt.BATECode << "\t";								// Trade BATE Code 
	outFiles.tradeStream << nt.Size << "\t";									// Trade Size
	outFiles.tradeStream << nt.ExgSequence << "\t";								// Trade ExgSequence
	outFiles.tradeStream << nt.RecordsBack << "\t";								// Trade RecordsBack
	outFiles.tradeStream << nt.TotalVolume << "\t";								// Trade Total Volume
	outFiles.tradeStream << nt.TickVolume << "\t";								// Trade Tick Volume
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Price, nt.PriceType) << "\t";	// Trade Price
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Open, nt.PriceType) << "\t";	// Trade Open
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.High, nt.PriceType) << "\t";	// Trade High
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Low, nt.PriceType) << "\t";	// Trade Low
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Last, nt.PriceType) << "\t";	// Trade Last
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.Tick, nt.PriceType) << "\t";	// Trade Tick
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.NetChange, nt.PriceType) << "\t";	// Trade Net Change
	outFiles.tradeStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(nt.nxAnalysis.FilterThreshold, nt.PriceType) << "\t";	// Analysis Filter Threshold
	outFiles.tradeStream << (int)nt.nxAnalysis.Filtered << "\t";				// Analysis Filtered Bool
	outFiles.tradeStream << (int)nt.nxAnalysis.FilterLevel << "\t";				// Analysis Filter Level
	outFiles.tradeStream << (int)nt.nxAnalysis.SigHiLoType << "\t";				// Analysis SigHiLo Type
	outFiles.tradeStream << nt.nxAnalysis.SigHiLoSeconds << "\t";				// Analysis SigHiLo Seconds
	outFiles.tradeStream << nt.nxAnalysis.QteMatchDistanceRGN << "\t";			// Analysis QteMatchDistanceRGN
	outFiles.tradeStream << nt.nxAnalysis.QteMatchDistanceBBO << "\t";			// Analysis QteMatchDistanceBBO
	outFiles.tradeStream << (int)nt.nxAnalysis.QteMatchFlagsBBO << "\t";		// Analysis QteMatchFlagsBBO
	outFiles.tradeStream << (int)nt.nxAnalysis.QteMatchFlagsRGN << "\t";		// Analysis QteMatchFlagRGN
	outFiles.tradeStream << (int)nt.nxAnalysis.QteMatchTypeBBO << "\t";			// Analysis QteMatchTypeBBO
	outFiles.tradeStream << (int)nt.nxAnalysis.QteMatchTypeRGN << "\n";	       // Analysis QteMatchTypeRGN

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
	outFiles.qExgStream << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	outFiles.qExgStream << setfill('0') << setw(2) << (int)d.Month << "-";
	outFiles.qExgStream << setfill('0') << setw(2) << (int)d.Day << "\t";
	outFiles.qExgStream << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	outFiles.qExgStream << setfill('0') << setw(2) << (int)t.Minute << ":";
	outFiles.qExgStream << setfill('0') << setw(2) << (int)t.Second << ".";
	outFiles.qExgStream << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	outFiles.qExgStream << (int)t.TimeZone << "\t";							// System Time Zone
	outFiles.qExgStream << (int)d.DSTIndicator << "\t";						// DST Indicator
	outFiles.qExgStream << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	outFiles.qExgStream << (int)d.DayOfWeek << "\t";							// Day of Week
	outFiles.qExgStream << (int)d.DayOfYear << "\t";							// Day of Year
	outFiles.qExgStream << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	outFiles.qExgStream << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	outFiles.qExgStream << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	outFiles.qExgStream << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	outFiles.qExgStream << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	outFiles.qExgStream << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	outFiles.qExgStream << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	outFiles.qExgStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	outFiles.qExgStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	outFiles.qExgStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	outFiles.qExgStream << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	outFiles.qExgStream << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	outFiles.qExgStream << symbol << "\t";										// Symbol
	outFiles.qExgStream << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	outFiles.qExgStream << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	outFiles.qExgStream << (int)ch.SessionID << "\t";							// Session ID
	//outFiles.qExgStream << (int)cq.AskPrice << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPrice, cq.PriceType) << "\t";
	//outFiles.qExgStream << (int)cq.AskPriceChange << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPriceChange, cq.PriceType) << "\t";
	outFiles.qExgStream << (int)cq.AskSize << "\t";
	outFiles.qExgStream << (int)cq.AskSizeChange << "\t";
	//outFiles.qExgStream << (int)cq.BidPrice << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPrice, cq.PriceType) << "\t";
	//outFiles.qExgStream << (int)cq.BidPriceChange << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPriceChange, cq.PriceType) << "\t";
	outFiles.qExgStream << (int)cq.BidSize << "\t";
	outFiles.qExgStream << (int)cq.BidSizeChange << "\t";
	outFiles.qExgStream << (int)cq.NasdaqBidTick << "\t";
	outFiles.qExgStream << (int)cq.PriceType << "\t";
	outFiles.qExgStream << (int)cq.QuoteCondition << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	outFiles.qExgStream << (int)cq.Refresh << "\t";
	outFiles.qExgStream << (int)eq.BBOChangeFlags << "\t";
	outFiles.qExgStream << (int)eq.BestAskCondition << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	outFiles.qExgStream << (int)eq.BestAskExg << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.BestAskExg) << "\t";
	//outFiles.qExgStream << (int)eq.BestAskPrice << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestAskPrice, cq.PriceType) << "\t";
	//outFiles.qExgStream << (int)eq.BestAskPriceChange << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestAskPriceChange, cq.PriceType) << "\t";
	outFiles.qExgStream << (int)eq.BestAskSize << "\t";
	outFiles.qExgStream << (int)eq.BestAskSizeChange << "\t";
	outFiles.qExgStream << (int)eq.BestBidCondition << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, eq.BestBidCondition) << "\t";
	outFiles.qExgStream << (int)eq.BestBidExg << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.BestBidExg) << "\t";
	//outFiles.qExgStream << (int)eq.BestBidPrice << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestBidPrice, cq.PriceType) << "\t";
	//outFiles.qExgStream << (int)eq.BestBidPriceChange << "\t";
	outFiles.qExgStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(eq.BestBidPriceChange, cq.PriceType) << "\t";
	outFiles.qExgStream << (int)eq.BestBidSize << "\t";
	outFiles.qExgStream << (int)eq.BestBidSizeChange << "\t";
	outFiles.qExgStream << (int)eq.ClosingQuoteFlag << "\t";
	outFiles.qExgStream << (int)eq.PrevBestAskExg << "\t";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.PrevBestAskExg) << "\t";
	outFiles.qExgStream << (int)eq.PrevBestBidExg << "\n";
	//outFiles.qExgStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, eq.PrevBestBidExg) << "\n";

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
	outFiles.qMmStream << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	outFiles.qMmStream << setfill('0') << setw(2) << (int)d.Month << "-";
	outFiles.qMmStream << setfill('0') << setw(2) << (int)d.Day << "\t";
	outFiles.qMmStream << setfill('0') << setw(2) << (int)t.Hour << ":";		// System Time (HH:MM:SS.NNN)
	outFiles.qMmStream << setfill('0') << setw(2) << (int)t.Minute << ":";
	outFiles.qMmStream << setfill('0') << setw(2) << (int)t.Second << ".";
	outFiles.qMmStream << setfill('0') << setw(3) << (int)t.Millisecond << "\t";
	outFiles.qMmStream << (int)t.TimeZone << "\t";							// System Time Zone
	outFiles.qMmStream << (int)d.DSTIndicator << "\t";						// DST Indicator
	outFiles.qMmStream << (int)d.NDays << "\t";								// Number of Days since 1883-01-01
	outFiles.qMmStream << (int)d.DayOfWeek << "\t";							// Day of Week
	outFiles.qMmStream << (int)d.DayOfYear << "\t";							// Day of Year
	outFiles.qMmStream << (int)ch.nxSessionDate.Year << "-";					// nxSessionDate
	outFiles.qMmStream << setfill('0') << setw(2) << (int)ch.nxSessionDate.Month;
	outFiles.qMmStream << "-" << setfill('0') << setw(2) << (int)ch.nxSessionDate.Day << "\t";
	outFiles.qMmStream << (int)ch.nxSessionDate.DSTIndicator << "\t";			// Session DST Indicator
	outFiles.qMmStream << (int)ch.nxSessionDate.NDays << "\t";				// Session Number of Days since 1883-01-01
	outFiles.qMmStream << (int)ch.nxSessionDate.DayOfWeek << "\t";			// Session Day of Week
	outFiles.qMmStream << (int)ch.nxSessionDate.DayOfYear << "\t";			// Session Day of Year
	outFiles.qMmStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Hour << ":"; // nxExgTimestamp
	outFiles.qMmStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Minute << ":";
	outFiles.qMmStream << setfill('0') << setw(2) << (int)ch.nxExgTimestamp.Second << ".";
	outFiles.qMmStream << setfill('0') << setw(3) << (int)ch.nxExgTimestamp.Millisecond << "\t";
	outFiles.qMmStream << (int)ch.nxExgTimestamp.TimeZone << "\t";			// Exchange Timestamp Time Zone
	outFiles.qMmStream << symbol << "\t";										// Symbol
	outFiles.qMmStream << (int)ch.ListedExg << "\t";							// Listed Exchange Index
	//outFiles.qMmStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ListedExg) << "\t";		// Listed Exchange
	outFiles.qMmStream << (int)ch.ReportingExg << "\t";						// Reporting Exchange Index
	//outFiles.qMmStream << nxCoreClass.GetDefinedString(NxST_EXCHANGE, ch.ReportingExg) << "\t";	// Reporting Exchange
	outFiles.qMmStream << (int)ch.SessionID << "\t";							// Session ID
	//outFiles.qMmStream << (int)cq.AskPrice << "\t";
	outFiles.qMmStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPrice, cq.PriceType) << "\t";
	//outFiles.qMmStream << (int)cq.AskPriceChange << "\t";
	outFiles.qMmStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble((int)cq.AskPriceChange, cq.PriceType) << "\t";
	outFiles.qMmStream << (int)cq.AskSize << "\t";
	outFiles.qMmStream << (int)cq.AskSizeChange << "\t";
	//outFiles.qMmStream << (int)cq.BidPrice << "\t";
	outFiles.qMmStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPrice, cq.PriceType) << "\t";
	//outFiles.qMmStream << (int)cq.BidPriceChange << "\t";
	outFiles.qMmStream << fixed << setprecision(3) << nxCoreClass.PriceToDouble(cq.BidPriceChange, cq.PriceType) << "\t";
	outFiles.qMmStream << (int)cq.BidSize << "\t";
	outFiles.qMmStream << (int)cq.BidSizeChange << "\t";
	outFiles.qMmStream << (int)cq.NasdaqBidTick << "\t";
	outFiles.qMmStream << (int)cq.PriceType << "\t";
	outFiles.qMmStream << (int)cq.QuoteCondition << "\t";
	//outFiles.qMmStream << nxCoreClass.GetDefinedString(NxST_QUOTECONDITION, cq.QuoteCondition) << "\t";
	outFiles.qMmStream << (int)cq.Refresh << "\t";
	outFiles.qMmStream << (int)mm.MarketMakerType << "\t";
	outFiles.qMmStream << mm.pnxStringMarketMaker->String << "\t";
	outFiles.qMmStream << (int)mm.QuoteType << "\n";

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
	outFiles.symStream << (int)d.Year << "-";									// System Date (yyyy-mm-dd)
	outFiles.symStream << setfill('0') << setw(2) << (int)d.Month << "-";
	outFiles.symStream << setfill('0') << setw(2) << (int)d.Day << "\t";
	outFiles.symStream << outString << "\n";
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
	_splitpath(argv[1], drive, dir, fname, ext);
	sprintf(outputdir, "%s%s\\processed\\", drive, dir);
	sprintf(outputInfo.tradedir, "%s\\trade", outputdir);
	sprintf(outputInfo.exgquotedir, "%s\\exgquote", outputdir);
	sprintf(outputInfo.mmquotedir, "%s\\mmquote", outputdir);
	sprintf(outputInfo.symboldir, "%s\\symbolchange", outputdir);

	_mkdir(outputdir);
	_mkdir(outputInfo.tradedir);
	_mkdir(outputInfo.exgquotedir);
	_mkdir(outputInfo.mmquotedir);
	_mkdir(outputInfo.symboldir);

	// Create Output File Names
	sprintf(outputInfo.tradefname, "%s\\%s_TRADES.csv", outputInfo.tradedir, fname);
	sprintf(outputInfo.exgquotefname, "%s\\%s_EXGQUOTE.csv", outputInfo.exgquotedir, fname);
	sprintf(outputInfo.mmquotefname, "%s\\%s_MMQUOTE.csv", outputInfo.mmquotedir, fname);
	sprintf(outputInfo.symbolfname, "%s\\%s_SYMBOLCHANGE.csv", outputInfo.symboldir, fname);

	// Open File Handles
	outFiles.tradeStream.open(outputInfo.tradefname,std::ios::trunc);
	outFiles.qExgStream.open(outputInfo.exgquotefname, std::ios::trunc);
	outFiles.qMmStream.open(outputInfo.mmquotefname, std::ios::trunc);
	outFiles.symStream.open(outputInfo.symbolfname, std::ios::trunc);

	printf("Directory: %s%s\tTape: %s%s\n", drive, dir, fname, ext);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK+NxCF_EXCLUDE_QUOTES+NxCF_EXCLUDE_QUOTES2+NxCF_EXCLUDE_OPRA, 0, OnNxCoreCallback);
	nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES + NxCF_EXCLUDE_QUOTES2, 0, OnNxCoreCallback);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES2, 0, OnNxCoreCallback);
	//nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK + NxCF_EXCLUDE_QUOTES, 0, OnNxCoreCallback);
	// Close File Handles
	outFiles.tradeStream.close();
	outFiles.qExgStream.close();
	outFiles.qMmStream.close();
	outFiles.symStream.close();

	return 0;
}
