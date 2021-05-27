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

DAILY_INFO *InitStockDailyInfoData(void*, SHORT16);

int MAIN()
{

  FILE       *fptr;
  DAILY_INFO *StockDailyData;
  SHORT16     DayIntervals;
  
  pFile = fopen( "StockData.xml","w" );
  InitStockDailyInfoData (fptr, StockDailyData, DayIntervals);
  

  //
  //StockSimulator
  //

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

  while(SellDayIndex+2 < EndDayIndex) {
    //
    // Find buying point
    //
    
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
  // Calculate MA data in DAILY_INFO.
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
  DailyInfo->MA120  = 0;
}

VOID CalculateRSI(DAILY_INFO * DailyInfo)
{
  //
  // Calculate RSI data in DAILY_INFO.
  //

}

VOID CalculateKD(DAILY_INFO * DailyInfo)
{
  //
  // Calculate KD data in DAILY_INFO.
  //

}

DAILY_INFO *InitStockDailyInfoData(FILE *XmlPointer , DAILY_INFO * DailyInfoBuffer, SHORT16 days)
{
  //
  // Catch stock Data By ID from XML file, then init the data to struct.
  //
  
  //
  // Reads XML data and write it to DailyInfoBuffer
  //

  //
  // Allcate memory to buffer
  //
  
  //
  // Calculate MA5 MA10 MA20 MA60 in DailyInfoBuffer
  //
  CalculateMA(DailyInfoBuffer);

  //
  // Calculate KD RSI in DailyInfoBuffer
  //
  CalculateRSI(DailyInfoBuffer);
  
  CalculateKD(DailyInfoBuffer);

  //
  // Convert dates to DayIndex in DailyInfoBuffer
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

VOID FindSellPoint(SHORT16 BuyDayIndex, SHORT16 EndDayIndex, SHORT16 BuyPrice, DAILY_INFO* DailyInfo, SHORT16 *SellDayIndex, SHORT16 *SellPrice)
{
  //
  // Find Selling point
  //
}
