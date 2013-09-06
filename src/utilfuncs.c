/*This code is part of the Epoch Init System.
* The Epoch Init System is maintained by Subsentient.
* This software is public domain.
* Please read the file LICENSE.TXT for more information.*/

/**This file contains functions and utilities used across Epoch
 * for miscellanious purposes, that don't really belong in a category of their own.**/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include "epoch.h"

/*Days in the month, for time stuff.*/
static const short MDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

Bool AllNumeric(const char *InStream)
{ /*Is the string all numbers?*/
	if (!*InStream)
	{ /*No data? Don't say it's all numeric then.*/
		return false;
	}
	
	for (; *InStream != '\0'; ++InStream)
	{
		if (!isdigit(*InStream))
		{
			return false;
		}
	}
	
	return true;
}

Bool ProcessRunning(unsigned long InPID)
{ /*Checks /proc for a directory with the name of the requested process.*/
	DIR *DCore;
	struct dirent *DStruct;
	
	DCore = opendir("/proc/");
	
	while ((DStruct = readdir(DCore)))
	{
		if (AllNumeric(DStruct->d_name) && DStruct->d_type == 4)
		{
			if (atoi(DStruct->d_name) == InPID)
			{
				return true;
			}
		}
	}
	
	return false;
}

void MinsToDate(unsigned long MinInc, unsigned long *OutHr, unsigned long *OutMin,
				unsigned long *OutMonth, unsigned long *OutDay, unsigned long *OutYear)
{  /*Returns the projected date that it will be after MinInc minutes.
	* Not really a good example of a function that belongs in console.c.*/
	time_t CoreClock;
	struct tm *TimeStruct = NULL;
	unsigned long Hr, Min, Day, Mon, Year;
	
	time(&CoreClock);
	TimeStruct = localtime(&CoreClock);
	
	Hr = TimeStruct->tm_hour;
	Min = TimeStruct->tm_min;
	Mon = TimeStruct->tm_mon + 1;
	Day = TimeStruct->tm_mday;
	Year = TimeStruct->tm_year + 1900;
	
	for (; MinInc; --MinInc)
	{
		if (Min + 1 == 60)
		{
			Min = 0;
			
			if (Hr + 1 == 24)
			{
				Hr = 0;
				
				if (Day == MDays[Mon - 1])
				{
					Day = 1;
					
					if (Mon == 12)
					{
						Mon = 1;
						
						++Year;
					}
					else
					{
						++Mon;
					}
				}
				else
				{
					++Day;
				}
	
			}
			else
			{
				++Hr;
			}
		}
		else
		{
			++Min;
		}
	}
	
	*OutHr = Hr;
	*OutMin = Min;
	*OutMonth = Mon;
	*OutDay = Day;
	*OutYear = Year;
}

unsigned long DateDiff(unsigned long InHr, unsigned long InMin, unsigned long *OutMonth,
						unsigned long *OutDay, unsigned long *OutYear)
{ /*Provides a true date as to when the next occurrence of this hour and minute will return via pointers, and
	* also provides the number of minutes that will elapse during the time between. You can pass NULL for the pointers.*/
	struct tm *TimeP;
	time_t CoreClock;
	unsigned long Hr, Min, Month, Day, Year, IncMin = 0;
	
	time(&CoreClock);
	TimeP = localtime(&CoreClock);
	
	Hr = TimeP->tm_hour;
	Min = TimeP->tm_min;
	Month = TimeP->tm_mon + 1;
	Day = TimeP->tm_mday;
	Year = TimeP->tm_year + 1900;
	
	for (; Hr != InHr || Min != InMin; ++IncMin)
	{
		if (Min == 60)
		{
			Min = 0;
			
			if (Hr + 1 == 24)
			{
				Hr = 0;
				
				if (Day == MDays[Month - 1])
				{
					Day = 1;
					
					if (Month == 12)
					{
						Month = 1;
						
						++Year;
					}
					else
					{
						++Month;
					}
				}
				else
				{
					++Day;
				}
			}
			else
			{
				++Hr;
			}
		}
		else
		{
			++Min;
		}
	}
	
	if (OutMonth) *OutMonth = Month;
	if (OutDay) *OutDay = Day;
	if (OutYear) *OutYear = Year;
	
	return IncMin;
}

void GetCurrentTime(char *OutHr, char *OutMin, char *OutSec, char *OutMonth, char *OutDay, char *OutYear)
{ /*You can put NULL for items that you don't want the value of.*/
	struct tm *TimeP;
	long HMS_I[3];
	long MDY_I[3];
	char *HMS[3];
	char *MDY[3];
	short Inc = 0;
	time_t TimeT;
	
	/*Compiler whines if I try to initialize these.*/
	HMS[0] = OutHr;
	HMS[1] = OutMin;
	HMS[2] = OutSec;
	
	MDY[0] = OutMonth;
	MDY[1] = OutDay;
	MDY[2] = OutYear;
	
	/*Actually get the time.*/
	time(&TimeT);
	TimeP = localtime(&TimeT);
	
	HMS_I[0] = TimeP->tm_hour;
	HMS_I[1] = TimeP->tm_min;
	HMS_I[2] = TimeP->tm_sec;
	
	MDY_I[0] = TimeP->tm_mon + 1;
	MDY_I[1] = TimeP->tm_mday;
	MDY_I[2] = TimeP->tm_year + 1900;
	
	for (; Inc < 3; ++Inc)
	{
		if (HMS[Inc] == NULL)
		{
			continue;
		}
		
		snprintf(HMS[Inc], 16, (HMS_I[Inc] >= 10 ? "%ld" : "0%ld"), HMS_I[Inc]);
	}
	
	for (Inc = 0; Inc < 3; ++Inc)
	{
		if (MDY[Inc] != NULL)
		{
			snprintf(MDY[Inc], 16, (MDY_I[Inc] >= 10 ? "%ld" : "0%ld"), MDY_I[Inc]);
		}
	}
}

