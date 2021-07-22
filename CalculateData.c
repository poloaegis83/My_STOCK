#include "DataDefine.h"

#define WriteNum(x,str,fp)          _itoa(x,str,10);\
                                    fputs(str,fp);\
                                    fputs("\n",fp);

#define WriteFloatNum(x,str,fp)     FixPoint2(x);\
                                    gcvt(x,6,str);\
                                    CheckFloatString(str);\
                                    fputs(str,fp);\
                                    fputs("\n",fp);

#define WriteFloatNumWoCL(x,str,fp) FixPoint2(x);\
                                    gcvt(x,6,str);\
                                    CheckFloatString(str);\
                                    fputs(str,fp);\


DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/

int StartCalIndex;
int len;
int len_v;
int StockId;

int         *MA_value;
int         MA_count = 0;
float       *MA_result;

int         *KD_value;
int         KD_count = 0;
float       *KD_result;

int         *MACD_value;
int         MACD_count = 0;
float       *MACD_result;

int         *RSI_value;
int         RSI_count = 0;
float       *RSI_result;

int     StartY;
int     StartM;
int     EndY;
int     EndM;

int     FixFlag;

void CalMA();
void CalMACD();
void CalRSI();
void CalKD();
void WriteCalData();


/***
 To prevet scientific notation leads gcvt error
 ex: 0.04 can casue error
***/
void FixPoint2(float Num)
{
  if (Num < 0.1 && Num > 0)
    FixFlag = 1;
  else if (Num > -0.1 && Num < 0 )
    FixFlag = 2;
  else
    FixFlag = 0;  
}
/***
 To prevet float number string like "200."
 should be "200.00"
***/
void CheckFloatString(char *Num)
{
  int i;
  char TheLastIsPoint = 0;
  char IsPoint = 0;

  i = 0;

  while(*(Num+i) != '\0')
  {
    TheLastIsPoint = 0;
    if ( *(Num+i) == '.')
    {
      TheLastIsPoint = 1;
    }
    i += 1;
  }

  if(TheLastIsPoint == 1)
  {
    *(Num+i)   = '0';
    *(Num+i+1) = '0';
    *(Num+i+2) = '\0';
  }
    
  i = 0;
  while(*(Num+i) != '\0')
  {
    if ( *(Num+i) == '.')
    {
      if(*(Num+i+2) == '\0')
      {
        *(Num+i+2) = '0';
      }
      *(Num+i+3) = '\0';
      break;
    }
    i += 1;
  }

  if(FixFlag == 1)
  {
    *(Num+3) = *(Num);
    *(Num)   = '0';
    *(Num+2) = '0';
    *(Num+4) = '\0';
  }
  if(FixFlag == 2)
  {
    *(Num+4)  = *(Num+1);
    *(Num+1)  = '0';
    *(Num+3)  = '0';
    *(Num+5)  = '\0';
  }

}    


void PrintInfo2(days)
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

int CheckInRange(int Year, int Month)
{
    if ( (Year > StartY  && Year  < EndY)    ||\
         (Year == StartY && Month >= StartM) ||\
         (Year == EndY   && Month <= EndM)  )
    {
      return 1;
    }
    else
      return 0;
}

int EqulStart(int Year, int Month)
{
    if ( Year == StartY && Month == StartM )
    {
      return 1;
    }
    else
      return 0;
}

int EqulEnd(int Year, int Month)
{
    if ( Year == EndY && Month == EndM )
      return 1;
    else
      return 0;
}

void ReadStockData(FILE *fp)
{
  char *str;
  int year,month,day;
  int record = 0;
  int i = 1;
  int     begin  = 0;
  int     finish = 0;

  DAILY_INFO *BuffInitPtr;

  str = (char*) malloc(50);

  len = FindTotalLen(fp);

  printf("len = %d\n",len);

  InfoBuffer = (DAILY_INFO*) malloc(sizeof(DAILY_INFO)*(len));

  BuffInitPtr = InfoBuffer;

  //
  // Data range [1 years ago-InputStart-InputEnd]
  //

  while(1)
  {
    ReadLine(fp,str);

    if (!strcmp("end",str))
      break;

    year = (int)atoi(str);

    ReadLine(fp,str);
    month = (int)atoi(str);

    ReadLine(fp,str);
    day = (int)atoi(str);

    if(StartY-1 == year && StartM == month && record == 0) //set start year from 1 year ago
    {
        begin = i;
        record = 1;
    }
    if( CheckInRange(year,month) ) // real calculate range
      len_v ++;

    if(EndY == year && EndM == month)
    {
        record = 2;
    }
    if(record == 2)
    {
       if (EndM != month)
       {
        finish = i-1;
        break;
       }
    }

    if(record > 0)
    {
      BuffInitPtr->Dates.Years  = year;
      BuffInitPtr->Dates.Months = month;
      BuffInitPtr->Dates.Days   = day;
      //printf("Record = %d-%d-%d- i = %d \n",year,month,day,i);
    }

    ReadLine(fp,str);
    if(record > 0)
      BuffInitPtr->Start        = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(record > 0)
      BuffInitPtr->End          = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(record > 0)
      BuffInitPtr->High         = (float)atoi(str) / 100;

    ReadLine(fp,str);
    if(record > 0)
      BuffInitPtr->Low          = (float)atoi(str) / 100;

    // Diff data below, no need to read.
    ReadLine(fp,str);
    ReadLine(fp,str);
    ReadLine(fp,str);
    ReadLine(fp,str);

    if(record > 0)
    {
      BuffInitPtr->DayIndex   = i;
      BuffInitPtr++;
    }

    i++;
  }

  len = finish - begin +1;
  printf("total = %d ,begin = %d, finish = %d, len = %d\n",i,begin,finish,len);

  if(MA_count != 0)
  {
    CalMA();
  }
  if(KD_count != 0)
  {
    CalKD();
  }
  if(MACD_count != 0)
  {
    CalMACD();
  }
  if(RSI_count != 0)
  {
    CalRSI();
  }    

  //PrintInfo2(len);

  free(str);
}


void CalMA()
{
  DAILY_INFO  *Daily;
  float   Total;
  int     MA_N;
  int     i,Dacount,count;
  float   MA_V;
  float   *ResultPtr;
  int     Record;

  MA_result = (float *) malloc(sizeof(float)*MA_count*len);
  ResultPtr = MA_result;

  for ( count = 0; count < MA_count ; count++)
  {
    MA_N   = *(MA_value+count);
    Daily  = InfoBuffer+MA_N-1;

    Record = 0;

    for ( Dacount = MA_N; Dacount <= len; Dacount++)
    {
      //printf("Dac=%d %d-%d-%d\n",Dacount,Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days);

      if(  EqulStart(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 1; //record start
      else if (  EqulEnd(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 2; //record end

      if(Record == 2 && !EqulEnd(Daily->Dates.Years, Daily->Dates.Months))
        break;

      for (i = 0; i < MA_N; i++)
      {
        Total += (Daily-i)->End;
      }

      MA_V          = Total/MA_N;

      if(Record > 0)
      {
        *ResultPtr    = MA_V;
        ResultPtr++;
        //printf("record = %f\n",MA_V);
      }

      Total = 0;
      Daily++;
    }
  }

}
void CalMACD()
{
  DAILY_INFO  *Daily;
  int         count,Dacount,i;
  float       *ResultPtr;  //OSC
  int         EMA_N1,EMA_N2,DIF_N;
  float       DI;
  float       *EMA_V1,*EMA_V2,*DIF_V,DIF,OSC; // EMA_V1 small,EMA_V2 big range EMA
  int         Record;

  MACD_result = (float *) malloc(sizeof(float)*MACD_count*len);
  ResultPtr = MACD_result;

  for ( count = 0; count < KD_count ; count++)
  {
    EMA_N1 = *( MACD_value+(count*3)   );
    EMA_N2 = *( MACD_value+(count*3)+1 );
    DIF_N  = *( MACD_value+(count*3)+2 );
    printf("EMA_N1 = %d, EMA_N2 = %d, DIF_N = %d\n",EMA_N1,EMA_N2,DIF_N);
    Daily = InfoBuffer;

    EMA_V1 = (float *) malloc(sizeof(float)*len);
    EMA_V2 = (float *) malloc(sizeof(float)*len);
    DIF_V  = (float *) malloc(sizeof(float)*len);

    Record = 0;

    for(Dacount = 1; Dacount <= len; Dacount++)
    {
      printf("Dac=%d %d-%d-%d\n",Dacount,Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days);
      if(  EqulStart(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 1; //record start
      else if (  EqulEnd(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 2; //record end
      
      if(Record == 2 && ! EqulEnd(Daily->Dates.Years, Daily->Dates.Months) )
        break;

      DI         =  (Daily->End * 2 + Daily->High + Daily->Low) / 4; 

      if(Dacount == 1)  //Default MACD
      {
        *EMA_V1     = DI;
        *EMA_V2     = DI;
        *DIF_V      = 0;
        OSC         = 0;

        if(Record > 0)
        {
          *ResultPtr = OSC;
          ResultPtr++;
        }

        EMA_V1++;
        EMA_V2++;
        DIF_V++;
        Daily++;
        continue;
      }

      *EMA_V1    = ( *(EMA_V1-1) * (EMA_N1-1) + DI * 2 ) / (EMA_N1+1);
      *EMA_V2    = ( *(EMA_V2-1) * (EMA_N2-1) + DI * 2 ) / (EMA_N2+1);
      DIF        = *EMA_V1 - *EMA_V2;
      *DIF_V     = ( *(DIF_V-1) * (DIF_N-1) + DIF*2 ) / (DIF_N+1);
      OSC        = DIF - *DIF_V;
      printf("EMA1= %f, EMA2= %f, DIF= %f, DIF_V = %f,OSC = %f\n",*EMA_V1,*EMA_V2,DIF,*DIF_V,OSC);

      if(Record > 0)
      {
        *ResultPtr = OSC;
        ResultPtr++;
      }

      EMA_V1++;
      EMA_V2++;
      DIF_V++;
      Daily++;

    }

  }

}

void CalRSI()
{
  DAILY_INFO  *Daily;
  float       Increase,Decrease;
  float       Upt,Dnt;
  int         RSI_N;
  float       RSI_V;
  int         count,Dacount,i;
  float       *ResultPtr;
  float       *Up,*Down;
  int         Record;

  RSI_result = (float *) malloc(sizeof(float)*RSI_count*len);
  ResultPtr = RSI_result;

  for ( count = 0; count < KD_count ; count++)
  {
    RSI_N = *(RSI_value+count);
    Daily = InfoBuffer + RSI_N -1;
    //printf("RSI_N = %d\n",RSI_N);
    Record = 0;

    Up   = (float *) malloc(sizeof(float)*len);
    Down = (float *) malloc(sizeof(float)*len);

    for(Dacount = RSI_N; Dacount <= len; Dacount++)
    {
      //printf("Dac=%d %d-%d-%d\n",Dacount,Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days);

      if ( EqulStart(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 1; //record start
      else if (  EqulEnd(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 2; //record end
      
      if ( Record == 2 && ! EqulEnd(Daily->Dates.Years, Daily->Dates.Months) )
        break;

      Increase = 0;
      Decrease = 0;
      if(Dacount == RSI_N)  //Default RSI
      {
        for (i = 0; i < RSI_N; i++)
        {
          if( (Daily-i-1)->End < (Daily-i)->End )
            Increase += (Daily-i)->End - (Daily-i-1)->End;
          else
            Decrease += (Daily-i-1)->End - (Daily-i)->End;
        }
        Increase = Increase/RSI_N;
        Decrease = Decrease/RSI_N;
        *Up      = Increase; 
        *Down    = Decrease;
        RSI_V = (Increase / (Increase + Decrease)) * 100;

        if(Record > 0)
        {
          *ResultPtr = RSI_V;
          ResultPtr++;   
        }

        Up++;
        Down++;
        Daily++;

        continue;
      }

      //Smooth RSI
      if( Daily->End > (Daily-1)->End ) //Increase today
      {
        Upt = *(Up-1) * (RSI_N-1) + Daily->End - (Daily-1)->End ;
        Dnt = *(Down-1) * (RSI_N-1) + 0;
      }
      else
      {
        Upt = *(Up-1) * (RSI_N-1) + 0;
        Dnt = *(Down-1) * (RSI_N-1) + (Daily-1)->End - Daily->End ;
      }

      Upt = Upt / RSI_N;
      Dnt = Dnt / RSI_N;
      *Up   = Upt;
      *Down = Dnt;
      RSI_V = (  Upt / (Upt+Dnt) )*100;

      if(Record > 0)
      {
        *ResultPtr = RSI_V;
        ResultPtr++;    
      }

      Up++;
      Down++;
      Daily++;

    }
  }

}

void CalKD()
{
  DAILY_INFO  *Daily;
  float       Highest;
  float       Lowest;
  int         i;
  int         RSV_N;
  float       RSV_V;
  int         count,Dacount;
  float       *K,*D;
  float       Old_K,Old_D;
  float       *ResultPtr;
  int         Record;  

  KD_result = (float *) malloc(sizeof(float)*KD_count*len*2); //(k,d) in one day so *2
  ResultPtr = KD_result;
  printf("KD count = %d\n",KD_count);

  for ( count = 0; count < KD_count ; count++)
  {
    RSV_N = *(KD_value+count);
    Daily = InfoBuffer + RSV_N -1;
    //printf("RSV_N = %d\n",RSV_N);
    Record = 0;

    K = (float *) malloc(sizeof(float)*len);
    D = (float *) malloc(sizeof(float)*len);

    for(Dacount = RSV_N; Dacount <= len ; Dacount++)
    {
     // printf("Dac=%d %d-%d-%d\n",Dacount,Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days);

      if(  EqulStart(Daily->Dates.Years, Daily->Dates.Months) )
      {
        Record = 1; //record start
        //printf("EqulStart\n");
      }

      else if (  EqulEnd(Daily->Dates.Years, Daily->Dates.Months) ) 
        Record = 2; //record end

      if ( Record == 2 && ! EqulEnd(Daily->Dates.Years, Daily->Dates.Months) )
      {
        break;
      }

      if(Dacount == RSV_N) // Default KD
      {
        *K = 50;
        *D = 50;
        //printf("K,D = %f,%f\n",*K,*D);
        if(Record > 0)
        {
          *ResultPtr     = *K;
          *(ResultPtr+1) = *D;
          ResultPtr += 2;
        }

        Daily++;
        K++;
        D++;

        continue;
      }

      // find highest and lowest in RSV days

      Highest = Daily->High;
      Lowest = Daily->Low;

      for(i = 0; i < RSV_N; i++)
      {
        if( (Daily-i)->High > Highest)
          Highest = (Daily-i)->High;
        if( (Daily-i)->Low < Lowest)
          Lowest = (Daily-i)->Low;
      }

      RSV_V = (Daily->End - Lowest) / (Highest - Lowest) * 100;
      Old_K = *(K-1);
      Old_D = *(D-1);
      *K = Old_K*2/3 + RSV_V / 3;
      *D = Old_D*2/3 + *K / 3;
      //printf("K,D = %f,%f\n",*K,*D);

      if(Record > 0)
      {
        *ResultPtr     = *K;
        *(ResultPtr+1) = *D;
        ResultPtr += 2;
      }

      Daily++;
      K++;
      D++;
    }
    //free(K);
    //free(D);
  }

}

void WriteCalData()
{
  int    i,j,k;
  FILE   *fp;
  char   *str;
  char   *Id;

  str = (char*) malloc(50);
  //Id  = (char*) malloc(10);

  //_itoa(StockId,Id,10);

  str = "TechResult";

  j = 0;
  while (1)
  {
    if (*(str+j) == '\0')
    {
      Id = str+j;
      break;
    }
    j++;
  }
  _itoa(StockId,Id,10);
/*
  for (k = 0; k < 5; k++)
  {
    *(str+j+k) = *(Id+k);
    if (k == 4)
      *(str+j+k) = '\0';
  }*/

  //printf("ID str = %s\n",Id);
  //printf("ID = %d\n",StockId);
  printf("str = %s\n",str);

  fp = fopen(str,"w");

  fputs("length\n",fp);
 
  WriteNum(len_v,str,fp) // all the data are same length

  // Prepare MA
  if(MA_count != 0)
  {
    fputs("MA\n",fp);
    for(i = 0; i < len_v * MA_count; i++)
    {
      WriteFloatNum(*(MA_result + i),str,fp)
    }
    fputs("end\n",fp);
  }

  // Prepare MACD
  if(MACD_count != 0)
  {    
    fputs("MACD\n",fp);
    for(i = 0; i < len_v * MACD_count; i++)
    {
      WriteFloatNum(*(MACD_result + i),str,fp)
    }
    fputs("end\n",fp);
  }

  // Prepare RSI
  if(RSI_count != 0)
  {   
    fputs("RSI\n",fp);
    for(i = 0; i < len_v * RSI_count; i++)
    {
      WriteFloatNum(*(RSI_result + i),str,fp)
    }  
    fputs("end\n",fp);
  }

  // Prepare KD
  if(KD_count != 0)
  {
    //printf("KD\n");        
    fputs("KD\n",fp);
    for(i = 0; i < len_v * KD_count; i++)
    {
      WriteFloatNumWoCL(*(KD_result + i*2 ),str,fp)
      fputs(",",fp);
      WriteFloatNum(*(KD_result + i*2 +1),str,fp)
    }    
    fputs("end\n",fp);
  }
  fclose(fp);
  free(str);

}

int main(int argc, char **argv)
{
    FILE        *fp;
    int         ArgIndex;
    char        *str,*StartDate,*EndDate;
    int         i;

    // format [filename] [Stock Id] -[MA|KD|RSI|MACD] [Number of data set] [Parameter ...] -[MA|KD|RSI|MACD] [Number of data set] [Parameter ...] ...
    for(ArgIndex = 0; ArgIndex < argc; ArgIndex++)
    {
  	  if(ArgIndex == 1)
	    {
        fp = fopen(argv[1],"r");
        if (fp == NULL) 
        {
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
        StartY = (int)atoi(str); 

        //Month
        *(str)   = *(StartDate+4);
        *(str+1) = *(StartDate+5);
        *(str+2) = '\0';
        StartM = (int)atoi(str);

        //End day
        //Year
        *(str)   = *(EndDate);
        *(str+1) = *(EndDate+1);
        *(str+2) = *(EndDate+2);
        *(str+3) = *(EndDate+3);
        *(str+4) = '\0';
        EndY = (int)atoi(str);
        //Month
        *(str)   = *(EndDate+4);
        *(str+1) = *(EndDate+5);
        *(str+2) = '\0';
        EndM = (int)atoi(str);

        printf("Start = %d-%d , End = %d-%d\n",StartY,StartM,EndY,EndM);
      }
	  }

  ReadStockData(fp);
  WriteCalData();

  free(MA_result);
  free(MACD_result);
  free(RSI_result);
  free(KD_result);  
}