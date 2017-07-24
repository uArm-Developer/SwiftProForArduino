/**
  ******************************************************************************
  * @file	uArmReportService.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#ifndef _UARMREPORTSERVICE_H_
#define _UARMREPORTSERVICE_H_

#include <Arduino.h>
#include "Marlin.h"

#define TYPE_NONE 0
#define REPORT_SERVICE_ITEM_MAX 10

typedef struct _ReportServiceItem
{
	void (*reportFuncCB)();
	uint8_t type;
	long interval;
	long timestamp;
} ReportServiceItem;

typedef struct _ReportServiceList
{
	ReportServiceItem item[REPORT_SERVICE_ITEM_MAX];

} ReportServiceList;

void reportServiceInit();
void reportServiceRun();

void addReportService(uint8_t type, long interval, void (*reportFuncCB)());
void removeReportService(uint8_t type);


#endif // _UARMREPORTSERVICE_H_
