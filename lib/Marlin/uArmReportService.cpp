/**
  ******************************************************************************
  * @file	uArmReportService.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#include "uArmReportService.h" 

ReportServiceList reportServiceList;

void reportServiceInit()
{

	for (int i = 0; i <REPORT_SERVICE_ITEM_MAX; i++)
	{
		reportServiceList.item[i].type = TYPE_NONE;
	}	
}

void reportServiceRun()
{
	long curTimeStamp = millis();
	for (int i = 0; i < REPORT_SERVICE_ITEM_MAX; i++)
	{
		// exist
		if (reportServiceList.item[i].type != TYPE_NONE)
		{
			if((unsigned long)(curTimeStamp - reportServiceList.item[i].timestamp) >= reportServiceList.item[i].interval)
			{
				reportServiceList.item[i].timestamp = curTimeStamp;

				if (reportServiceList.item[i].reportFuncCB != NULL)
				{
					debugPrint("reportFuncCB\r\n");
					reportServiceList.item[i].reportFuncCB();
				}
			}

		}
	}		
}

void addReportService(uint8_t type, long interval, void (*reportFuncCB)())
{
	if (reportFuncCB == NULL)
		return;

	if (type == TYPE_NONE)
		return;

	if (interval == 0)
	{
		return;
	}
	
	debugPrint("addReportService %d\r\n", type);
	int i = 0;
	for (i = 0; i < REPORT_SERVICE_ITEM_MAX; i++)
	{
		// exist
		if (reportServiceList.item[i].type == type)
		{
			reportServiceList.item[i].interval = interval;
			reportServiceList.item[i].timestamp = millis();
			reportServiceList.item[i].reportFuncCB = reportFuncCB;
			break;
		}
	}

	// new item
	if (i == REPORT_SERVICE_ITEM_MAX)
	{


		// find a hole
		for (int j = 0; j < REPORT_SERVICE_ITEM_MAX; j++)
		{
			if (reportServiceList.item[j].type == TYPE_NONE)
			{
				reportServiceList.item[j].type = type;
				reportServiceList.item[j].interval = interval;
				reportServiceList.item[j].timestamp = millis();
				reportServiceList.item[j].reportFuncCB = reportFuncCB;	



				return;
			}
		}


	}

}


void removeReportService(uint8_t type)
{
	if (type == TYPE_NONE)
		return;

	for (int i = 0; i < REPORT_SERVICE_ITEM_MAX; i++)
	{
		// exist
		if (reportServiceList.item[i].type == type)
		{
			reportServiceList.item[i].type = TYPE_NONE;

			break;
		}
	}	
}