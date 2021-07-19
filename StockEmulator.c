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

int         *MA_value;
int         MA_count = 0;

int         *KD_value;
int         KD_count = 0;

int         *MACD_value;
int         MACD_count = 0;

int         *RSI_value;
int         RSI_count = 0;

int         len;
int         StockId;

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
/*
int  *RuleBuy;
int  *RuleSell;
int  *RuleBuyNext;

int  *RuleBuyLen;
int  *RuleSellLen;
int  *RuleBuyNextLen;

int  RuleBuyLenTotal = 0;
int  RuleSellLenTotal = 0;
int  RuleBuyNextLenTotal = 0;

int  RuleBuyCount = 0;
int  RuleSellCount = 0;
int  RuleBuyNextCount = 0;

void LoadRule()
{
  FILE    *fp;
  char    *str;
  int     i;
  int     EndCheck;
  char    TradeMode;
  int     *RuleBuyPtr;
  int     *RuleSellPtr;
  int     *RuleBuyNextPtr;
  int     *RuleBuyLenPtr;
  int     *RuleSellLenPtr;
  int     *RuleBuyNextLenPtr;

  TradeMode     = 0;

  i = 0;
  EndCheck = 0;
  str = (char*) malloc(50);

  fp = fopen("ConditionList.stock","r");

  while(1)
  {
    ReadLine(fp,str);

    if ( !strcmp("end",str) && EndCheck == 1)  // read end twice => EOF
      break;

    if (!strcmp("end",str))  // -10 = end mark for condition
    {
      EndCheck  = 1;
      TradeMode = 0;
      continue;
    } else
      EndCheck = 0;

    if ( TradeMode == 1)
      RuleBuyLenTotal++;
    if ( TradeMode == 2)
      RuleSellLenTotal++;
    if ( TradeMode == 3)
      RuleBuyNextLenTotal++;

    if ( !strcmp("50",str) && TradeMode == 0) // buy
    {
      TradeMode = 1;
      RuleBuyCount++;
    }

    if ( !strcmp("51",str) && TradeMode == 0) // sell
    {
      TradeMode = 2;
      RuleSellCount++;
    }

    if ( !strcmp("52",str) && TradeMode == 0) // buy next
    {
      TradeMode = 3;
      RuleBuyNextCount++;
    }
  
  }

 printf("RuleBuyLenT,RuleSellLenT,RuleBuyNextLenT=%d,%d,%d\n",RuleBuyLenTotal,RuleSellLenTotal,RuleBuyNextLenTotal);

  RuleBuyLenTotal += RuleBuyCount;
  RuleSellLenTotal += RuleSellCount;
  RuleBuyNextLenTotal += RuleBuyNextCount;

  RuleBuy     = (int *) malloc(RuleBuyLenTotal*sizeof(int));
  RuleSell    = (int *) malloc(RuleSellLenTotal*sizeof(int));
  RuleBuyNext = (int *) malloc(RuleBuyNextLenTotal*sizeof(int));

  RuleBuyLen     = (int *) malloc(RuleBuyCount*sizeof(int));
  RuleSellLen    = (int *) malloc(RuleSellCount*sizeof(int));
  RuleBuyNextLen = (int *) malloc(RuleBuyNextCount*sizeof(int));

  RuleBuyLenPtr     = RuleBuyLen;
  RuleSellLenPtr    = RuleSellLen;
  RuleBuyNextLenPtr = RuleBuyNextLen;

  fseek(fp, 0, SEEK_SET);
  printf("==========\n");

  while(1)
  {
    ReadLine(fp,str);

    if ( !strcmp("end",str) && EndCheck == 1)  // read end twice => EOF
      break;

    if (!strcmp("end",str))
    {
      EndCheck = 1;
      if ( TradeMode == 1)
      {
        printf("RuleBuyLen = %d\n",*RuleBuyLen);
        RuleBuyLenPtr++;
      }

      if ( TradeMode == 2)
      {
        printf("RuleSellLen = %d\n",*RuleSellLen);        
        RuleSellLenPtr++;
      }

      if ( TradeMode == 3)
      {
        printf("RuleNextLen = %d\n",*RuleBuyNextLen);        
        RuleBuyNextLenPtr++;
      }

      TradeMode = 0;
      continue;
    } else
      EndCheck = 0;

    if ( TradeMode == 1)
      *RuleBuyLenPtr += 1;

    if ( TradeMode == 2)
      *RuleSellLenPtr += 1;

    if ( TradeMode == 3)
      *RuleBuyNextLenPtr += 1;


    if ( !strcmp("50",str) && TradeMode == 0) // buy
    {
      *RuleBuyLenPtr = 0;
      TradeMode = 1;
    }
    if ( !strcmp("51",str) && TradeMode == 0) // sell
    {
      *RuleSellLenPtr = 0;
      TradeMode = 2;
    }

    if ( !strcmp("52",str) && TradeMode == 0) // buy next
    {
      *RuleBuyNextLenPtr = 0;
      TradeMode = 3;
    }

  }

  RuleBuyPtr     = RuleBuy;
  RuleSellPtr    = RuleSell;
  RuleBuyNextPtr = RuleBuyNext;

  RuleBuyLenPtr     = RuleBuyLen;
  RuleSellLenPtr    = RuleSellLen;
  RuleBuyNextLenPtr = RuleBuyNextLen;

  fseek(fp, 0, SEEK_SET);

  while(1)
  {
    ReadLine(fp,str);

    if ( !strcmp("end",str) && EndCheck == 1)  // read end twice => EOF
      break;

    if (!strcmp("end",str))  // -10 = end mark for condition
    {
      EndCheck = 1;
      TradeMode = 0;
      continue;
    } else
      EndCheck = 0;

    if ( TradeMode == 1)
    {
      *RuleBuyPtr = (int)atoi(str);
      RuleBuyPtr++;
    }
    if ( TradeMode == 2)
    {
      *RuleSellPtr = (int)atoi(str);
      RuleSellPtr++;
    }
    if ( TradeMode == 3)
    {
      *RuleBuyNextPtr = (int)atoi(str); 
      RuleBuyNextPtr++;
    }

    if ( !strcmp("50",str) && TradeMode == 0) // buy
    {
      *RuleBuyPtr = *RuleBuyLenPtr;
      RuleBuyPtr++;
      RuleBuyLenPtr++;
      TradeMode = 1;
    }
    if ( !strcmp("51",str) && TradeMode == 0) // sell
    {
      *RuleSellPtr = *RuleSellLenPtr;
      RuleSellPtr++;
      RuleSellLenPtr++;
      TradeMode = 2;
    }

    if ( !strcmp("52",str) && TradeMode == 0) // buy next
    {
      *RuleBuyNextPtr= *RuleBuyNextLenPtr;
      RuleBuyNextPtr++;
      RuleBuyNextLenPtr++;
      TradeMode = 3;
    }

  }

  fclose(fp);

  printf("BuyRule:\n");
  for (i = 0; i < RuleBuyLenTotal; i++)
  {
    printf("%d\n",*(RuleBuy+i));
  }
  printf("SellRule:\n");
  for (i = 0; i < RuleSellLenTotal; i++)
  {
    printf("%d\n",*(RuleSell+i));
  }
  printf("BuyNextRule:\n");
  for (i = 0; i < RuleBuyNextLenTotal; i++)
  {
    printf("%d\n",*(RuleBuyNext+i));
  }
}
/*
// For > < = crossup crsoodown
void ConditionParsing(int *Rule, int **SplitRet, int **ConditionRet, int len)
{
  int *RulePtr;
  int *Split;
  int *SplitPtr;
  int *ConditionList;
  int i;

  SplitPtr     = (int *) malloc(100*sizeof(int));
  ConditionRet = (int *) malloc(100*sizeof(int));

  //Find  > < = crossup crsoodown
  RulePtr++; // Skip shares

  while(i < len)
  {
    if ( (*RulePtr >= 20 && *RulePtr <= 24) || *RulePtr == 29 || *RulePtr == 30 )//  > < = crossup crsoodown
    {
      if(*RulePtr == 20)
        *ConditionList = 1;
      else if(*RulePtr == 21)
        *ConditionList = 2;
      else if(*RulePtr == 22)
        *ConditionList = 3;        
      else if(*RulePtr == 23)
        *ConditionList = 4;        
      else if(*RulePtr == 24)
        *ConditionList = 5;        
      else if(*RulePtr == 29)
        *ConditionList = 6;        
      else if(*RulePtr == 30)
        *ConditionList = 7;        

      ConditionList++;
      SplitPtr++;
      *SplitPtr = -99;
    }

    *SplitPtr = *RulePtr;

    RulePtr++;
    SplitPtr++;
    i++;
  }

  SplitPtr++;
  *SplitPtr = -99;

  *SplitRet = SplitPtr;
  *ConditionRet = ConditionList;
}

void AndOrParsing(int *Rule, int **SplitRet, int **AndOrRet, int len)
{
  // do first with bracket 

  // split [statement] and [statement]
  // save each statement to array

  // After finish satement in bracket
  // then do it all again
  int *RulePtr;
  int *Split;
  int *SplitPtr;
  int *AndOrList;
  int i;

  SplitPtr = (int *) malloc(100*sizeof(int));
  AndOrRet = (int *) malloc(100*sizeof(int));

  i = 0;
  RulePtr = Rule;
  SplitPtr = Split;

  //Find And Or
  RulePtr++; // Skip shares

  while(i < len)
  {
    if(*RulePtr == 33 || *RulePtr == 34) // and or
    {
      if(*RulePtr == 33)
        *AndOrList = 1;
      else
        *AndOrList = 0;

      AndOrList++;
      SplitPtr++;
      *SplitPtr = -99;
    }

    *SplitPtr = *RulePtr;

    RulePtr++;
    SplitPtr++;
    i++;
  }

  SplitPtr++;
  *SplitPtr = -99;

  *SplitRet = SplitPtr;
  *AndOrRet = AndOrList;
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
  char        *str,*StartDate,*EndDate;
  int         i;

  // 
  // Argument format:
  // StockEmulator.exe [XmlFileName] [Days] -c(ChipAnalysisFlag on)
  //

  //LoadRule();
  MainProcess2();
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
      StockId = (int)atoi(argv[ArgIndex]); 
      if(StockId < 0 || StockId > 9999)
      {
        printf("Id not correct!\n");
        return 1;
      } 
    }

    if(!strcmp("-MA",argv[ArgIndex]))
    {
      MA_count = atoi(argv[ArgIndex+1]);
      MA_value = (int*) malloc(sizeof(int)*MA_count);
      for(i = 0; i < MA_count; i++)
      {
        *(MA_value+i) = atoi( argv[ArgIndex+i +2] );
        printf("MA, N = %d\n",*(MA_value+i));    
      }
    }

    if(!strcmp("-KD",argv[ArgIndex]))
    {
      KD_count = atoi(argv[ArgIndex+1]);
      KD_value = (int*) malloc(sizeof(int)*KD_count);
      for(i = 0; i < KD_count; i++)
      {
        *(KD_value+i ) = atoi( argv[ArgIndex+i+2] );
        printf("KD, RSV_N = %d\n",*(KD_value+i));          
      }
    }

    if(!strcmp("-RSI",argv[ArgIndex]))
    {
      RSI_count = atoi(argv[ArgIndex+1]);
      RSI_value = (int*) malloc(sizeof(int)*RSI_count);
      for(i = 0; i < RSI_count; i++)
      {
        *(RSI_value+i) = atoi( argv[ArgIndex+i +2] );
        printf("RSI, RSI_N = %d\n",*(RSI_value+i));
      }
    }

    if(!strcmp("-MACD",argv[ArgIndex]))
    {
      MACD_count = atoi(argv[ArgIndex+1]);
      MACD_value = (int *)malloc(sizeof(int)*MACD_count*3);
      for(i = 0; i < MACD_count; i++)
      {
        *(MACD_value+i)    = atoi( argv[ArgIndex + i*3 +2] );
        *(MACD_value+i +1) = atoi( argv[ArgIndex + i*3 +2 +1] );
        *(MACD_value+i +2) = atoi( argv[ArgIndex + i*3 +2 +2] );
        printf("MACD, EMA_N1,EMA_N2,DIF_N = %d,%d,%d \n",*(MACD_value+i),*(MACD_value+i+1),*(MACD_value+i+2));
      }
    }
    if(!strcmp("-Range",argv[ArgIndex]))
    {
      str = (char*) malloc(50);

      StartDate = argv[ArgIndex+1];
      EndDate   = argv[ArgIndex+2];
      printf("S=%s,E=%s\n",StartDate,EndDate);
      //
      // Read StartDay and EndDay from argument
      //
      //Start day
      //Year
      *(str)   = *(StartDate);
      *(str+1) = *(StartDate+1);
      *(str+2) = *(StartDate+2);
      *(str+3) = *(StartDate+3);
      *(str+4) = '\0';
      StartYear = (int)atoi(str); 

      //Month
      *(str)   = *(StartDate+4);
      *(str+1) = *(StartDate+5);
      *(str+2) = '\0';
      StartMonth = (int)atoi(str);

      //End day
      //Year
      *(str)   = *(EndDate);
      *(str+1) = *(EndDate+1);
      *(str+2) = *(EndDate+2);
      *(str+3) = *(EndDate+3);
      *(str+4) = '\0';
      EndYear = (int)atoi(str);
      //Month
      *(str)   = *(EndDate+4);
      *(str+1) = *(EndDate+5);
      *(str+2) = '\0';
      EndMonth = (int)atoi(str);

      printf("Start = %d-%d , End = %d-%d\n",StartYear,StartMonth,EndYear,EndMonth);
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