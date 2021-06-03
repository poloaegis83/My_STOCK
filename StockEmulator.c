#include "expat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MA5_OVER_MA10 101
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

   int     DayIndex;
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

char DebugFlag = 0;

void InitStockDailyInfoData (FILE *fp, int days);
void StockSimulator (int StartDayIndex, int EndDayIndex, TRADE_RECORD **ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords);
void CalculateMA (int days);
void CalculateRSI (int days);
void CalculateKD (int days);
void FindBuyPoint (int StartDayIndex, int EndDayIndex, int *BuyDayIndex ,int *BuyPrice);
void FindSellPoint (int BuyDayIndex, int EndDayIndex, int BuyPrice, int *SellDayIndex, int *SellPrice);
void StartElement (void *data, const char *element, const char **attribute);
void EndElement   (void *data, const char *element);
void ElementData  (void *data, const char *content, int length);

char        StockIdFlag;
int         TheStockID;
int         Depth;              /*Global element depth*/
DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/
DAILY_INFO  *BuffInitPtr;
XML_Parser   Parser;

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
        BuffInitPtr->DayIndex      = (int)atoi(Value);
	  }
	  if(!strcmp("Years",attribute[i]))
	  {
        BuffInitPtr->Dates.Years   = (int)atoi(Value);
	  }
	  if(!strcmp("Months",attribute[i]))
	  {
        BuffInitPtr->Dates.Months  = (int)atoi(Value);
	  }
	  if(!strcmp("Days",attribute[i]))
	  {
        BuffInitPtr->Dates.Days    = (int)atoi(Value);
	  }
	
	  if(!strcmp("Start",attribute[i]))
	  {
        BuffInitPtr->Start         = (int)atoi(Value);
	  }
	  if(!strcmp("High",attribute[i]))
	  {
	    BuffInitPtr->High          = (int)atoi(Value);
	  }
	  if(!strcmp("Low",attribute[i]))
	  {
	    BuffInitPtr->Low           = (int)atoi(Value); 
	  }
	  if(!strcmp("End",attribute[i]))
	  {
	    BuffInitPtr->End           = (int)atoi(Value);
	  }	  

	  if(!strcmp("DealersDiff",attribute[i]))
	  {
	    BuffInitPtr->DealersDiff           = atoi(Value);
	  }
	  if(!strcmp("ForeignInvestorsDiff",attribute[i]))
	  {
	    BuffInitPtr->ForeignInvestorsDiff  = atoi(Value);
	  }
	  if(!strcmp("InvestmentTrustDiff",attribute[i]))
	  {
	    BuffInitPtr->InvestmentTrustDiff   = atoi(Value);
	  }
	  if(!strcmp("LeaderDiff",attribute[i]))
	  {
	    BuffInitPtr->LeaderDiff            = atoi(Value);
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
	BuffInitPtr->StockID = TheStockID;
    BuffInitPtr++;    /*Move the pointer to next day*/
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

void PrintInfo()
{
  DAILY_INFO  *Daily;
  int         DailyIndex;
  
  Daily = InfoBuffer;
  
  for(DailyIndex = 0; DailyIndex < 70; DailyIndex++)
  {    
    printf("DailyInfoPtr->StockID  = %d\n",Daily->StockID);
    printf("DailyInfoPtr->DayIndex = %d\n",Daily->DayIndex);
    printf("DailyInfoPtr->Start    = %d\n",Daily->Start);
    printf("DailyInfoPtr->High     = %d\n",Daily->High);
    printf("DailyInfoPtr->Low      = %d\n",Daily->Low);  
    printf("DailyInfoPtr->End      = %d\n",Daily->End);
    printf("DailyInfoPtr->LeaderDiff              = %d\n",Daily->LeaderDiff);
    printf("DailyInfoPtr->ForeignInvestorsDiff    = %d\n",Daily->ForeignInvestorsDiff);
    printf("DailyInfoPtr->InvestmentTrustDiff     = %d\n",Daily->InvestmentTrustDiff);  
    printf("DailyInfoPtr->DealersDiff       = %d\n",Daily->DealersDiff);
    printf("DailyInfoPtr->Dates.Years       = %d\n",Daily->Dates.Years);
    printf("DailyInfoPtr->Dates.Months      = %d\n",Daily->Dates.Months);
    printf("DailyInfoPtr->Dates.Days        = %d\n",Daily->Dates.Days);
    printf("DailyInfoPtr->MA5        = %d\n",Daily->MA5);
    printf("DailyInfoPtr->MA10       = %d\n",Daily->MA10);
    printf("DailyInfoPtr->MA20       = %d\n",Daily->MA20);
    printf("DailyInfoPtr->MA60       = %d\n===================\n",Daily->MA60);	
	Daily += 1;
  }
}

void InitStockDailyInfoData(FILE *fp , int days)
{
  //
  // Catch stock data and ID from XML file, then init the data to struct.
  //
  void         *Buff;
  int          FileLens;
  
  DEBUG("InitStockDailyInfoData Start\n");  
  //
  // Allcate memory to buffer, the first data should 60 (depends on FIRST_DAILY_DATA) days before start day for calculate MA60 (depends on FIRST_DAILY_DATA) .
  //
  InfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(days+FIRST_DAILY_DATA));
  BuffInitPtr = InfoBuffer;

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
  CalculateMA (days);

  //
  // Calculate KD and RSI write into DailyInfoBuffer
  //
  CalculateRSI (days);

  CalculateKD (days);
  
  DEBUG("InitStockDailyInfoData End\n");   
}

void StockSimulator(int StartDayIndex, int EndDayIndex, TRADE_RECORD  **ReturnRecordsHead)
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
  SellPrice    = 0;
  StartDayIndex1 = StartDayIndex;

  while(SellDayIndex+2 < EndDayIndex) {  //Need at least 2 days left, for buypoint and sellpoint check.
    //
    // Time order: StartDayIndex-->BuyDayIndex-->SellDayIndex-->EndDayIndex
    //             [---------------FindBuyPoint---------------------------]
    //                            [-------------FindSellPoint-------------]        
    //
    FindBuyPoint(StartDayIndex1, EndDayIndex, &BuyDayIndex, &BuyPrice);

    if((BuyDayIndex != 0 && BuyPrice != 0))
	{
      FindSellPoint(BuyDayIndex+1, EndDayIndex, BuyPrice, &SellDayIndex, &SellPrice); /*if buy, move to next day to find sell*/
	}

    if ( (BuyDayIndex == 0 && BuyPrice == 0) || (SellDayIndex == 0 && SellPrice == 0) ) /*no sell or buy info ==> finish*/
    {
      break;
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
      *ReturnRecordsHead = NewRecords;
    } else {	
      OldRecords->Next = NewRecords;
    }
      OldRecords = NewRecords;
    //
    // Next time StartDayIndex equals SellDayIndex at this times
    //
    StartDayIndex1 = SellDayIndex;
    Count += 1;
  }

  DEBUG("StockSimulator End\n");  
}

void CalculateMA(int days)
{
  //
  // Calculate MA data write into DAILY_INFO.
  //
  int         Price5;
  int         Price10;
  int         Price20;
  int         Price60;
  int         Price120;   
  int         MAIndex;
  int         DailyIndex;
  DAILY_INFO  *Daily;
  
  DEBUG("CalculateMA Start\n");    
  //
  // Todo: Need add error handle here
  //

  Daily = InfoBuffer + FIRST_DAILY_DATA;

  for(DailyIndex = 0; DailyIndex < days; DailyIndex++)
  {
    Price5       = 0;
    Price10      = 0;
    Price20      = 0;
    Price60      = 0;
    Price120     = 0;

    for(MAIndex = 0; MAIndex < 5; MAIndex ++) {
      Price5 += (Daily-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 10; MAIndex ++) {
      Price10 += (Daily-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 20; MAIndex ++) {
      Price20 += (Daily-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 60; MAIndex ++) {
      Price60 += (Daily-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 120; MAIndex ++) {
      Price120 += (Daily-MAIndex)->End;
    }

    if(FIRST_DAILY_DATA >= 5){
      Daily->MA5    = Price5/5;
    }
    if(FIRST_DAILY_DATA >= 10){
      Daily->MA10   = Price10/10;
    }
    if(FIRST_DAILY_DATA >= 20){
      Daily->MA20   = Price20/20;
    }
    if(FIRST_DAILY_DATA >= 60){
      Daily->MA60   = Price60/60;
    }
    if(FIRST_DAILY_DATA >= 120){
      Daily->MA120  = Price120/120;
    }
    Daily += 1;
  }
  DEBUG("CalculateMA End\n");  
}

void CalculateRSI(int days)
{
  //
  // Calculate RSI data write into DAILY_INFO.
  //

}

void CalculateKD(int days)
{
  //
  // Calculate KD data write into DAILY_INFO.
  //

}

void FindBuyPoint(int StartDayIndex, int EndDayIndex, int *BuyDayIndex ,int *BuyPrice)
{
  //
  // Find buying point
  //
  int        CurrentIndex;
  DAILY_INFO *Daily;     
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  int        NewPrice;
  char       MA_check  = 0;
  char       LD_check  = 0;
  char       RSI_check = 0;
  char       KD_check  = 0;

  Daily = InfoBuffer + StartDayIndex-1; 

  for(CurrentIndex = StartDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay =  Daily-1;
    Last2Day = LastDay-1;
    NewPrice = Daily->Start;
    //printf("SearchBuy- NewDay(%d),LastDay(%d),LD2(%d) = %d,%d,%d\n",Daily->DayIndex,LastDay->DayIndex,Last2Day->DayIndex, NewPrice, LastDay->Start, Last2Day->Start);

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
    //if(MA_check && LD_check)
	if(MA_check)
    {
      *BuyDayIndex = CurrentIndex;
      *BuyPrice    = NewPrice;
	  DEBUG("FindBuyPoint Find\n"); 
      DEBUG("FindBuyPoint End\n");  
      return;
    }
    Daily += 1;
  }
  
  //
  // No buy point
  //
  *BuyDayIndex = 0;
  *BuyPrice    = 0;

  DEBUG("FindBuyPoint Not Find\n"); 
  DEBUG("FindBuyPoint End\n");  
}

void FindSellPoint(int BuyDayIndex, int EndDayIndex, int BuyPrice, int *SellDayIndex, int *SellPrice)
{
  //
  // Find Selling point
  //
  int        NewPrice;
  int        CurrentIndex;
  DAILY_INFO *Daily;   
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;  
  char       ConditionCheck = 0;
  char       RSI_check      = 0;
  char       KD_check       = 0;

  DEBUG("FindSellPoint Start\n"); 

  Daily = InfoBuffer+BuyDayIndex;

  for (CurrentIndex = BuyDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay = Daily-1;
    Last2Day = LastDay-1;
    NewPrice = Daily->Start;
    //printf("SearchSell- NewDay(%d),LastDay(%d),LD2(%d) = %d,%d,%d\n",Daily->DayIndex,LastDay->DayIndex,Last2Day->DayIndex, NewPrice, LastDay->Start, Last2Day->Start);
    //
    // Check for stop loss order
    //
    if ( (NewPrice < BuyPrice) && ((BuyPrice - NewPrice)/BuyPrice > STOP_LOSS_LIMIT) )// if loss more than 15% 
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
      *SellDayIndex = CurrentIndex;
      *SellPrice    = NewPrice;
	  DEBUG("FindSellPoint Find\n"); 
      DEBUG("FindSellPoint End\n");  	  
      return;
    }
	
    Daily += 1;
  }
  //
  // No sell point
  //
  *SellDayIndex = 0;
  *SellPrice    = 0;
  DEBUG("SellPoint Not Find\n"); 	     
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

  printf("===============================================\n");   
   do{
       printf("\nTradeRecords count %d\n", Count);

       if((TradeRecords->BuyPrice != 0 || TradeRecords->BuyDayIndex != 0) && (TradeRecords->SellPrice != 0 || TradeRecords->SellDayIndex != 0)) /*if buy point and sell point exist*/ 
       {
         printf("Buy: DayIndex:%d, price:%d |||||| Sell: DayIndex:%d, price:%d\n", TradeRecords->BuyDayIndex, TradeRecords->BuyPrice, TradeRecords->SellDayIndex, TradeRecords->SellPrice);                         
         if(TradeRecords->BuyPrice <= TradeRecords->SellPrice)
         {
           EarnedMoney += (TradeRecords->SellPrice - TradeRecords->BuyPrice);
           printf("EarnedMoney = %d\n", TradeRecords->SellPrice - TradeRecords->BuyPrice);
         } else {
           LoseMoney += (TradeRecords->BuyPrice - TradeRecords->SellPrice);
           printf("LoseMoney = %d\n",TradeRecords->BuyPrice - TradeRecords->SellPrice);         
         }
         Count++;
       }
	 TradeRecords = TradeRecords->Next;
     printf("===============================================\n");    	 
   } while(TradeRecords != NULL);

  printf("\nTotal = %d\n",EarnedMoney - LoseMoney);
  printf("===============================================\n");    
  DEBUG("AnalysisProfit End\n");    
}

int main(int argc, char **argv)
{
  int           DayIntervals;    //Tatol days for emulator
  int           StartDayIndex;
  int           EndDayIndex;
  TRADE_RECORD  *ReturnRecords;
  FILE          *fp;  

  // 
  // Argument format:
  // StockEmulator.exe [XmlFileName] [Days]
  //
  DayIntervals = atoi(argv[2]);

  fp = fopen(argv[1],"r");
  if (fp == NULL) {
	printf("open file error!!\n");
	return 1;  
  }

  //
  // Init the stock data struct
  //
  InitStockDailyInfoData (fp, DayIntervals);

  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //

  ReturnRecords = NULL;
 
  StartDayIndex = FIRST_DAILY_DATA+3;  /*Start from 20+3 days*/
   
  EndDayIndex = StartDayIndex + DayIntervals;
  
  printf("DayIntervals = %d, Start from index %d (FIRST_DAILY_DATA+3) to %d days\n",DayIntervals,StartDayIndex,EndDayIndex);  

  StockSimulator (StartDayIndex, EndDayIndex, &ReturnRecords);

  //
  // Calculate profit base on the records
  //
  AnalysisProfit (ReturnRecords);

  free(InfoBuffer);
  XML_ParserFree(Parser);
  return 0;
}