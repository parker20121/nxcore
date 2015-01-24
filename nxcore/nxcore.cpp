// nxcore.cpp : Defines the entry point for the console application.
//
// Various problems encountered along the way:
//
// Use Multi-Byte Character Set
// http://forum.dtniq.com/index.cfm?page=topic&topicID=1592
//
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include "stdafx.h"
#include "NxCoreAPI_Class.h"

NxCoreClass nxCoreClass;

void output(const NxCategoryField& cf){

	switch (cf.FieldType)
	{
	case NxCFT_UNKNOWN:
	{
		printf("--unknown--");
		break;
	}
	case NxCFT_64BIT:
	{
		printf("%I64d", cf.data.i64Bit);
		break;
	}
	case NxCFT_32BIT:
	{
		printf("%ld", cf.data.i32Bit);
		break;
	}
	case NxCFT_STRINGZ:
	{
		printf("%s", cf.data.StringZ);
		break;
	}
	case NxCFT_DOUBLE:
	{
		printf("%lf", cf.data.Double);
		break;
	}
	case NxCFT_PRICE:
	{
		printf("%.2lf", nxCoreClass.PriceToDouble(cf.data.nxPrice.Price, cf.data.nxPrice.PriceType));
		break;
	}
	case NxCFT_DATE:
	{
		printf("%d%.2d%.2d", (int)cf.data.nxDate.Year, (int)cf.data.nxDate.Month, (int)cf.data.nxDate.Day);
		break;
	}
	case NxCFT_TIME:
	{
		printf("%.2d:%.2d:%.2d.%.3d", (int)cf.data.nxTime.Hour, (int)cf.data.nxTime.Minute, (int)cf.data.nxTime.Second, (int)cf.data.nxTime.Millisecond);
		break;
	}
	case NxCFT_NxSTRING:
	{
		if (cf.data.pnxString)
		{
			const NxString& s = (const NxString&)*cf.data.pnxString;
			printf("'%s'", s.String);
		}
		break;
	}
	case NxCFT_STRING_IDX:
	{
		const StringTableItem& stItem = (const StringTableItem&)cf.data.stringTableItem;
		const char* ss = nxCoreClass.GetDefinedString(stItem.ixTable, stItem.idString);
		if (!ss)
		{
			ss = "***NULL***";
		}
		printf("[%ld:%ld] '%s'", stItem.ixTable, stItem.idString, ss);
		break;
	}
	case NxCFT_STRING_MAP:
	{
		const StringTableItem& stItem = (const StringTableItem&)cf.data.stringTableItem;
		bool printed = false;
		unsigned int id = stItem.idString;
		for (int i = 0; id; ++i, id >>= 1)
		{
			if (id & 1)
			{
				const char* ss = nxCoreClass.GetDefinedString(stItem.ixTable, i);
				if (!ss)
				{
					ss = "***NULL***";
				}
				if (!printed)
				{
					printf("[%ld:%ld] '%s'", stItem.ixTable, i, ss);
					printed = true;
				}
				else
				{
					printf(",[%ld:%ld] '%s'", stItem.ixTable, i, ss);
				}
			}
		}
		break;
	}
	}

}

void categoryDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreCategory &cat = pNxCoreMessage->coreData.Category;

	char option[128] = { 0 };

	if (ch.pnxOptionHdr)
	{
		if (ch.pnxOptionHdr->pnxsDateAndStrike)
		{
			int n = sprintf(option, "Root='%s' Symbol='%s%c%c'",
				ch.pnxStringSymbol->String,
				ch.pnxStringSymbol->String + 1,
				ch.pnxOptionHdr->pnxsDateAndStrike->String[0],
				ch.pnxOptionHdr->pnxsDateAndStrike->String[1]);
			if (ch.pnxOptionHdr->pnxsUnderlying)
			{
				sprintf(option + n, " Underlying='%s'",
					ch.pnxOptionHdr->pnxsUnderlying->String);
			}
		}
	}

		
	printf("category,");                                                                                                                       //Record Type
	printf("%s(%ld),", cat.pnxStringCategory->String, cat.pnxStringCategory->Atom);                                                            //Category
	printf("%.2d:%.2d:%.2d,", (int)pNxCoreSys->nxTime.Hour, (int)pNxCoreSys->nxTime.Minute, (int)pNxCoreSys->nxTime.Second);                   //nxTime		
	printf("%s,", ch.pnxStringSymbol->String);                                                                                                 //symbol
	printf("%s,", option);                                                                                                                     //pnxOptionHdr
	printf("%ld,", ch.ListedExg);                                                                                                              //listedExch
	printf("%d%.2d%.2d,", (int)ch.nxSessionDate.Year, (int)ch.nxSessionDate.Month, (int)ch.nxSessionDate.Day);                                 //sessionDate
	printf("%.2d:%.2d:%.2d,", (int)ch.nxExgTimestamp.Hour, (int)ch.nxExgTimestamp.Minute, (int)ch.nxExgTimestamp.Second);                      //time
	printf("%ld,", ch.ReportingExg);                                                                                                           //reportExch
	printf("%ld,", (int)ch.SessionID);                                                                                                         //sessionId
	printf("%ld [%s],", ch.PermissionID, nxCoreClass.GetDefinedString(NxST_PERMID, ch.PermissionID));                                          //permissionId
	//printf("  NFields        : %ld\n", cat.NFields);
	
	int fields = cat.NFields;

	for (int i = 0; i < fields; i++)
	{
		const NxCategoryField& cf = cat.pnxFields[i];

		//printf("%ld|%s|[%s] {%ld} :", i, cf.FieldName, cf.FieldInfo, (int)cf.Set);

		if (!cf.Set)
		{
			//printf("--!Set--");
			//continue;
			printf("NULL");
		}
		else {
			output(cf);
		}

		if (i != fields - 1){
			printf(",");
		}
	}

	printf("\n");

}

void getSymbol(const NxCoreMessage* pNxCoreMsg, char *Symbol)
{
	// Is this a valid option?    
	if ((pNxCoreMsg->coreHeader.pnxStringSymbol->String[0] == 'o') && (pNxCoreMsg->coreHeader.pnxOptionHdr))
	{
		// If pnxsDateAndStrike->String[1] == ' ', then this symbol is in new OSI format. 	
		if (pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[1] == ' ')
		{
			sprintf(Symbol, "%s%02d%02d%02d%c%08d,",
				pNxCoreMsg->coreHeader.pnxStringSymbol->String,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Year - 2000,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Month,
				pNxCoreMsg->coreHeader.pnxOptionHdr->nxExpirationDate.Day,
				(pNxCoreMsg->coreHeader.pnxOptionHdr->PutCall == 0) ? 'C' : 'P',
				pNxCoreMsg->coreHeader.pnxOptionHdr->strikePrice);
		}
		// Otherwise the symbol is in old OPRA format.
		else
		{
			sprintf(Symbol, "%s%c%c",
				pNxCoreMsg->coreHeader.pnxStringSymbol->String,
				pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[0],
				pNxCoreMsg->coreHeader.pnxOptionHdr->pnxsDateAndStrike->String[1]);
		}
	}
	// Not an option, just copy the symbol
	else
	{
		strcpy(Symbol, pNxCoreMsg->coreHeader.pnxStringSymbol->String);
	}
}


void exgquouteDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreExgQuote &quote = pNxCoreMessage->coreData.ExgQuote;

	printf("exgquote\n");

	getSymbol(pNxCoreMessage, symbol);
	
	printf("", quote.BBOChangeFlags);
	printf("", quote.BestAskCondition);
	printf("", quote.BestAskExg);
	printf("", quote.BestAskPrice);
	printf("", quote.BestAskPriceChange);
	printf("", quote.BestAskSize);
	printf("", quote.BestAskSizeChange);
	printf("", quote.BestBidCondition);
	printf("", quote.BestBidExg);
	printf("", quote.BestBidPrice);
	printf("", quote.BestBidPriceChange);
	printf("", quote.BestBidSize);
	printf("", quote.BestBidSizeChange);
	printf("", quote.PrevBestAskExg);
	printf("", quote.PrevBestBidExg);
	printf("\n", quote.ClosingQuoteFlag);

	const NxCoreHeader&   ch = pNxCoreMessage->coreHeader;
	const NxCoreExgQuote& eq = pNxCoreMessage->coreData.ExgQuote;
	const NxCoreQuote&    cq = eq.coreQuote;
	const NxTime& t = pNxCoreSys->nxTime;

	char symbol[23];

	getSymbol(pNxCoreMessage, symbol);

	char buf[1024] = { 0 };
	char* p = buf;

	if (cq.BidPriceChange || cq.BidSizeChange)
	{
		p += sprintf(p, "RGN_bid(%ld @ %.2lf)[%ld] ",
			cq.BidSize,
			nxCoreClass.PriceToDouble(cq.BidPrice, cq.PriceType),
			ch.ReportingExg);
	}
	if (cq.AskPriceChange || cq.AskSizeChange)
	{
		p += sprintf(p, "RGN_ask(%ld @ %.2lf)[%ld] ",
			cq.AskSize,
			nxCoreClass.PriceToDouble(cq.AskPrice, cq.PriceType),
			ch.ReportingExg);
	}
	if ((eq.BBOChangeFlags & (NxBBOCHANGE_BIDPRICE | NxBBOCHANGE_BIDSIZE)) && eq.BestBidPrice > 0)
	{
		p += sprintf(p, "BBO_bid(%ld @ %.2lf) ",
			eq.BestBidSize,
			nxCoreClass.PriceToDouble(eq.BestBidPrice, cq.PriceType));
	}
	if ((eq.BBOChangeFlags & (NxBBOCHANGE_ASKPRICE | NxBBOCHANGE_ASKSIZE)) && eq.BestAskPrice > 0)
	{
		p += sprintf(p, "BBO_ask(%ld @ %.2lf) ",
			eq.BestAskSize,
			nxCoreClass.PriceToDouble(eq.BestAskPrice, cq.PriceType));
	}
	if (p > buf)
	{
		printf("%.2d:%.2d:%.2d.%.3d %s %s\n",
			(int)t.Hour, (int)t.Minute, (int)t.Second, (int)t.Millisecond,
			symbol, buf);
	}
	
}

void mmquoteDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreMMQuote &quote = pNxCoreMessage->coreData.MMQuote;

	printf("mm_qoute,");
	printf("", quote.MarketMakerType);
	printf("\n", quote.QuoteType);	
}

void statusDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{	
	//printf("status\n");
}

void symbolchangeDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreSymbolChange &symbol = pNxCoreMessage->coreData.SymbolChange;

	printf("symbol_change,");
	printf("", symbol.ListedExgOld);
	printf("\n", symbol.Status);
	
}

void symbolspinDump(const	NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreSymbolSpin &symbol = pNxCoreMessage->coreData.SymbolSpin;

	//printf("symbol_spin", symbol.);
}

void tradeDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader &ch = pNxCoreMessage->coreHeader;
	const NxCoreTrade &trade = pNxCoreMessage->coreData.Trade;

	printf("trade,");
	printf("", trade.BATECode);
	printf("", trade.ConditionFlags);
	printf("", trade.ExgSequence);
	printf("", trade.High);
	printf("", trade.Last);
	printf("", trade.Low);
	printf("", trade.NetChange);
	printf("", trade.Open);
	printf("", trade.Price);
	printf("", trade.PriceFlags);
	printf("", trade.PriceType);
	printf("", trade.RecordsBack);
	printf("", trade.Size);
	printf("", trade.Tick);
	printf("", trade.TickVolume);
	printf("", trade.TotalVolume);
	printf("", trade.TradeCondition);
	printf("\n", trade.VolumeType);
}

int __stdcall nxCoreCallback(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{

	switch (pNxCoreSys->Status) {
		case NxCORESTATUS_RUNNING:       // all is well.
		case NxCORESTATUS_INITIALIZING:  // start up code.
		case NxCORESTATUS_COMPLETE:      // tear down code.
			break;
		case NxCORESTATUS_ERROR:
			printf("Error %s (%d)\n:", pNxCoreSys->StatusDisplay, pNxCoreSys->StatusData);
			return NxCALLBACKRETURN_STOP;   // not required to return this
	}

	switch (pNxCoreMessage->MessageType)
	{
		case NxMSG_CATEGORY:
		{
			categoryDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		case NxMSG_EXGQUOTE:
		{
			exgquouteDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		case NxMSG_MMQUOTE:
		{
			mmquoteDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		//case NxMSG_STATUS:
		//{
		//	statusDump(pNxCoreSys, pNxCoreMessage);
		//	break;
		//}

		case NxMSG_SYMBOLCHANGE:
		{
			symbolchangeDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		case NxMSG_SYMBOLSPIN:
		{
			symbolspinDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		case NxMSG_TRADE:
		{
			tradeDump(pNxCoreSys, pNxCoreMessage);
			break;
		}

		default:
			break;
	}

	//Return and continue processing.
	return NxCALLBACKRETURN_CONTINUE;

}

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char *argv[])
{
	if (!nxCoreClass.LoadNxCore("NxCoreAPI.dll"))
	{
		fprintf(stderr, "Can't find NxCoreAPI.dll\n");
		return -1;
	}
	nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK, 0, nxCoreCallback);
	return 0;
}

