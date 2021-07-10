#include "DataDefine.h"

char DebugFlag = 0;

void InitStockDailyInfoData (FILE *fp);
void CalculateMA (int days, int *result);
void CalculateRSI (int days);
void CalculateKDJ (int days);
void CalculateMACD (int days);

int           StartYear;
int           EndYear;
int           StartMonth;
int           EndMonth;
int           StartDay;
int           EndDay;
int           len;
int         TheStockID;
DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/
DAILY_INFO  *BuffInitPtr;


void PrintInfo(int days)
{
  DAILY_INFO  *Daily;
  int         DailyIndex;
  
  Daily = InfoBuffer;
  
  printf("\n=================================================\n");
  for(DailyIndex = 1; DailyIndex <= days ; DailyIndex++)
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

void ReadLine(FILE *fp,char *str)
{
  fscanf(fp,"%s",str);
}

int FindTotalLen(FILE *fp)
{
  char  *str;
  int   counter = 0;

  str = (char *) malloc(50);

  while(1)
  {
    ReadLine(fp,str);
    if (!strcmp("end",str))
      break;
    counter++;
  }

  fseek(fp, 0, SEEK_SET);

  return counter/11;
}

void InitStockDailyInfoData(FILE *fp)
{
  char *str;

  int i = 1;
  int Year,Month;
  int Record = 0;
  
  str = (char*) malloc(50);

  len = FindTotalLen(fp);

  InfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(len));

  BuffInitPtr = InfoBuffer;

  while(1)
  {
    printf("i = %d ",i);
    ReadLine(fp,str);

    if (!strcmp("end",str))
      break;
    Year  = (int)atoi(str);

    ReadLine(fp,str);
    Month = (int)atoi(str);

    if ( (Year > StartYear  && Year  < EndYear)    ||\
         (Year == StartYear && Month > StartMonth) ||\
         (Year == EndYear   && Month < EndMonth)  )
    {
      BuffInitPtr->Dates.Years  = Year;
      BuffInitPtr->Dates.Months = Month;
      Record = 1;
    }
    else
      Record = 0;

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->Dates.Days   = (int)atoi(str);

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->Start        = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->End          = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->High         = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->Low          = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->InvestmentTrustDiff  = (int)atoi(str);

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->DealersDiff          = (int)atoi(str);

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->ForeignInvestorsDiff = (int)atoi(str);

    ReadLine(fp,str);
    if(Record)
      BuffInitPtr->LeaderDiff           = (int)atoi(str);

    if(Record)
      BuffInitPtr->DayIndex             = i;

    BuffInitPtr++;
    i++;
  }
  
  len = i;

  PrintInfo(len);

  free(str);
}

/*
void CalculateMA(int day, int *Result)
{
    DAILY_INFO  *Daily;
    float   Total = 0;
    int     i,count;
    float   *ResultPtr;

    i = 0;
    count = 0;

    Daily     = InfoBuffer;

    Result = (float *) malloc(sizeof(int)*(len));

    ResultPtr = Result;

    Daily     = InfoBuffer;

    for ( count = 0; count <= len; count++)
    {
      for (i = 0; i < day; i++)
      {
        Total += (Daily-i)->End;
      }
      *ResultPtr    = Total/day;

      Total = 0;
      ResultPtr++;
      Daily++;
    }
}

void CalculateMACD(int days, int *Result)
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

void CalculateRSI(int days, int *Result)
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

  Daily6    = InfoBuffer + 7 -1;  //Day Index 7
  Daily12   = InfoBuffer + 13 -1; //Day Index 13


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

void CalculateKDJ(int days, int *Result)
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
  Daily   = InfoBuffer + RSV_n -1; //(Day number 9)

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
    for (i = 1; i < RSV_n; i++) //To pervious 8 days
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
    Daily->KDJ.K = ((Daily-1)->KDJ.K * 2 / 3) +  (Daily->KDJ.RSV / 3);  //Pervious K * 2/3 + todays RSV *1/3 
    Daily->KDJ.D = ((Daily-1)->KDJ.D * 2 / 3) +  (Daily->KDJ.K /3);     //Pervious D * 2/3 + todays K   *1/3 
    Daily->KDJ.J = Daily->KDJ.K * 3 - Daily->KDJ.D * 2;

    Daily += 1;
  }

}
*/
int main(int argc, char **argv)
{
  char          *Str;
  char          *Str2;
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

  Str2 = (char*) malloc(50);

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
	   Str = argv[2];

     *Str2     = *Str;
     *(Str2+1) = *(Str+1);
     *(Str2+2) = *(Str+2);
     *(Str2+3) = *(Str+3);
     *(Str2+4) = *(Str+4);
     *(Str2+5) = '\n';
     StartYear  = atoi(Str2);

     *(Str2+1) = *(Str+5);
     *(Str2+2) = *(Str+6);
     *(Str2+3) = '\n';
     StartMonth = atoi(Str2);

     *(Str2+1) = *(Str+7);
     *(Str2+2) = *(Str+8);
     *(Str2+3) = '\n';    
     StartDay   = atoi(Str2);
	 }

  if(ArgIndex == 3)
	 {
	   Str = argv[3];

     *Str2     = *Str;
     *(Str2+1) = *(Str+1);
     *(Str2+2) = *(Str+2);
     *(Str2+3) = *(Str+3);
     *(Str2+4) = *(Str+4);
     *(Str2+5) = '\n';
     EndYear  = atoi(Str2);

     *(Str2+1) = *(Str+5);
     *(Str2+2) = *(Str+6);
     *(Str2+3) = '\n';
     EndMonth = atoi(Str2);

     *(Str2+1) = *(Str+7);
     *(Str2+2) = *(Str+8);
     *(Str2+3) = '\n';    
     EndDay   = atoi(Str2);     
	 }
  
  }

  //
  // Init the stock data struct
  //
  //InitStockDailyInfoData2(fp);

  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //

  ReturnRecords = NULL;


  //StockSimulator1 (StartDayIndex, EndDayIndex, &ReturnRecords);
  //StockSimulator2 (StartDayIndex, EndDayIndex, &ReturnRecords2);
  //StockSimulator3

  //
  // Calculate profit base on the records
  //
  //AnalysisProfit (ReturnRecords);
  //AnalysisProfit2 (ReturnRecords2);
  //free(InfoBuffer);
  //XML_ParserFree(Parser);
  return 0;
}