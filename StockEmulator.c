#include "expat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MA5_OVER_MA10 103
#define STOP_LOSS_LIMIT 15
#define FIRST_DAILY_DATA 20 /*For MA calculate*/
#define BUFF_SIZE        100000
#define DEBUG(Expression)        \
          if(DebugFlag){         \
		  	printf(Expression);  \
		  }                      \

typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date DATE;
typedef struct _Trade_Record  TRADE_RECORD;
typedef char bool;

struct _Date {
  int Years;
  int Months;
  int Days;
};

struct _DailyInfo {
   int StockID;

   int Start;
   int End;
   int High;
   int Low;

   int LeaderDiff;
   int ForeignInvestorsDiff;
   int InvestmentTrustDiff;  
   int DealersDiff;

   int MA5;
   int MA10;
   int MA20;
   int MA60;
   int MA120;

   char    RSI6_Value;
   char    RSI12_Value;
   char    KD_K_Value;
   char    KD_D_Value;   

   int DayIndex;
   DATE    Dates;
};

struct _Trade_Record {
  int       BuyDayIndex;
  int       SellDayIndex;

  int       BuyPrice;
  int       SellPrice;

  DATE          BuyDates;
  DATE          SellDates;

  TRADE_RECORD  *Next;
};

char DebugFlag = 1;

void InitStockDailyInfoData (FILE *fp, DAILY_INFO *DataBuffer, int days);
void StockSimulator (int StartDayIndex, int EndDayIndex, DAILY_INFO* DailyInfo, TRADE_RECORD  *ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords);
void CalculateMA (DAILY_INFO * DailyInfo, int days);
void CalculateRSI (DAILY_INFO * DailyInfo, int days);
void CalculateKD (DAILY_INFO * DailyInfo, int days);
void FindBuyPoint (int StartDayIndex, int EndDayIndex, DAILY_INFO* DailyInfo, int *BuyDayIndex ,int *BuyPrice);
void FindSellPoint (int BuyDayIndex, int EndDayIndex, int BuyPrice, DAILY_INFO *DailyInfo, int *SellDayIndex, int *SellPrice);
void StartElement (void *data, const char *element, const char **attribute);
void EndElement   (void *data, const char *element);
void ElementData  (void *data, const char *content, int length);

bool StockIdFlag;
int     TheStockID;
int         Depth;              /*Global element depth*/
DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/

void StartElement (void *data, const char *Element, const char **attribute)
{ 
  int i;
  const char *Value;

  if ( !strcmp("Daily",Element) || !strcmp("Price",Element) || !strcmp("Difference",Element)) // Element = Daily or Price or Difference
  {
    for(i = 0; attribute[i]; i += 2)
    {
	  Value = attribute[i+1];
	  if(!strcmp("Index",attribute[i]))
	  {
        InfoBuffer->DayIndex      = (int)atoi(Value);
	  }
	  if(!strcmp("Years",attribute[i]))
	  {
        InfoBuffer->Dates.Years   = (int)atoi(Value);
	  }
	  if(!strcmp("Months",attribute[i]))
	  {
        InfoBuffer->Dates.Months  = (int)atoi(Value);
	  }
	  if(!strcmp("Days",attribute[i]))
	  {
        InfoBuffer->Dates.Days    = (int)atoi(Value);
	  }
	
	  if(!strcmp("Start",attribute[i]))
	  {
        InfoBuffer->Start         = (int)atoi(Value);
	  }
	  if(!strcmp("High",attribute[i]))
	  {
	    InfoBuffer->High          = (int)atoi(Value);
	  }
	  if(!strcmp("Low",attribute[i]))
	  {
	    InfoBuffer->Low           = (int)atoi(Value); 
	  }
	  if(!strcmp("End",attribute[i]))
	  {
	    InfoBuffer->End           = (int)atoi(Value);
	  }	  

	  if(!strcmp("DealersDiff",attribute[i]))
	  {
	    InfoBuffer->DealersDiff           = atoi(Value);
	  }
	  if(!strcmp("ForeignInvestorsDiff",attribute[i]))
	  {
	    InfoBuffer->ForeignInvestorsDiff  = atoi(Value);
	  }
	  if(!strcmp("InvestmentTrustDiff",attribute[i]))
	  {
	    InfoBuffer->InvestmentTrustDiff   = atoi(Value);
	  }
	  if(!strcmp("LeaderDiff",attribute[i]))
	  {
	    InfoBuffer->LeaderDiff            = atoi(Value);
	  }
    }
  }

  if (!strcmp("StockId",Element)) // Element = StockId
  {
    StockIdFlag = 1;
  }

  Depth++;
}
void EndElement   (void *Data, const char *Element)
{
  if (!strcmp("Daily",Element))
  {
	InfoBuffer->StockID = TheStockID;
    InfoBuffer++;    /*Move the pointer to next day*/
  }
  Depth--;
}
void ElementData  (void *Data, const char *Content, int Length)
{
  if(StockIdFlag)
  {
	TheStockID = atoi(Content);
    StockIdFlag = 0;
  }
}

void InitStockDailyInfoData(FILE *fp , DAILY_INFO *DataBuffer, int days)
{
  //
  // Catch stock data and ID from XML file, then init the data to struct.
  //
  XML_Parser   Parser;
  void         *Buff;
  int          FileLens;
  DAILY_INFO   *DailyInfoHead;
  
  DEBUG("InitStockDailyInfoData Start\n");  
  //
  // Allcate memory to buffer, the first data should 60 (depends on FIRST_DAILY_DATA) days before start day for calculate MA60 (depends on FIRST_DAILY_DATA) .
  //
  InfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(days+FIRST_DAILY_DATA));
  DailyInfoHead   = InfoBuffer;

  Parser   = XML_ParserCreate(NULL);

  Buff     = XML_GetBuffer(Parser, BUFF_SIZE);               //Allocate buffer

  FileLens = fread( Buff, sizeof(char), BUFF_SIZE, fp);      //Read data to buffer

  //
  // Set parser callback function
  //
  XML_SetStartElementHandler (Parser,StartElement);  /*When element start*/
  XML_SetEndElementHandler (Parser,EndElement);      /*When element end*/
  XML_SetCharacterDataHandler (Parser,ElementData);  /*When element data*/

  //
  // Call parser
  //
  if (! XML_ParseBuffer(Parser, FileLens, FileLens == 0)) {
    /* handle parse error */
  }
  DEBUG("Parsing finished\n");  
  //
  // Parsing XML data and write into DailyInfoBuffer
  //

  //
  // Calculate MA5 MA10 MA20 MA60 write into DailyInfoBuffer
  //
  CalculateMA (DailyInfoHead, days);

  //
  // Calculate KD and RSI write into DailyInfoBuffer
  //
  CalculateRSI (DailyInfoHead, days);

  CalculateKD (DailyInfoHead, days);

  XML_ParserFree(Parser);

  DataBuffer = DailyInfoHead;
  DEBUG("InitStockDailyInfoData End\n");   
}

void StockSimulator(int StartDayIndex, int EndDayIndex, DAILY_INFO* DailyInfo, TRADE_RECORD  *ReturnRecordsHead)
{
  int       StartDayIndex1;
  int       BuyDayIndex;
  int       BuyPrice;
  int       SellDayIndex;
  int       SellPrice;
  int       Count;

  TRADE_RECORD  *OldRecords;
  TRADE_RECORD  *NewRecords; 

  DEBUG("StockSimulator Start\n");

  Count = 0;
  OldRecords = NULL;
  NewRecords = NULL;
  SellDayIndex = 0;
  StartDayIndex1 = StartDayIndex;

  while(SellDayIndex+2 < EndDayIndex) {  //Need at least 2 days left, for buypoint and sellpoint check.

    //
    // Time order: StartDayIndex-->BuyDayIndex-->SellDayIndex-->EndDayIndex
    //             [---------------FindBuyPoint---------------------------]
    //                            [-------------FindSellPoint-------------]        
    //
    FindBuyPoint(StartDayIndex1, EndDayIndex, DailyInfo, &BuyDayIndex, &BuyPrice);

    if (BuyDayIndex != 0 && BuyPrice != 0)
    {
      FindSellPoint(BuyDayIndex, EndDayIndex, BuyPrice, DailyInfo, &SellDayIndex, &SellPrice);           
    }
    //
    // Record trade events
    //
    NewRecords = (TRADE_RECORD*) malloc(sizeof(TRADE_RECORD));

    NewRecords->BuyDayIndex  = BuyDayIndex;
    NewRecords->SellDayIndex = SellDayIndex;
    NewRecords->BuyPrice     = BuyPrice;
    NewRecords->SellPrice    = SellPrice;
    NewRecords->Next         = 0;

    if(Count == 0) {
      ReturnRecordsHead = NewRecords;
    } else {
      OldRecords->Next = NewRecords;
    }
      OldRecords = NewRecords;

    //
    // Next time StartDayIndex equals SellDayIndex at this times
    //
    StartDayIndex1 = SellDayIndex;
    Count++;

    if ( (BuyDayIndex != 0 && BuyPrice != 0) || (SellDayIndex != 0 && SellPrice != 0) ) /*no buy or sell info*/
    {
      break;
    }
  }
  
  DEBUG("StockSimulator End\n");  
}

void CalculateMA(DAILY_INFO *DailyInfo, int days)
{
  //
  // Calculate MA data write into DAILY_INFO.
  //
  int       Price5;
  int       Price10;
  int       Price20;
  int       Price60;
  int       Price120;   
  int       MAIndex;
  int       DailyIndex;
  DAILY_INFO    *DailyInfoPtr;
  
  DEBUG("CalculateMA Start\n");    
  //
  // Todo: Need add error handle here
  //
  Price5       = 0;
  Price10      = 0;
  Price20      = 0;
  Price60      = 0;
  Price120     = 0;

  DailyInfoPtr = DailyInfo + FIRST_DAILY_DATA;

  for(DailyIndex = 0; DailyIndex < days; DailyIndex++)
  {    
    for(MAIndex = 0; MAIndex < 5; MAIndex ++) {
      Price5 += (DailyInfoPtr-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 10; MAIndex ++) {
      Price10 += (DailyInfoPtr-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 20; MAIndex ++) {
      Price20 += (DailyInfoPtr-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 60; MAIndex ++) {
      Price60 += (DailyInfoPtr-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 120; MAIndex ++) {
      Price120 += (DailyInfoPtr-MAIndex)->End;
    }

    if(FIRST_DAILY_DATA >= 5){
      DailyInfoPtr->MA5    = Price5/5;
    }
    if(FIRST_DAILY_DATA >= 10){
      DailyInfoPtr->MA10   = Price10/10;
    }
    if(FIRST_DAILY_DATA >= 20){
      DailyInfoPtr->MA20   = Price20/20;
    }
    if(FIRST_DAILY_DATA >= 60){
      DailyInfoPtr->MA60   = Price60/60;
    }
    if(FIRST_DAILY_DATA >= 120){
      DailyInfoPtr->MA120  = Price120/120;
    }
/*
  printf("DailyInfoPtr->StockID  = %d\n",DailyInfoPtr->StockID);
  printf("DailyInfoPtr->DayIndex = %d\n",DailyInfoPtr->DayIndex);
  printf("DailyInfoPtr->Start    = %d\n",DailyInfoPtr->Start);
  printf("DailyInfoPtr->High    = %d\n",DailyInfoPtr->High);
  printf("DailyInfoPtr->Low      = %d\n",DailyInfoPtr->Low);  
  printf("DailyInfoPtr->End      = %d\n",DailyInfoPtr->End);
  printf("DailyInfoPtr->LeaderDiff    = %d\n",DailyInfoPtr->LeaderDiff);
  printf("DailyInfoPtr->ForeignInvestorsDiff    = %d\n",DailyInfoPtr->ForeignInvestorsDiff);
  printf("DailyInfoPtr->InvestmentTrustDiff   = %d\n",DailyInfoPtr->InvestmentTrustDiff);  
  printf("DailyInfoPtr->DealersDiff      = %d\n",DailyInfoPtr->DealersDiff);
  printf("DailyInfoPtr->Dates.Years       = %d\n",DailyInfoPtr->Dates.Years);
  printf("DailyInfoPtr->Dates.Months       = %d\n",DailyInfoPtr->Dates.Months);
  printf("DailyInfoPtr->Dates.Days       = %d\n===================\n",DailyInfoPtr->Dates.Days);    
*/
    DailyInfoPtr++;
  }
  DEBUG("CalculateMA End\n");  
}

void CalculateRSI(DAILY_INFO * DailyInfo, int days)
{
  //
  // Calculate RSI data write into DAILY_INFO.
  //

}

void CalculateKD(DAILY_INFO * DailyInfo, int days)
{
  //
  // Calculate KD data write into DAILY_INFO.
  //

}

void FindBuyPoint(int StartDayIndex, int EndDayIndex, DAILY_INFO* DailyInfo, int *BuyDayIndex ,int *BuyPrice)
{
  //
  // Find buying point
  //
  int    CurrentIndex;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  int    NewPrice;
  bool       MA_check  = 0;
  bool       LD_check  = 0;
  bool       RSI_check = 0;
  bool       KD_check  = 0;

  DEBUG("FindBuyPoint Start\n");

  DailyInfo += StartDayIndex; 
  
  for(CurrentIndex = StartDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay = DailyInfo--;
    Last2Day = LastDay--;
    NewPrice = DailyInfo->Start;
    
    //
    // Check MA
    //
    if(NewPrice > LastDay->MA5 && LastDay->MA5 > LastDay->MA10 && LastDay->MA10 >= LastDay->MA20)
    {
      if(LastDay->MA5 >= (LastDay->MA10 * MA5_OVER_MA10)/100) //MA5 should over MA10 103%
      {
        MA_check = 1;
      }
    }

    //
    // Check LeaderDifference, should be a postive number 2 days.
    //
    if(LastDay->LeaderDiff >0 && Last2Day->LeaderDiff >0)
    {
       LD_check = 1;
    }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
    if(MA_check && LD_check)
    {
      *BuyDayIndex = StartDayIndex + CurrentIndex;
      *BuyPrice    = NewPrice;
      break;
    }
    else
    {
       //
       // No buy point
       //
       *BuyDayIndex = 0;
       *BuyPrice    = 0;
    }
  }
  
  DEBUG("FindBuyPoint End\n");  
}

void FindSellPoint(int BuyDayIndex, int EndDayIndex, int BuyPrice, DAILY_INFO *DailyInfo, int *SellDayIndex, int *SellPrice)
{
  //
  // Find Selling point
  //
  int    NewPrice;
  int    CurrentIndex;  
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;  
  bool       ConditionCheck = 0;
  bool       RSI_check      = 0;
  bool       KD_check       = 0;

  DEBUG("FindSellPoint Start\n"); 

  NewPrice = DailyInfo->Start;

  for (CurrentIndex = BuyDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay = DailyInfo--;
    Last2Day = LastDay--;
    NewPrice = DailyInfo->Start;

    //
    // Check for stop loss order
    //
    if((BuyPrice - NewPrice)/BuyPrice > STOP_LOSS_LIMIT) // if loss more than 15% 
    {
      ConditionCheck = 1;
      break;
    }

    //
    // Check MA
    //
    // 3 Cases of LeaderDifference
    //  
    if (LastDay->LeaderDiff < 0 && Last2Day->LeaderDiff < 0) {
      if(NewPrice < LastDay->MA20) {
        ConditionCheck = 1;
      }  
    } else if (LastDay->LeaderDiff < 0) {
      if(NewPrice < LastDay->MA10) {
        ConditionCheck = 1;
      }
    } else if(LastDay->LeaderDiff >= 0) {
      if(NewPrice < LastDay->MA5) {
        ConditionCheck = 1;
      }
    }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
    if(ConditionCheck)
    {
      *SellDayIndex = BuyDayIndex + CurrentIndex;
      *SellPrice    = NewPrice;
      break;
    } else {
       //
       // No sell point
       //
       *SellDayIndex = 0;
       *SellPrice    = 0;
    }
  }
  DEBUG("FindSellPoint End\n");   
}

void AnalysisProfit (TRADE_RECORD  *TradeRecords)
{
   int  Count;
   int  EarnedMoney;
   int  LoseMoney;

   DEBUG("AnalysisProfit Start\n");
   EarnedMoney = 0;
   LoseMoney   = 0;
   Count       = 0;

   while(TradeRecords->Next != NULL) 
   {
     printf("TradeRecords count %d \n", Count);

     if((TradeRecords->BuyPrice != 0 || TradeRecords->BuyDayIndex != 0) && (TradeRecords->SellPrice != 0 || TradeRecords->SellDayIndex != 0)) /*if buy point and sell point exist*/ 
     {
       printf("Buy: DayIndex:%d, price:%d |||||| Sell: DayIndex:%d, price:%d\n", TradeRecords->BuyDayIndex, TradeRecords->BuyPrice, TradeRecords->SellDayIndex, TradeRecords->SellPrice);                         
       if(TradeRecords->BuyPrice >= TradeRecords->SellPrice)
       {
         EarnedMoney += (TradeRecords->SellPrice - TradeRecords->BuyPrice);
         printf("EarnedMoney = %d\n",TradeRecords->SellPrice - TradeRecords->BuyPrice);
       } else {
         LoseMoney += (TradeRecords->BuyPrice - TradeRecords->SellPrice);
         printf("LoseMoney = %d\n",TradeRecords->BuyPrice - TradeRecords->SellPrice);         
       }
       Count++;
     }
   }
  DEBUG("AnalysisProfit End\n");    
}

int main(int argc, char **argv)
{
  DAILY_INFO    *StockDailyData;
  int       DayIntervals;    //Tatol days for emulator
  int       StartDayIndex;
  int       EndDayIndex;
  TRADE_RECORD  *ReturnRecords;
  FILE          *fp;  

  StockDailyData = NULL;
  // 
  // Argument format:
  // StockEmulator.exe [XmlFileName] [Days]
  //
  DayIntervals = atoi(argv[2]);
  printf("DayIntervals = %d\n",DayIntervals);

  fp = fopen(argv[1],"r");
  if (fp == NULL) {
	printf("open file error!!\n");
	return 1;  
  }

  //
  // Init the stock data struct
  //
  InitStockDailyInfoData (fp, StockDailyData, DayIntervals);

  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //
  ReturnRecords = NULL;
  StartDayIndex = FIRST_DAILY_DATA;  /*Start from 60 days*/
  EndDayIndex = StartDayIndex + DayIntervals;
  StockSimulator (StartDayIndex, EndDayIndex, StockDailyData, ReturnRecords);

  //
  // Calculate profit base on the records
  //
  AnalysisProfit (ReturnRecords);

  free(StockDailyData);

  return 0;
}
