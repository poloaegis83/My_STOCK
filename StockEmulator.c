#include "ezxml.h"

#define MA5_OVER_MA10 103
#define STOP_LOSS_LIMIT 15
#define FIRST_DAILY_DATA 60 /*For MA calculate*/

typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date DATE;
typedef struct _Trade_Record  TRADE_RECORD;
typedef unsigned short SHORT16;
typedef char bool;

struct _Date {
  SHORT16 Years;
  SHORT16 Mouths;
  SHORT16 Days;
};

struct _DailyInfo {
   SHORT16 StockID;

   SHORT16 Start;
   SHORT16 End;
   SHORT16 High;
   SHORT16 Low;

   int LeaderDiff;
   int ForeignInvestorsDiff;
   int InvestmentTrustDiff;  
   int DealersDiff;

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

struct _Trade_Record {
  SHORT16       BuyDayIndex;
  SHORT16       SellDayIndex;

  SHORT16       BuyPrice;
  SHORT16       SellPrice;

  DATE          BuyDates;
  DATE          SellDates;

  TRADE_RECORD  *Next;
};

void InitStockDailyInfoData (char *FileName, DAILY_INFO *DailyInfoBuffer, SHORT16 days);
void StockSimulator (SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo, TRADE_RECORD  *ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords);
void CalculateMA (DAILY_INFO * DailyInfo, SHORT16 days);
void CalculateRSI (DAILY_INFO * DailyInfo, SHORT16 days);
void CalculateKD (DAILY_INFO * DailyInfo, SHORT16 days);
void FindBuyPoint (SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo, SHORT16 *BuyDayIndex ,SHORT16 *BuyPrice);
void FindSellPoint (SHORT16 BuyDayIndex, SHORT16 EndDayIndex, SHORT16 BuyPrice, DAILY_INFO *DailyInfo, SHORT16 *SellDayIndex, SHORT16 *SellPrice);

void InitStockDailyInfoData(char *FileName , DAILY_INFO *DailyInfoBuffer, SHORT16 days)
{
  //
  // Catch stock data and ID from XML file, then init the data to struct.
  //
  ezxml_t  XmlFile, StockDatax, StockIdx, Dailyx, Pricex, Differencex;
  char     *LeaderDiff, *ForeignInvestorsDiff, *InvestmentTrustDiff, *DealersDiff;     /*Might be negative number*/
  char     *LeaderDiff2, *ForeignInvestorsDiff2, *InvestmentTrustDiff2, *DealersDiff2; /*Postive number*/  
  SHORT16  IndexDay,i,j; 
  
  InvestmentTrustDiff2                   = 0;
  ForeignInvestorsDiff2                  = 0;
  DealersDiff2                           = 0;
  LeaderDiff2                            = 0;	  

  XmlFile      = ezxml_parse_file (FileName);
  StockDatax   = ezxml_child (XmlFile,"StockData");
  StockIdx     = ezxml_child (StockDatax,"StockId"); 
  Dailyx       = ezxml_child (StockDatax,"Daily");
  //
  // Allcate memory to buffer, the first data should 60 (depends on FIRST_DAILY_DATA) days before start day for calculate MA60 (depends on FIRST_DAILY_DATA) .
  //
  DailyInfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(days+FIRST_DAILY_DATA));

  //
  // Parsing XML data and write into DailyInfoBuffer
  //
  for(IndexDay = 0; IndexDay < days+FIRST_DAILY_DATA; IndexDay++)
  {
    Pricex       = ezxml_child (Dailyx,"Price");
    Differencex  = ezxml_child (Dailyx,"Difference");

    DailyInfoBuffer->StockID               = (SHORT16)(StockIdx->txt);
    DailyInfoBuffer->DayIndex              = atoi(ezxml_attr(Dailyx,"Index"));
    DailyInfoBuffer->Dates.Years           = atoi(ezxml_attr(Dailyx,"Years"));
    DailyInfoBuffer->Dates.Mouths          = atoi(ezxml_attr(Dailyx,"Mouths"));
    DailyInfoBuffer->Dates.Days            = atoi(ezxml_attr(Dailyx,"Days"));
    DailyInfoBuffer->Start                 = atoi(ezxml_attr(Pricex, "Start"));
    DailyInfoBuffer->End                   = atoi(ezxml_attr(Pricex, "End"));
    DailyInfoBuffer->High                  = atoi(ezxml_attr(Pricex, "High"));
    DailyInfoBuffer->Low                   = atoi(ezxml_attr(Pricex, "Low"));
	InvestmentTrustDiff                    = 0;
	ForeignInvestorsDiff                   = 0;
	DealersDiff                            = 0;
	LeaderDiff                             = 0;
	
    //
    // Check if it's a negative number
    //
    if(strstr(LeaderDiff,"-")){
      for(i=strlen(LeaderDiff)-1,j=0;i>=0;i--){
          LeaderDiff2[j]=LeaderDiff[i];
          j++;
      }
    }
    if(strstr(ForeignInvestorsDiff,"-")){
      for(i=strlen(ForeignInvestorsDiff)-1,j=0;i>=0;i--){
          ForeignInvestorsDiff2[j]=ForeignInvestorsDiff[i];
          j++;
      }
    }
    if(strstr(InvestmentTrustDiff,"-")){
      for(i=strlen(InvestmentTrustDiff)-1,j=0;i>=0;i--){
          InvestmentTrustDiff2[j]=InvestmentTrustDiff[i];
          j++;
      }
    }
    if(strstr(DealersDiff,"-")){
      for(i=strlen(DealersDiff)-1,j=0;i>=0;i--){
          DealersDiff2[j]=DealersDiff[i];
          j++;
      }
    }

    DailyInfoBuffer->LeaderDiff            = strstr(LeaderDiff,"-") ?   -atoi(LeaderDiff2): atoi(LeaderDiff);
    DailyInfoBuffer->ForeignInvestorsDiff  = strstr(ForeignInvestorsDiff,"-") ? -atoi(ForeignInvestorsDiff2) : atoi(ForeignInvestorsDiff);
    DailyInfoBuffer->InvestmentTrustDiff   = strstr(InvestmentTrustDiff,"-") ? -atoi(InvestmentTrustDiff2) : atoi(InvestmentTrustDiff);
    DailyInfoBuffer->DealersDiff           = strstr(DealersDiff,"-") ? -atoi(DealersDiff2) : atoi(DealersDiff);

    Dailyx = Dailyx->next;
  }

  //
  // Calculate MA5 MA10 MA20 MA60 write into DailyInfoBuffer
  //
  CalculateMA (DailyInfoBuffer, days);

  //
  // Calculate KD and RSI write into DailyInfoBuffer
  //
  CalculateRSI (DailyInfoBuffer, days);

  CalculateKD (DailyInfoBuffer, days);

  ezxml_free (XmlFile);
}

void StockSimulator(SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo, TRADE_RECORD  *ReturnRecordsHead)
{
  SHORT16       StartDayIndex1;
  SHORT16       BuyDayIndex;
  SHORT16       BuyPrice;
  SHORT16       SellDayIndex;
  SHORT16       SellPrice;
  SHORT16       Count;

  TRADE_RECORD  *OldRecords;
  TRADE_RECORD  *NewRecords; 

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
}

void CalculateMA(DAILY_INFO *DailyInfo, SHORT16 days)
{
  //
  // Calculate MA data write into DAILY_INFO.
  //
  SHORT16       Price5;
  SHORT16       Price10;
  SHORT16       Price20;
  SHORT16       Price60;
  SHORT16       Price120;   
  SHORT16       MAIndex;
  SHORT16       DailyIndex;
  DAILY_INFO    *DailyInfoPtr;
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
      Price5 += (DailyInfo-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 10; MAIndex ++) {
      Price10 += (DailyInfo-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 20; MAIndex ++) {
      Price20 += (DailyInfo-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 60; MAIndex ++) {
      Price60 += (DailyInfo-MAIndex)->End;
    }

    for(MAIndex = 0; MAIndex < 120; MAIndex ++) {
      Price120 += (DailyInfo-MAIndex)->End;
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
    DailyInfoPtr++;
  }
}

void CalculateRSI(DAILY_INFO * DailyInfo, SHORT16 days)
{
  //
  // Calculate RSI data write into DAILY_INFO.
  //

}

void CalculateKD(DAILY_INFO * DailyInfo, SHORT16 days)
{
  //
  // Calculate KD data write into DAILY_INFO.
  //

}

void FindBuyPoint(SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo, SHORT16 *BuyDayIndex ,SHORT16 *BuyPrice)
{
  //
  // Find buying point
  //
  SHORT16    CurrentIndex;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  SHORT16    NewPrice;
  bool       MA_check  = 0;
  bool       LD_check  = 0;
  bool       RSI_check = 0;
  bool       KD_check  = 0;
  
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
}

void FindSellPoint(SHORT16 BuyDayIndex, SHORT16 EndDayIndex, SHORT16 BuyPrice, DAILY_INFO *DailyInfo, SHORT16 *SellDayIndex, SHORT16 *SellPrice)
{
  //
  // Find Selling point
  //
  SHORT16    NewPrice;
  SHORT16    CurrentIndex;  
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;  
  bool       ConditionCheck = 0;
  bool       RSI_check      = 0;
  bool       KD_check       = 0;

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
}

void AnalysisProfit (TRADE_RECORD  *TradeRecords)
{
   SHORT16  Count;
   SHORT16  EarnedMoney;
   SHORT16  LoseMoney;

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
}

int Main(int argc, char **argv)
{
  DAILY_INFO    *StockDailyData;
  SHORT16       DayIntervals;    //Tatol days for emulator
  SHORT16       StartDayIndex;
  SHORT16       EndDayIndex;
  TRADE_RECORD  *ReturnRecords;

  StockDailyData = NULL;
  // 
  // Argument format:
  // StockEmulator.exe [XmlFileName] [Days]
  //
  DayIntervals = (SHORT16)argv[1];

  //
  // Init the stock data struct
  //
  InitStockDailyInfoData (argv[0], StockDailyData, DayIntervals);

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
