#include "expat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MA5_OVER_MA10 100.1
#define STOP_LOSS_LIMIT 0.02
#define MA_MAX          20     /*For MA calculate*/
#define FIRST_DAILY_DATA_INDEX 22 /*MA_MAX + 2*/
#define BUFF_SIZE        300000
#define DEBUG(Expression)        \
          if(DebugFlag){         \
		  	printf(Expression);  \
		  }                      \

typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date DATE;
typedef struct _MA_Value  MA_VALUE;
typedef struct _KDJ_Value KDJ_VALUE;
typedef struct _RSI_Value RSI_VALUE;
typedef struct _Trade_Record  TRADE_RECORD;

struct _Date {
  int Years;
  int Months;
  int Days;
};

struct _MA_Value {
   int MA5;
   int MA10;
   int MA20;
   int MA60;
   int MA120;
};

struct _KDJ_Value {
  float  K;
  float  D;
  float  J;
  float  RSV;
};

struct _RSI_Value {
  float  RSI_6;
  float  RSI_12;
  float  Up6;
  float  Down6;
  float  Up12;
  float  Down12;  
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

   MA_VALUE   MA;
   RSI_VALUE  RSI;
   KDJ_VALUE  KDJ;

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

char DebugFlag = 1;

void InitStockDailyInfoData (FILE *fp, int days);
void StockSimulator (int StartDayIndex, int EndDayIndex, TRADE_RECORD **ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords);
void CalculateMA (int days);
void CalculateRSI (int days);
void CalculateKDJ (int days);
void FindBuyPoint (int StartDayIndex, int EndDayIndex, int *BuyDayIndex ,int *BuyPrice);
void FindSellPoint (int BuyDayIndex, int EndDayIndex, int BuyPrice, int *SellDayIndex, int *SellPrice);
void StartElement (void *data, const char *element, const char **attribute);
void EndElement   (void *data, const char *element);
void ElementData  (void *data, const char *content, int length);

char        StockIdFlag;
int         TheStockID;
int         Depth;              /*Global element depth*/
int         GlobalDays;
DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/
DAILY_INFO  *BuffInitPtr;
XML_Parser   Parser;
int         ChipAnalysisFlag; /*Featrue*/
float       MA5_Over_MA10;

void StartElement (void *data, const char *Element, const char **attribute)
{ 
  int i;
  const char *Value;

  Depth++;
  
  if(GlobalDays == 0){
  	 return;
  }

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

}
void EndElement   (void *Data, const char *Element)
{
  Depth--;
  if(GlobalDays == 0){
  	 return;
  }
  if (!strcmp("Daily",Element))
  {
	  BuffInitPtr->StockID = TheStockID;
    BuffInitPtr++;    /*Move the pointer to next day*/
	  GlobalDays--;
  }

}
void ElementData  (void *Data, const char *Content, int Length)
{
  if(StockIdFlag)
  {
	TheStockID = atoi(Content);
    StockIdFlag = 0;
  }
}

void PrintInfo(days)
{
  DAILY_INFO  *Daily;
  int         DailyIndex;
  
  Daily = InfoBuffer;
  
  for(DailyIndex = 0; DailyIndex < days+FIRST_DAILY_DATA_INDEX -1 ; DailyIndex++)
  {    
    printf("DailyInfoPtr->StockID               = %d\n",Daily->StockID);
    printf("DailyInfoPtr->DayIndex   %d/%d/%d  = %d\n",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,Daily->DayIndex);
    printf("DailyInfoPtr->Start                 = %d\n",Daily->Start);
    printf("DailyInfoPtr->High                  = %d\n",Daily->High);
    printf("DailyInfoPtr->Low                   = %d\n",Daily->Low);  
    printf("DailyInfoPtr->End                   = %d\n",Daily->End);
    printf("DailyInfoPtr->LeaderDiff            = %d\n",Daily->LeaderDiff);
    printf("DailyInfoPtr->ForeignInvestorsDiff  = %d\n",Daily->ForeignInvestorsDiff);
    printf("DailyInfoPtr->InvestmentTrustDiff   = %d\n",Daily->InvestmentTrustDiff);  
    printf("DailyInfoPtr->DealersDiff           = %d\n",Daily->DealersDiff);
    printf("DailyInfoPtr->Dates.Years           = %d\n",Daily->Dates.Years);
    printf("DailyInfoPtr->Dates.Months          = %d\n",Daily->Dates.Months);
    printf("DailyInfoPtr->Dates.Days            = %d\n",Daily->Dates.Days);
    printf("DailyInfoPtr->MA5                   = %d\n",Daily->MA.MA5);
    printf("DailyInfoPtr->MA10                  = %d\n",Daily->MA.MA10);
    printf("DailyInfoPtr->MA20                  = %d\n",Daily->MA.MA20);
    printf("DailyInfoPtr->MA60                  = %d\n",Daily->MA.MA60);
    printf("DailyInfoPtr->K                     = %.1f\n",Daily->KDJ.K);	
    printf("DailyInfoPtr->D                     = %.1f\n",Daily->KDJ.D);
    printf("DailyInfoPtr->J                     = %.1f\n",Daily->KDJ.J);    
    printf("DailyInfoPtr->RSI(6)                = %.1f\n",Daily->RSI.RSI_6);	
    printf("DailyInfoPtr->RSI(12)               = %.1f\n",Daily->RSI.RSI_12);	
    printf("=============================================\n\n");
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
  // Allcate memory to buffer, the first data should 20 (depends on FIRST_DAILY_DATA_INDEX) days before start day for calculate MA20 (depends on FIRST_DAILY_DATA_INDEX) .
  //
  InfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(days+FIRST_DAILY_DATA_INDEX+50));
  BuffInitPtr = InfoBuffer;

  GlobalDays = days + FIRST_DAILY_DATA_INDEX - 1;  // Total days needs, prevent init data over the buffer

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

  CalculateKDJ (days);
  
  //PrintInfo(days);
  
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

  while(SellDayIndex+2 < EndDayIndex) //Need at least 2 days left, for buypoint and sellpoint check.
  {  
    //
    // Time order: StartDayIndex-->BuyDayIndex-->SellDayIndex-->EndDayIndex
    //             [---------------FindBuyPoint---------------------------]
    //                            [-------------FindSellPoint-------------]        
    //
    FindBuyPoint(StartDayIndex1, EndDayIndex, &BuyDayIndex, &BuyPrice);

    if((BuyDayIndex != 0 && BuyPrice != 0))
	  {
      printf("<===>\n");
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
    // Next time StartDayIndex equals SellDayIndex+1 at this times
    //
    StartDayIndex1 = SellDayIndex+1;
    Count += 1;
	  printf(" \n");  
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
  int         DailyCounter;
  DAILY_INFO  *Daily;
  
  DEBUG("CalculateMA Start\n");    
  //
  // Todo: Need add error handle here
  //

  Daily = InfoBuffer + MA_MAX -1; /*Day Number 20*/

  for(DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX - MA_MAX) ; DailyCounter++)
  {
    Price5       = 0;
    Price10      = 0;
    Price20      = 0;
    Price60      = 0;
    Price120     = 0;

    if(MA_MAX >= 5){
      for(MAIndex = 0; MAIndex < 5; MAIndex ++) {
        Price5 += (Daily-MAIndex)->End;
      }
      Daily->MA.MA5    = Price5/5;
    }

    if(MA_MAX >= 10){
      for(MAIndex = 0; MAIndex < 10; MAIndex ++) {
        Price10 += (Daily-MAIndex)->End;
      }
      Daily->MA.MA10   = Price10/10;
    }

    if(MA_MAX >= 20){
      for(MAIndex = 0; MAIndex < 20; MAIndex ++) {
        Price20 += (Daily-MAIndex)->End;
      }
      Daily->MA.MA20   = Price20/20;
    }

    if(MA_MAX >= 60){
      for(MAIndex = 0; MAIndex < 60; MAIndex ++) {
        Price60 += (Daily-MAIndex)->End;
      }
      Daily->MA.MA60   = Price60/60;
    }

    if(MA_MAX >= 120){
      for(MAIndex = 0; MAIndex < 120; MAIndex ++) {
        Price120 += (Daily-MAIndex)->End;
      }
      Daily->MA.MA120  = Price120/120;
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
  int         DailyCounter;
  int         i;
  DAILY_INFO  *Daily6;
  DAILY_INFO  *Daily12;
  float       Increase;
  float       Decrease;
  float       Upt,Dnt;

  Daily6    = InfoBuffer + 6;  /*Day Index 7*/
  Daily12   = InfoBuffer + 12; /*Day Index 13*/


  for(DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX -6) -1; DailyCounter++)
  {
    //printf("Daily = %d/%d/%d  ",Daily6->Dates.Years,Daily6->Dates.Months,Daily6->Dates.Days);
    //Smooth RSI
    if(DailyCounter == 0) {    //First day
      Increase = 0;
      Decrease = 0;
      for(i = 0; i < 6; i++)
      {
        if( (Daily6-i -1)->End > (Daily6-i)->End )
        {
          //printf("-= %d  ,",(Daily6-i -1)->End - (Daily6-i)->End);
          Decrease += (Daily6-i -1)->End - (Daily6-i)->End;
        }else
        {
          //printf("+= %d ,",(Daily6-i)->End - (Daily6-i -1)->End);        
          Increase += (Daily6-i)->End - (Daily6-i -1)->End;
        }
      }
      //printf("Increase = %1.f, Decrease = %1.f ,",Increase,Decrease);    
      Increase = Increase/6;
      Decrease = Decrease/6;
      Daily6->RSI.Down6 = Decrease;
      Daily6->RSI.Up6   = Increase;
      //printf("Increase/6 = %1.f, Decrease =/6 %1.f\n\n",Increase,Decrease);
   
      Daily6->RSI.RSI_6 = (Increase / (Increase + Decrease)) * 100;

      Daily6 += 1;
      continue;
    }
 
    if( Daily6->End > (Daily6-1)->End ) //Increase today
    {
      Upt = (Daily6-1)->RSI.Up6 * 5 + Daily6->End - (Daily6-1)->End ;
      Dnt = (Daily6-1)->RSI.Down6 * 5 + 0;
      //printf("+= %d ,",(Daily6)->End - (Daily6 -1)->End);       
    }
    else
    {
      Upt = (Daily6-1)->RSI.Up6 * 5 + 0;
      Dnt = (Daily6-1)->RSI.Down6 *5 + (Daily6-1)->End - Daily6->End ;
      //printf("-= %d  ,",(Daily6 -1)->End - (Daily6)->End);
    }
    //printf("Upt = %f, Dnt = %f,  ",Upt,Dnt);
    Upt = Upt /6;
    Dnt = Dnt /6;
    //printf("Upt/6 = %f, Dnt/6 = %f \n",Upt,Dnt);
    Daily6->RSI.Up6   = Upt;
    Daily6->RSI.Down6 = Dnt;
    Daily6->RSI.RSI_6 = (  Upt / (Upt+Dnt) )*100;

    Daily6 += 1;
  }

  for(DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX -12) -1; DailyCounter++)
  {
    //printf("Daily = %d/%d/%d  ",Daily6->Dates.Years,Daily6->Dates.Months,Daily6->Dates.Days);
    //Smooth RSI
    if(DailyCounter == 0) {    //First day
      Increase = 0;
      Decrease = 0;
      for(i = 0; i < 12; i++)
      {
        if( (Daily12-i -1)->End > (Daily12-i)->End )
        {
          Decrease += (Daily12-i -1)->End - (Daily12-i)->End;
        }else
        {   
          Increase += (Daily12-i)->End - (Daily12-i -1)->End;
        }
      }

      Increase = Increase/12;
      Decrease = Decrease/12;
      Daily12->RSI.Down12 = Decrease;
      Daily12->RSI.Up12   = Increase;
   
      Daily12->RSI.RSI_12 = (Increase / (Increase + Decrease)) * 100;

      Daily12 += 1;
      continue;
    }
 
    if( Daily12->End > (Daily12-1)->End ) //Increase today
    {
      Upt = (Daily12-1)->RSI.Up12 * 11 + Daily12->End - (Daily12-1)->End ;
      Dnt = (Daily12-1)->RSI.Down12 * 11 + 0;    
    }
    else
    {
      Upt = (Daily12-1)->RSI.Up12 * 11 + 0;
      Dnt = (Daily12-1)->RSI.Down12 * 11 + (Daily12-1)->End - Daily12->End ;
    }

    Upt = Upt /12;
    Dnt = Dnt /12;

    Daily12->RSI.Up12   = Upt;
    Daily12->RSI.Down12 = Dnt;
    Daily12->RSI.RSI_12 = (  Upt / (Upt+Dnt) )*100;

    Daily12 += 1;
  }

}


void CalculateKDJ(int days)
{
  //
  // Calculate KD data write into DAILY_INFO.
  //

  int         Highest;
  int         Lowest;
  int         DailyCounter;
  int         i;
  int         RSV_n;
  DAILY_INFO  *Daily;

  //
  // Calculate RSV(9)
  //

  RSV_n   = 9;
  Daily   = InfoBuffer + RSV_n -1; /*(Day number 9)*/

  //
  // Default K,D Value = 50 (Day number 8)
  //
  (Daily-1)->KDJ.K = 50;
  (Daily-1)->KDJ.D = 50;

  for (DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX - RSV_n) -1; DailyCounter++)
  {
    //printf("Daily = %d/%d/%d  ",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days);
    //
	  // Find Highest and Lowest in N days
	  //
    Highest = Daily->High;
    Lowest  = Daily->Low;
    //printf("Begin Highest= %d Loweest =%d  \n", Highest, Lowest);
    for (i = 1; i < RSV_n; i++) /*To pervious 8 days*/
    {
      //printf("-%d day,High = %d, Low = %d   \n",i,(Daily-i)->High,(Daily-i)->Low );
      if((Daily-i)->High > Highest)
      {
        Highest = (Daily-i)->High;
      }
      if((Daily-i)->Low < Lowest)
      {
        Lowest = (Daily-i)->Low;
      }
    }
    //printf("After Daily(9) Highest= %d Loweest =%d  \n\n", Highest, Lowest);
    Daily->KDJ.RSV = ((float)(Daily->End - Lowest) / (float)(Highest - Lowest)) * 100;
    Daily->KDJ.K = ((Daily-1)->KDJ.K * 2 / 3) +  (Daily->KDJ.RSV / 3);  /*Pervious K * 2/3 + todays RSV *1/3 */
    Daily->KDJ.D = ((Daily-1)->KDJ.D * 2 / 3) +  (Daily->KDJ.K /3);     /*Pervious D * 2/3 + todays K   *1/3 */
    Daily->KDJ.J = Daily->KDJ.K * 3 - Daily->KDJ.D * 2;

    Daily += 1;
  }

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
  int        NewPriceS;
  int        NewPriceE;  
  int        New5MA;
  int        New10MA;
  int        New20MA;  
  char       MA_checkS = 0;
  char       MA_checkE = 0;  
  char       LD_check  = 0;
  char       RSI_check = 0;
  char       KD_check  = 0;
  float      Percent;

  Daily = InfoBuffer + StartDayIndex-1; /* +22 */ 

  for (CurrentIndex = StartDayIndex; CurrentIndex <= EndDayIndex; CurrentIndex++)
  {
    LastDay =  Daily-1;
    Last2Day = LastDay-1;
    printf("SearchBuy-%d/%d/%d LastDay(%d),LD2(%d),CurrInd = %d,%d,%d\n",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,LastDay->DayIndex,Last2Day->DayIndex, LastDay->Start, Last2Day->Start, CurrentIndex);

    //
    // Check MA Start
    //
    NewPriceS = Daily->Start;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceS)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceS)/10;
	  New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceS)/20;
	  //printf("Price(S) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceS,New5MA,New10MA,New20MA);
    if(NewPriceS > New5MA && New5MA >= New10MA && New10MA >= New20MA)
    {
	  Percent = ((float)New5MA/(float)New10MA)*100;
      if (Percent >= MA5_OVER_MA10) //MA5 should over MA10 101%
      {
        MA_checkS = 1;
      }
    }

    //
    // Check MA End
    //
	  NewPriceE = Daily->End;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceE)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceE)/10;
	  New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceE)/20;
  	//printf("Price(E) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceE,New5MA,New10MA,New20MA);	
    if(NewPriceE > New5MA && New5MA >= New10MA && New10MA >= New20MA)
    {
	  Percent = ((float)New5MA/(float)New10MA)*100;	
      if( Percent >= MA5_OVER_MA10) //MA5 should over MA10 101%
      {
        MA_checkE = 1;
      }
    }


    //
    // Check LeaderDifference, should be a postive number 2 days.
    //
	  if(ChipAnalysisFlag)
  	{
      if(LastDay->LeaderDiff >0 && Last2Day->LeaderDiff >0)
      {
       LD_check = 1;
      }
	  } else
    {
	  LD_check = 1;
	  }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
	  if(MA_checkS && LD_check)
    {
      *BuyPrice    = NewPriceS;
      *BuyDayIndex = CurrentIndex;
	    //printf("V(S)\n");
	    return;	
    }
	  if (MA_checkE && LD_check) 
    {
      *BuyPrice    = NewPriceE;
      *BuyDayIndex = CurrentIndex;
    	//printf("V(E)\n");
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
  int        CurrentIndex;
  DAILY_INFO *Daily;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  int        NewPriceS;
  int        NewPriceE; 
  char       ConditionCheckS = 0;
  char       ConditionCheckE = 0;  
  char       RSI_check      = 0;
  char       KD_check       = 0;
  int        New5MA;
  int        New10MA;
  int        New20MA;  
  float      Percent;
  
  DEBUG("FindSellPoint Start\n"); 

  Daily = InfoBuffer + BuyDayIndex-1;

  for (CurrentIndex = BuyDayIndex; CurrentIndex <= EndDayIndex; CurrentIndex++)
  {
    LastDay   = Daily-1;
    Last2Day  = LastDay-1;

    printf("SearchSell-%d/%d/%d ,LastDay(%d),LD2(%d),CurrentIndex = %d,%d,%d\n",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,LastDay->DayIndex,Last2Day->DayIndex, LastDay->Start, Last2Day->Start,CurrentIndex);

    NewPriceS = Daily->Start;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceS)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceS)/10;
  	New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceS)/20;
  	//printf("Price(S) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceS,New5MA,New10MA,New20MA);
    //
    // Check MA Start
    //
    // 3 Cases of LeaderDifference
    //
    if(ChipAnalysisFlag)
    {
      if (LastDay->LeaderDiff > 0 && Last2Day->LeaderDiff > 0) 
        {
          if(NewPriceS < New10MA) 
	      {
            ConditionCheckS = 1;
          }
	    }
      if (LastDay->LeaderDiff > 0) 
        {
          if(NewPriceS < New10MA) 
	      {
            ConditionCheckS = 1;	
          }
        } 
      if(LastDay->LeaderDiff <= 0 && ChipAnalysisFlag) 
	    {
          if(NewPriceS < New5MA) 
	      {
            ConditionCheckS = 1;
          }
        }
    }else
    {
      if(NewPriceS < New5MA) 
	    {
          ConditionCheckS = 1;
        }
    }

	/*
    if(NewPriceS < New5MA) {
      ConditionCheckS = 1;
    }*/
    //
    // Check for stop loss order
    //
	//print("stoploss = %f \n",(((float)BuyPrice - (float)NewPriceS) /(float)BuyPrice) );
	
	  Percent = (float)(BuyPrice - NewPriceS) /(float)BuyPrice;
    if ( (NewPriceS < BuyPrice) && (Percent > STOP_LOSS_LIMIT) )// if loss more than 15% 
    {
	    //printf("  Percent = %.2f\n",Percent);
      ConditionCheckS = 1;
    }

    NewPriceE = Daily->End;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceE)/5;
  	New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceE)/10;
  	New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceE)/20;
  	//printf("Price(E) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceE,New5MA,New10MA,New20MA);
	
    //
    // Check MA End
    //
    // 3 Cases of LeaderDifference
    //

    if(ChipAnalysisFlag)
    {
      if (LastDay->LeaderDiff > 0 && Last2Day->LeaderDiff > 0) 
        {
          if(NewPriceE < New20MA) 
	      {
            ConditionCheckE = 1;
          }
	    }
      if (LastDay->LeaderDiff > 0) 
        {
          if(NewPriceE < New10MA) 
	      {
            ConditionCheckE = 1;	
          }
        } 
      if(LastDay->LeaderDiff <= 0 && ChipAnalysisFlag) 
	    {
          if(NewPriceE < New5MA) 
	      {
            ConditionCheckE = 1;
          }
        }
    }else
    {
      if(NewPriceE < New5MA) 
	    {
          ConditionCheckE = 1;
        }
    }
	/*
    if(NewPriceE < New5MA) {
      ConditionCheckE = 1;
    }*/
    //
    // Check for stop loss order
    //
    Percent = (float)(BuyPrice - NewPriceE) /(float)BuyPrice;
    if ( (NewPriceE < BuyPrice) && (Percent > STOP_LOSS_LIMIT))// if loss more than 10% 
    {
	  //printf("  Percent = %.2f\n",Percent);	
      ConditionCheckE = 1;
    }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
    if(ConditionCheckS)
    {
      *SellDayIndex = CurrentIndex;
      *SellPrice    = NewPriceS;
	    //printf("V(S)\n");  
      return;
    }
	if (ConditionCheckE)
	{
      *SellDayIndex = CurrentIndex;
      *SellPrice    = NewPriceE;
	    //printf("V(E)\n");  	  
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
   int   Count;
   int   WinCount;
   int   LoseCount;
   int   EarnedMoney;
   int   LoseMoney;
   float Percent;
   float AverWin;
   float AverLose;   
   DAILY_INFO *Daily; 

   
   Daily = InfoBuffer;
   
   DEBUG("AnalysisProfit Start\n");
   EarnedMoney = 0;
   LoseMoney   = 0;
   WinCount    = 0;
   LoseCount   = 0;
   Count       = 1;

  printf("===============================================\n");   
   do{
       printf("\nTradeRecord: %d\n", Count);

       if((TradeRecords->BuyPrice != 0 || TradeRecords->BuyDayIndex != 0) && (TradeRecords->SellPrice != 0 || TradeRecords->SellDayIndex != 0)) /*if buy point and sell point exist*/ 
       {
         printf("[Buy]: DayIndex:%d, price:%d  ====> [Sell]: DayIndex:%d, price:%d\n", TradeRecords->BuyDayIndex+1, TradeRecords->BuyPrice, TradeRecords->SellDayIndex+1, TradeRecords->SellPrice);
		 printf("%d/%d/%d-------------------",(Daily+TradeRecords->BuyDayIndex)->Dates.Years,(Daily+TradeRecords->BuyDayIndex)->Dates.Months,(Daily+TradeRecords->BuyDayIndex)->Dates.Days);                    
         printf("-----------------%d/%d/%d\n",(Daily+TradeRecords->SellDayIndex)->Dates.Years,(Daily+TradeRecords->SellDayIndex)->Dates.Months,(Daily+TradeRecords->SellDayIndex)->Dates.Days);
		 if(TradeRecords->BuyPrice <= TradeRecords->SellPrice)
         {
           EarnedMoney += (TradeRecords->SellPrice - TradeRecords->BuyPrice);
           printf("EarnedMoney = %d\n", TradeRecords->SellPrice - TradeRecords->BuyPrice);
		   WinCount++;
         } else {
           LoseMoney += (TradeRecords->BuyPrice - TradeRecords->SellPrice);
           printf("LoseMoney = %d\n",TradeRecords->BuyPrice - TradeRecords->SellPrice);
		   LoseCount++;   
         }
         Count++;
       }
	 TradeRecords = TradeRecords->Next;
     printf("===============================================\n");    	 
   } while(TradeRecords != NULL);

  Percent = (float)WinCount/(float)Count;
  AverWin  = (float)EarnedMoney/(float)WinCount;
  AverLose = (float)LoseMoney/(float)LoseCount;
  printf("\nTotal Earned = %d, Average Win/Lose money per trade = %.1f/%.1f \n",EarnedMoney - LoseMoney, AverWin, AverLose);
  printf("Win/Lose/Total = %d/%d/%d , %.1f %%Chance to Wins\n", WinCount, LoseCount, Count, Percent*100);
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
  int           ArgIndex;
  // 
  // Argument format:
  // StockEmulator.exe [XmlFileName] [Days] -c(ChipAnalysisFlag on)
  //
  ChipAnalysisFlag = 0;

  for(ArgIndex = 0; ArgIndex < argc; ArgIndex++)
  {
  	 if(ArgIndex == 1)
	 {
       fp = fopen(argv[1],"r");
       if (fp == NULL) {
	     printf("open file error!!\n");
	     return 1;  
       }
	 }
	 if(ArgIndex == 2)
	 {
	   DayIntervals = atoi(argv[2]);
     if ( DayIntervals <= 0)
     {
       printf("Error: DayIntervals should > 0\n");
       return 1;
     }
	 }
	 if(!strcmp("-c",argv[ArgIndex]))
	 {
	   ChipAnalysisFlag = 1;
	 }

 }

  //
  // Init the stock data struct
  //
  InitStockDailyInfoData (fp, DayIntervals);

  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //

  ReturnRecords = NULL;
 
  StartDayIndex = FIRST_DAILY_DATA_INDEX;  /*Start from Index day 22 (20,21 for analysis)*/

  EndDayIndex = StartDayIndex + DayIntervals -1;

  printf("DayIntervals = %d, Start from day index %d(Start) to %d(End)\n",DayIntervals,StartDayIndex,EndDayIndex);  

  StockSimulator (StartDayIndex, EndDayIndex, &ReturnRecords);

  //
  // Calculate profit base on the records
  //
  AnalysisProfit (ReturnRecords);

  free(InfoBuffer);
  XML_ParserFree(Parser);
  return 0;
}
