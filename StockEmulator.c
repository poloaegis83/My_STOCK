#include "expat.h"
#include "DataDefine.h"

char DebugFlag = 0;

void InitStockDailyInfoData (FILE *fp, int days);
void CalculateMA (int days);
void CalculateRSI (int days);
void CalculateKDJ (int days);
void CalculateMACD (int days);
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
      BuffInitPtr->Start         = (float)atoi(Value) / 100;
	  }
	  if(!strcmp("High",attribute[i]))
	  {
	    BuffInitPtr->High          = (float)atoi(Value) / 100;
	  }
	  if(!strcmp("Low",attribute[i]))
	  {
	    BuffInitPtr->Low           = (float)atoi(Value) / 100; 
	  }
	  if(!strcmp("End",attribute[i]))
	  {
	    BuffInitPtr->End           = (float)atoi(Value) / 100;
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
  
  printf("\n=================================================\n");
  for(DailyIndex = 0; DailyIndex < days+FIRST_DAILY_DATA_INDEX -1 ; DailyIndex++)
  {
    printf("======================For:%d=====================\n\n",DailyIndex);    
    printf("DayIndex(ID:%d)   %d/%d/%d            = %d\n",Daily->StockID,Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,Daily->DayIndex);
    printf("Start,High,Low,End                      = %.1f,%.1f,%.1f,%.1f\n",Daily->Start,Daily->High,Daily->Low,Daily->End);
    printf("Diff Leader,Foreign,Investment,Dealers  = %d,%d,%d,%d\n",Daily->LeaderDiff,Daily->ForeignInvestorsDiff,Daily->InvestmentTrustDiff,Daily->DealersDiff);
    printf("MA5,10,20,60                            = %.f,%.1f,%.1f,%.1f\n",Daily->MA.MA5,Daily->MA.MA10,Daily->MA.MA20,Daily->MA.MA60);
    printf("K,D,J                                   = %.1f, %.1f, %.1f\n",Daily->KDJ.K, Daily->KDJ.D, Daily->KDJ.J);
    printf("RSI(6),RSI(12)                          = %.1f, %.1f\n",Daily->RSI.RSI_6,Daily->RSI.RSI_12);	
    printf("DIF,EMA12,EMA26,MACD9,OSC               = %.1f, %.1f, %.1f, %.1f, %.1f\n",Daily->MACD.DIF,Daily->MACD.EMA12,Daily->MACD.EMA26,Daily->MACD.MACD9,Daily->MACD.OSC);	 
	  Daily += 1;
  }
  printf("\n=================================================\n");
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
  // Parsing XML data and write into DailyInfoBuffer
  //
  // Call parser
  //
  if (! XML_ParseBuffer(Parser, FileLens, FileLens == 0)) {
    /* handle parse error */
  }
  DEBUG("Parsing finished\n");  

  //
  // Calculate MA, KD, RSI and MACD then write into DailyInfoBuffer
  //
  CalculateMA (days);

  CalculateRSI (days);

  CalculateKDJ (days);
  
  CalculateMACD (days);

  PrintInfo(days);
  
  DEBUG("InitStockDailyInfoData End\n");   
}

void CalculateMA(int days)
{
  //
  // Calculate MA data write into DAILY_INFO.
  //
  float         Price5;
  float         Price10;
  float         Price20;
  float         Price60;
  float         Price120;   
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

void CalculateMACD(int days)
{
  DAILY_INFO  *Daily;
  int         DailyCounter;
  float       DIF,EMA12,EMA26,MACD9;
  int         i, j , EMA_12MAX = 12;

  Daily = InfoBuffer; 

  Daily->MACD.EMA12 = Daily->End;
  Daily->MACD.EMA26 = Daily->End;
  Daily->MACD.DIF   = 0;
  Daily->MACD.MACD9 = 0;
  Daily->MACD.OSC = Daily->MACD.DIF - Daily->MACD.MACD9;

  Daily += 1;

  for(DailyCounter = 1; DailyCounter < days + (FIRST_DAILY_DATA_INDEX) - 1; DailyCounter++)
  {
    EMA12 = ( (Daily-1)->MACD.EMA12 * 11 + (Daily->End) * 2 ) /13;
    EMA26 = ( (Daily-1)->MACD.EMA26 * 25 + (Daily->End) * 2 ) /27;
    DIF   = EMA12 - EMA26;
    MACD9 = ( (Daily-1)->MACD.MACD9 * 8 + DIF * 2 ) /10;
    Daily->MACD.EMA12    = EMA12;
    Daily->MACD.EMA26    = EMA26;
    Daily->MACD.DIF      = DIF;
    Daily->MACD.MACD9    = MACD9;
    Daily->MACD.OSC = DIF - MACD9;

    Daily += 1;
  }

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
  float       Increase,Decrease;
  float       Upt,Dnt;

  Daily6    = InfoBuffer + 7 -1;  /*Day Index 7*/
  Daily12   = InfoBuffer + 13 -1; /*Day Index 13*/


  for(DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX -7); DailyCounter++)
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

  for(DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX -13); DailyCounter++)
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

  float        Highest;
  float        Lowest;
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

  for (DailyCounter = 0; DailyCounter < days + (FIRST_DAILY_DATA_INDEX - RSV_n); DailyCounter++)
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
    Daily->KDJ.RSV = (Daily->End - Lowest) / (Highest - Lowest) * 100;
    Daily->KDJ.K = ((Daily-1)->KDJ.K * 2 / 3) +  (Daily->KDJ.RSV / 3);  /*Pervious K * 2/3 + todays RSV *1/3 */
    Daily->KDJ.D = ((Daily-1)->KDJ.D * 2 / 3) +  (Daily->KDJ.K /3);     /*Pervious D * 2/3 + todays K   *1/3 */
    Daily->KDJ.J = Daily->KDJ.K * 3 - Daily->KDJ.D * 2;

    Daily += 1;
  }

}

int main(int argc, char **argv)
{
  int           DayIntervals;    //Tatol days for emulator
  int           StartDayIndex;
  int           EndDayIndex;
  TRADE_RECORD  *ReturnRecords;
  TRADE_RECORD2 *ReturnRecords2;  
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
 
  StartDayIndex = FIRST_DAILY_DATA_INDEX;  /*Start from Index day 62 (60, 61 for analysis)*/

  EndDayIndex = StartDayIndex + DayIntervals -1;

  printf("DayIntervals = %d, Start from day index %d(Start) to %d(End)\n",DayIntervals,StartDayIndex,EndDayIndex);  

  //StockSimulator1 (StartDayIndex, EndDayIndex, &ReturnRecords);
  StockSimulator2 (StartDayIndex, EndDayIndex, &ReturnRecords2);
  //
  // Calculate profit base on the records
  //
  //AnalysisProfit (ReturnRecords);
  AnalysisProfit2 (ReturnRecords2);
  free(InfoBuffer);
  XML_ParserFree(Parser);
  return 0;
}