//
// nxcore.c : Dump all NXCore categories.
//
// See Using Visual C++ with simple C programs. 
// http://www.swarthmore.edu/NatSci/tali/E15/Visual_C.html
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

#include "NxCoreAPI.h"
#include "NxCoreAPI_Class.h"

NxCoreClass nxCoreClass;

void categoryDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	const NxCoreHeader   &ch = pNxCoreMessage->coreHeader;
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

	printf("Category         : %s(%ld)\n", cat.pnxStringCategory->String, cat.pnxStringCategory->Atom);
	printf("  nxTime         : %.2d:%.2d:%.2d\n", (int)pNxCoreSys->nxTime.Hour, (int)pNxCoreSys->nxTime.Minute, (int)pNxCoreSys->nxTime.Second);
	printf("  symbol         : %s\n", ch.pnxStringSymbol->String);
	printf("  pnxOptionHdr   : %s\n", option);
	printf("  listedExch     : %ld\n", ch.ListedExg);
	printf("  sessionDate    : %d%.2d%.2d\n", (int)ch.nxSessionDate.Year, (int)ch.nxSessionDate.Month, (int)ch.nxSessionDate.Day);
	printf("  time           : %.2d:%.2d:%.2d\n", (int)ch.nxExgTimestamp.Hour, (int)ch.nxExgTimestamp.Minute, (int)ch.nxExgTimestamp.Second);
	printf("  reportExch     : %ld\n", ch.ReportingExg);
	printf("  sessionId      : %ld\n", (int)ch.SessionID);
	printf("  permissionId   : %ld [%s]\n", ch.PermissionID, nxCoreClass.GetDefinedString(NxST_PERMID, ch.PermissionID));
	printf("  NFields        : %ld\n", cat.NFields);

	for (int i = 0; i < cat.NFields; i++)
	{
		const NxCategoryField& cf = cat.pnxFields[i];

		printf("     %ld|%s|[%s] {%ld} :", i, cf.FieldName, cf.FieldInfo, (int)cf.Set);

		if (!cf.Set)
		{
			printf("  -- !Set --\n");
			continue;
		}

		switch (cf.FieldType)
		{
		case NxCFT_UNKNOWN:
		{
			printf("  -- unknown --");
			break;
		}
		case NxCFT_64BIT:
		{
			printf("  %I64d", cf.data.i64Bit);
			break;
		}
		case NxCFT_32BIT:
		{
			printf("  %ld", cf.data.i32Bit);
			break;
		}
		case NxCFT_STRINGZ:
		{
			printf("  %s", cf.data.StringZ);
			break;
		}
		case NxCFT_DOUBLE:
		{
			printf("  %lf", cf.data.Double);
			break;
		}
		case NxCFT_PRICE:
		{
			printf("  %.2lf", nxCoreClass.PriceToDouble(cf.data.nxPrice.Price, cf.data.nxPrice.PriceType));
			break;
		}
		case NxCFT_DATE:
		{
			printf("  %d%.2d%.2d", (int)cf.data.nxDate.Year, (int)cf.data.nxDate.Month, (int)cf.data.nxDate.Day);
			break;
		}
		case NxCFT_TIME:
		{
			printf("  %.2d:%.2d:%.2d.%.3d", (int)cf.data.nxTime.Hour, (int)cf.data.nxTime.Minute, (int)cf.data.nxTime.Second, (int)cf.data.nxTime.Millisecond);
			break;
		}
		case NxCFT_NxSTRING:
		{
			if (cf.data.pnxString)
			{
				const NxString& s = (const NxString&)*cf.data.pnxString;
				printf("  '%s'", s.String);
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
			printf("  [%ld:%ld] '%s'", stItem.ixTable, stItem.idString, ss);
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
						printf("  [%ld:%ld] '%s'", stItem.ixTable, i, ss);
						printed = true;
					}
					else
					{
						printf(",  [%ld:%ld] '%s'", stItem.ixTable, i, ss);
					}
				}
			}
			break;
		}
		}

		printf("\n");
	}
}

void exgquouteDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	printf("exgquote\n");
}

void mmquoteDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	printf("mmquote\n");
}


void statusDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	printf("status\n");
}

void symbolchangeDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	printf("symbolchange]\n");
}

void symbolspinDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	printf("symbolspin\n");
}

void statusDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	statusdump("\n");
}

void tradeDump(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{

}

int __stdcall nxCoreCallback(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
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

	case NxMSG_STATUS:
	{
		statusDump(pNxCoreSys, pNxCoreMessage);
		break;
	}

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

	}

	//Return and continue processing.
	return NxCALLBACKRETURN_CONTINUE;

}


int main(int argc, char** argv)
{
	if (!nxCoreClass.LoadNxCore("NxCoreAPI.dll"))
	{
		fprintf(stderr, "Can't find NxCoreAPI.dll\n");
		return -1;
	}
	nxCoreClass.ProcessTape(argv[1], 0, NxCF_EXCLUDE_CRC_CHECK | NxCF_EXCLUDE_QUOTES | NxCF_EXCLUDE_QUOTES2, 0, nxCoreCallback);
	return 0;
}


