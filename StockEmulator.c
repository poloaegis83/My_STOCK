#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date DATE;
typedef struct _Trade_Record  TRADE_RECORD;
typedef unsigned short SHORT16;
#define MA5_OVER_MA10 103
  
struct _DailyInfo {
   SHORT16 StockID;

   SHORT16 Start;
   SHORT16 End;
   SHORT16 High;
   SHORT16 Low;

   SHORT16 LeaderDifference;
   SHORT16 ForeignInvestorsDiff;
   SHORT16 InvestmentTrustDiff;  
   SHORT16 DealersDiff;

   SHORT16 MA5;
   SHORT16 MA10;
   SHORT16 MA20;
   SHORT16 MA60;
   SHORT16 MA120;

   char    RSI6_Value;
   char    RSI12_Value;
   char    KD_K_Value;
   char    KD_D_Value;   

   SHORT16 DayIndex;
   DATE    Dates;
};

struct _Date {
  SHORT16 Years;
  SHORT16 Mouths;
  SHORT16 Days;
}

struct _Trade_Record {
  SHORT16       BuyDayIndex;
  SHORT16       SellDayIndex;
  
  SHORT16       BuyPrice;
  SHORT16       SellPrice;

  DATE          BuyDates;
  DATE          SellDates;

  TRADE_RECORD  *Next;
}

VOID InitStockDailyInfoData (FILE *XmlPointer, DAILY_INFO *DailyInfoBuffer, SHORT16 days);

int MAIN()
{

  FILE       *fptr;
  DAILY_INFO *StockDailyData;
  SHORT16     DayIntervals;
  SHORT16     StartDayIndex;
  SHORT16     EndDayIndex;
  
  pFile = fopen( "StockData.xml","w" );
  InitStockDailyInfoData(fptr, StockDailyData, DayIntervals);
  
  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //
  // StockSimulator (StartDayIndex, EndDayIndex, StockDailyData);


  return 0;

}

VOID StockSimulator(SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo)
{
  SHORT16       StartDayIndex1;
  SHORT16       BuyDayIndex;
  SHORT16       BuyPrice;
  SHORT16       SellDayIndex;
  SHORT16       SellPrice;
  
  TRADE_RECORD  *OldRecords;
  TRADE_RECORD  *NewRecords;
  
  StartDayIndex1 = StartDayIndex;

  while(SellDayIndex+2 < EndDayIndex) {  //Need at least 2 days left, for buypoint and sellpoint check.
    
    // TimeLine  |StartDayIndex---BuyDayIndex---EndDayIndex|
    FindBuyPoint(StartDayIndex1, EndDayIndex, DailyInfo, &BuyDayIndex, &BuyPrice);
    
    // TimeLine |BuyDayIndex---SellDayIndex---EndDayIndex|
    FindSellPoint(BuyDayIndex, EndDayIndex, BuyPrice, DailyInfo &SellDayIndex, &SellPrice);
  
    //
    // Record Events
    //
    NewRecords = (TRADE_RECORD*) malloc(sizeof(TRADE_RECORD));
    
    NewRecords->BuyDayIndex = BuyDayIndex;
    NewRecords->SellDayIndex = SellDayIndex;
    NewRecords->BuyPrice = BuyPrice;
    NewRecords->SellPrice = SellPrice;
    NewRecords->Next = NULL;
    
    OldRecords->Next = NewRecords;
    OldRecords = NewRecords;

    //
    // Next StartDay = this time SellDay
    //
    StartDayIndex1 = SellDayIndex;
  }

}

VOID CalculateMA(DAILY_INFO * DailyInfo)
{
  //
  // Calculate MA data write into DAILY_INFO.
  //
  SHORT16       Price5;
  SHORT16       Price10;
  SHORT16       Price20;
  SHORT16       Price60;   
  char          Index;

  //
  // Todo: Need add error handle here
  //


  for(Index = 0; Index < 5; Index ++) {
    Price5 += (DailyInfo-Index)->End;
  }
  
  for(Index = 0; Index < 10; Index ++) {
    Price10 += (DailyInfo-Index)->End;
  }
  
  for(Index = 0; Index < 20; Index ++) {
    Price20 += (DailyInfo-Index)->End;
  }

  for(Index = 0; Index < 60; Index ++) {
    Price60 += (DailyInfo-Index)->End;
  }
  
  DailyInfo->MA5    = Price5/5;
  DailyInfo->MA10   = Price10/10;
  DailyInfo->MA20   = Price20/20;  
  DailyInfo->MA60   = Price60/60;
  DailyInfo->MA120  = 0;          // No need MA120 now
}

VOID CalculateRSI(DAILY_INFO * DailyInfo)
{
  //
  // Calculate RSI data write into DAILY_INFO.
  //
  
}

VOID CalculateKD(DAILY_INFO * DailyInfo)
{
  //
  // Calculate KD data write into DAILY_INFO.
  //
  
}

VOID InitStockDailyInfoData(FILE *XmlPointer , DAILY_INFO *DailyInfoBuffer, SHORT16 days)
{
  //
  // Catch stock Data By ID from XML file, then init the data to struct.
  //

  //
  // Reads XML data and write it to DailyInfoBuffer
  //

  //
  // Allcate memory to buffer, the first data should 60 days before start day for calculate MA60.
  //
  DailyInfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(days+60));

  //
  // Calculate MA5 MA10 MA20 MA60 write into DailyInfoBuffer
  //
  CalculateMA(DailyInfoBuffer);

  //
  // Calculate KD and RSI write into DailyInfoBuffer
  //
  CalculateRSI(DailyInfoBuffer);
  
  CalculateKD(DailyInfoBuffer);

  //
  // Convert dates to DayIndex write into DailyInfoBuffer
  //
  
}

SHORT16 FindBuyPoint(SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo, SHORT16 *BuyDayIndex ,SHORT16 *BuyPrice)
{
  //
  // Find buying point
  //
  SHORT16    CurrentIndex;
  SHORT16    BuyDayIndex;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  SHORT16    NewPrice;
  SHORT16    BuyPrice;
  bool       MA_check = false;
  bool       LD_check = false;
  bool       RSI_check = false;
  bool       KD_check = false;
  
  DailyInfo += StartDayIndex; 
  
  for(CurrentIndex = StartDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay = DailyInfo--;
    Last2Day = LastDay--;
    NewPrice = DailyInfo.Start;
    
    //
    // Check MA
    //
    if(NewPrice > LastDay->MA5 && LastDay.MA5 > LastDay.MA10 && LastDay.MA10 >= LastDay.MA20)
    {
      if(LastDay->MA5 >= (LastDay->MA10 * MA5_OVER_MA10)/100) //MA5 should over MA10 103%
      {
        MA_check = true;
      }
    }

    //
    // Check LeaderDifference, should be a postive number 2 days.
    //
    if(LastDay->LeaderDifference >0 && Last2Day->LeaderDifference >0)
    {
       LD_check = true;
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
  }
  
}

VOID FindSellPoint(SHORT16 BuyDayIndex, SHORT16 EndDayIndex, SHORT16 BuyPrice, DAILY_INFO *DailyInfo, SHORT16 *SellDayIndex, SHORT16 *SellPrice)
{
  //
  // Find Selling point
  //
  SHORT16    NewPrice;
  SHORT16    CurrentIndex;  
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;  
  bool       ConditionCheck = false;
  bool       RSI_check      = false;
  bool       KD_check       = false;
  
  NewPrice = DailyInfo.Start;

  for (CurrentIndex = BuyDayIndex; CurrentIndex < EndDayIndex; CurrentIndex++)
  {
    LastDay = DailyInfo--;
    Last2Day = LastDay--;
    NewPrice = DailyInfo.Start;
    
    //
    // Check MA
    //
    // 3 Cases of LeaderDifference
    //  
    if (LastDay->LeaderDifference < 0 && Last2Day->LeaderDifference < 0) {
      if(NewPrice < LastDay.MA20) {
        ConditionCheck = true;
      }  
    } else if (LastDay->LeaderDifference < 0) {
      if(NewPrice < LastDay.MA10) {
        ConditionCheck = true;
      }  
    } else (LastDay->LeaderDifference >= 0) {
      if(NewPrice < LastDay.MA5) {
        ConditionCheck = true;
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
    }
  }
}
