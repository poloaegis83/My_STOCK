#include "DataDefine.h"

char DebugFlag = 0;

void InitStockDailyInfoData (FILE *fp);

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
/*
void ReadLine(FILE *fp,char *str)
{
  fscanf(fp,"%s",str);
}
*/
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

  MainProcess();

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
  //InitStockDailyInfoData(fp);

  //
  // Emulator for (StartDayIndex - EndDayIndex) Days Interval
  //

  ReturnRecords = NULL;


  //StockSimulator (StartDayIndex, EndDayIndex, &ReturnRecords);

  //
  // Calculate profit base on the records
  //
  //AnalysisProfit (ReturnRecords);

  //free(InfoBuffer);
  //XML_ParserFree(Parser);
  return 0;
}