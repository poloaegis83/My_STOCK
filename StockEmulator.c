typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date Date;
typedef unsigned short SHORT16;
#define MA5_OVER_MA10 103
  
struct _DailyInfo {
   SHORT16 StockID;

   SHORT16 Start;
   SHORT16 End;
   SHORT16 High;
   SHORT16 Low;

   SHORT16 MA5;
   SHORT16 MA10;
   SHORT16 MA20;
   SHORT16 MA60;
   SHORT16 MA120;

   SHORT16 LeaderDifference;
   SHORT16 ForeignInvestorsDiff;
   SHORT16 InvestmentTrustDiff;  
   SHORT16 DealersDiff;

   SHORT16 DayIndex;
   
   Date    Dates;
};

struct _Date {
  SHORT16 Years;
  SHORT16 Mouths;
  SHORT16 Days;
}

DAILY_INFO *InitStockDailyInfoData(void*, SHORT16);

int MAIN()
(
  
)

VOID CaculateMA(DAILY_INFO * DailyInfo)
{
  //
  // Caculate MA data in DAILY_INFO.
  //
}

DAILY_INFO *InitStockDailyInfoData(void* XmlPointer ,SHORT16 days)
{
  //
  // Catch stock Data By ID from XML file, then init the data to struct.
  //
}

VOID FindBuyPoint(SHORT16 StartDayIndex, SHORT16 EndDayIndex, DAILY_INFO* DailyInfo)
{
  //
  // Find buying point
  //
  SHORT16    CurrentIndex;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  SHORT16    NewPrice;
  
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
      if(LastDay.->MA5 >= (LastDay->MA10 * MA5_OVER_MA10)/100) //MA5 should over MA10 103%
      {
        
      }
    }

    //
    // Check LeaderDifference, should be a postive number 2 days.
    //
    if(LastDay->LeaderDifference >0 && Last2Day->LeaderDifference >0)
    {

    }
    
  }
  

}

VOID FindSellPoint(SHORT16 BuyDayIndex, SHORT16 Price, DAILY_INFO* DailyInfo)
{
  //
  // Find Selling point
  //
}
