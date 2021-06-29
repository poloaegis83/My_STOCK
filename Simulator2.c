#include "DataDefine.h"

#define MOVE_TO_X_DAYS_AGO(x)     Sim2Curr-=x;
#define MOVE_TO_X_DAYS_FORWORD(x) Sim2Curr+=x;
#define WriteDates(x,y,z,str,fp)  _itoa(x,str,10);\
                                  fputs(str,fp);\
                                  fputs("/",fp);\
                                  _itoa(y,str,10);\
                                  fputs(str,fp);\
                                  fputs("/",fp);\
                                  _itoa(z,str,10);\
                                  fputs(str,fp);\
                                  fputs("\n",fp);

#define WriteNum(x,str,fp)        _itoa(x,str,10);\
                                  fputs(str,fp);\
                                  fputs("\n",fp);

#define WriteFloatNum(x,str,fp)   gcvt(x,6,str);\
                                  fputs(str,fp);\
                                  fputs("\n",fp);                                  

char           OpenOrClose;
DAILY_INFO     *Sim2Curr;
TRADE_RECORD2  *Record2Head = NULL;
TRADE_RECORD2  *Record2Current = NULL;
int            gID;

void MA5Cross20Alg();

void StrIDAppend(char *StringData,int NewId)
{
  char *New;
  char *Ptr;
  char i;

  New = (char*) malloc(5);

  Ptr = StringData;
  while( *Ptr != '\0' )
  {
    Ptr += 1;
  }

  _itoa(NewId,New,10);

  for(i = 0; i < 4; i++){
    *Ptr = *(New+i);
     Ptr += 1;
  }
    *Ptr = '\0';
  printf("+STRING = %s\n",StringData);  
}

float PRICE()
{
  if(OpenOrClose)
  {
    return Sim2Curr->End;
    //printf("PRICE() = %.1f", Sim2Curr->End);
  } else
  {
    return Sim2Curr->Start;
    //printf("PRICE() = %.1f", Sim2Curr->End);
  }
}

float LAST_PRICE(char* Type,int Days)
{
  if(!strcmp("high",Type))
  {
    return (Sim2Curr-Days)->High;
  }
  if(!strcmp("low",Type))
  {
    return (Sim2Curr-Days)->Low;
  }
  if(!strcmp("start",Type))
  {
    return (Sim2Curr-Days)->Start;
  }
  if(!strcmp("end",Type))
  {
    return (Sim2Curr-Days)->End;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_PRICE");
  return 0;    
}

void BuyOrSell(char Options, char *Type, int Shares)
{
  TRADE_RECORD2  *Record;
  float Price;
  int   DayIndex;
  DATE  Dates;

  if(!strcmp("Now",Type))
  {
    Price    = PRICE();
    printf("price = %f\n",Price);
    DayIndex = Sim2Curr->DayIndex;
    Dates    = Sim2Curr->Dates;    
  }
  if(!strcmp("NextBarStart",Type))
  {
    Price    = (Sim2Curr+1)->Start;
    DayIndex = (Sim2Curr+1)->DayIndex;
    Dates    = (Sim2Curr+1)->Dates;       
  }
  if(!strcmp("NextBarEnd",Type))
  {
    Price    = (Sim2Curr+1)->End;
    DayIndex = (Sim2Curr+1)->DayIndex;
    Dates    = (Sim2Curr+1)->Dates;       
  }

  if (Record2Head == NULL)
  {
    if(Options) //Block sell with no shares remain
    {
      Record = (TRADE_RECORD2 *) malloc(sizeof(TRADE_RECORD2));
      Record->Price    = Price;
      Record->DayIndex = DayIndex;
      Record->Dates     = Dates; 

      Record->BuyOrSell = 1;
      Record->ShareTrades = Shares;  
      Record->SharesRemaining = Shares;
      Record->Next     = NULL;
      Record2Head    = Record;
      Record2Current = Record;      
    }
    return;
  }

  if(!Options) //Block sell with no shares remain
  {
    if(Record2Current->SharesRemaining < Shares)
    {
      // Shares remain not enough
      return;
    }
  } else  //Block buy with shares remain
  {
    if(Record2Current->SharesRemaining != 0)
    {
      // Shares remains
      return;
    }
  }

  Record = (TRADE_RECORD2 *) malloc(sizeof(TRADE_RECORD2));

  Record->Price    = Price;
  Record->DayIndex = DayIndex;
  Record->Dates    = Dates; 

  Record->BuyOrSell = Options ? 1 : 0;
  Record->ShareTrades = Shares;
  Record->Next     = NULL;


  if(Options)
  {
  Record->SharesRemaining = Record2Current->SharesRemaining + Shares;
  } else
  {
  Record->SharesRemaining = Record2Current->SharesRemaining - Shares;
  }

  Record2Current->Next = Record;
  Record2Current = Record;
}

void Buy(char *Type, int Shares)
{
  BuyOrSell(1,Type,Shares);
}

void Sell(char *Type, int Shares)
{
  BuyOrSell(0,Type,Shares);
}

float LAST_MA(char* Type, int Days)
{
  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  if(!strcmp("5",Type))
  {
    return (Sim2Curr-Days)->MA.MA5;
  }
  if(!strcmp("10",Type))
  {
    return (Sim2Curr-Days)->MA.MA10;
  }
  if(!strcmp("20",Type))
  {
    return (Sim2Curr-Days)->MA.MA20;
  }
  if(!strcmp("60",Type))
  {
    return (Sim2Curr-Days)->MA.MA60;
  }
  if(!strcmp("120",Type))
  {
    return (Sim2Curr-Days)->MA.MA120;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_MA");
  return 0;    
}

float LAST_KDJ(char* Type,int Days)
{
  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }
  
  if(!strcmp("K",Type))
  {
    return (Sim2Curr-Days)->KDJ.K;
  }
  if(!strcmp("D",Type))
  {
    return (Sim2Curr-Days)->KDJ.D;
  }
  if(!strcmp("J",Type))
  {
    return (Sim2Curr-Days)->KDJ.J;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_KDJ");
  return 0;  
}

float LAST_RSI(char* Type,int Days)
{
  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  if(!strcmp("6",Type))
  {
    return (Sim2Curr-Days)->RSI.RSI_6;
  }
  if(!strcmp("12",Type))
  {
    return (Sim2Curr-Days)->RSI.RSI_12;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_RSI");
  return 0;
}

float LAST_MACD(char* Type,int Days)
{
  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  if(!strcmp("DIF",Type))
  {
    return (Sim2Curr-Days)->MACD.DIF;
  }

  if(!strcmp("EMA12",Type))
  {
    return (Sim2Curr-Days)->MACD.EMA12;
  }

  if(!strcmp("EMA26",Type))
  {
    return (Sim2Curr-Days)->MACD.EMA26;
  }

  if(!strcmp("MACD9",Type))
  {
    return (Sim2Curr-Days)->MACD.MACD9;
  }

  if(!strcmp("OSC",Type))
  {
    return (Sim2Curr-Days)->MACD.OSC;
  }  

  //
  // Should not reach here!
  //
  printf("Error:LAST_MACD");
  return 0;
}

int LAST_DIFF(char* Type, int Days)
{
  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }
  if(!strcmp("leader",Type))
  {
    return (Sim2Curr-Days)->LeaderDiff;
  }
  if(!strcmp("foreign",Type))
  {
    return (Sim2Curr-Days)->ForeignInvestorsDiff;
  }
  if(!strcmp("investment",Type))
  {
    return (Sim2Curr-Days)->InvestmentTrustDiff;
  }
  if(!strcmp("dealers",Type))
  {
    return (Sim2Curr-Days)->DealersDiff;
  }

  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  return 0;  
}

float MA (char* Type)
{
  float        New5MA;
  float        New10MA;
  float        New20MA;
  float        New60MA;
  float        NewPrice;

  NewPrice = PRICE();

  if(!strcmp("5",Type))
  {
    New5MA  = (Sim2Curr-1)->MA.MA5 * 5;
    New5MA -= (Sim2Curr-5)->End;
    New5MA  = (New5MA + NewPrice)/5;
    return New5MA;
  }
  if(!strcmp("10",Type))
  {
    New10MA  = (Sim2Curr-1)->MA.MA10 * 10;
    New10MA -= (Sim2Curr-10)->End;
    New10MA  = (New10MA + NewPrice)/10;
    return New10MA;
  }
  if(!strcmp("20",Type))
  {
    New20MA  = (Sim2Curr-1)->MA.MA20 * 20;
    New20MA -= (Sim2Curr-20)->End;
    New20MA  = (New20MA + NewPrice)/20;
    return New20MA;
  }
  if(!strcmp("60",Type))
  {
    New60MA  = (Sim2Curr-1)->MA.MA60 * 60;
    New60MA -= (Sim2Curr-60)->End;
    New60MA  = (New60MA + NewPrice)/60;
    return New60MA;
  }
  if(!strcmp("120",Type))
  {

  }

  //
  // Should not reach here!
  //
  printf("Error:MA");
  return 0;   
}

float KDJ(char* Type)
{
  float      NewPrice;
  float      Highest,Lowest;
  int        i,RSV_n;
  float      RSV,K,D,J;
  //
  // Re-calculator KDJ(9)
  //
  RSV_n = 9;

  NewPrice = PRICE();

  //
  // Find Highest and Lowest in last 9 days
  //
  if(OpenOrClose)
  {
    Highest = Sim2Curr->High;
    Lowest  = Sim2Curr->Low;
  }
  else
  {
    Highest = NewPrice;
    Lowest  = NewPrice;
  }
  //printf("before Highest = %f ,Lowest = %f \n",Highest,Lowest);

  for (i = 1; i < RSV_n; i++)
  {
    if (LAST_PRICE("high",i) > Highest) //( (Sim2Curr-i)->High > Highest)
    {
      Highest = LAST_PRICE("high",i); //(Sim2Curr-i)->High;
    }
    if( LAST_PRICE("low",i) < Lowest ) //(Sim2Curr-i)->Low < Lowest )
    {
      Lowest = LAST_PRICE("low",i); //(Sim2Curr-i)->Low;
    }
  }
 
  RSV = (NewPrice - Lowest) / (Highest - Lowest) * 100;
  //printf("LAST_KDJ(K) = %f, LAST_KDJ(D) = %f, RSV = %f ,after  Highest = %f ,Lowest = %f \n",LAST_KDJ("K"),LAST_KDJ("D"),RSV,Highest,Lowest);
  K   = LAST_KDJ("K",1) * 2 / 3;
  K  += RSV / 3;
  D   = LAST_KDJ("D",1) * 2 / 3;
  D  += K / 3;
  J   = 3 * K - 2 * D;

  if(!strcmp("K",Type))
  {
    return K;
  }
  if(!strcmp("D",Type))
  {
    return D;
  }
  if(!strcmp("J",Type))
  {
    return J;
  }
  //
  // Should not reach here!
  //
  printf("Error:KDJ");
  return 0;
}

float RSI(char* Type)
{
  int     NewPrice, LastPrice;
  float   Upt, Dnt;
  float   RSI6, RSI12;

  //
  // Re-calculator RSI6, RSI12
  //  
  NewPrice = PRICE();
  LastPrice = LAST_PRICE("end",1);

  if(NewPrice > LastPrice)
  {
    Upt = (Sim2Curr-1)->RSI.Up6 * 5  + NewPrice - LastPrice;
    Dnt = (Sim2Curr-1)->RSI.Down6 * 5 + 0;
  } else
  {
    Upt = (Sim2Curr-1)->RSI.Up6 * 5 + 0;
    Dnt = (Sim2Curr-1)->RSI.Down6 *5 + LastPrice - NewPrice ;
  }
  Upt = Upt/6;
  Dnt = Dnt/6;
  RSI6 = ( Upt / (Upt+Dnt) )*100;

  if(NewPrice > LastPrice)
  {
    Upt = (Sim2Curr-1)->RSI.Up12 * 11  + NewPrice - LastPrice;
    Dnt = (Sim2Curr-1)->RSI.Down12 * 11 + 0;
  } else
  {
    Upt = (Sim2Curr-1)->RSI.Up12 * 11 + 0;
    Dnt = (Sim2Curr-1)->RSI.Down12 * 11 + LastPrice - NewPrice ;
  }
  Upt = Upt/12;
  Dnt = Dnt/12;
  RSI12 = ( Upt / (Upt+Dnt) )*100;

  if(!strcmp("6",Type))
  {
    return RSI6;
  }
  if(!strcmp("12",Type))
  {
    return RSI12;
  }

  //
  // Should not reach here!
  //
  printf("Error:RSI");
  return 0;
}

float MACD(char* Type)
{
  float   NewPrice;
  float EMA12, EMA26, DIF, MACD9;

  NewPrice = PRICE();
  
  EMA12 = ( LAST_MACD("EMA12",1) * 11 + NewPrice * 2 ) /13;
  EMA26 = ( LAST_MACD("EMA26",1) * 25 + NewPrice * 2 ) /27;
  DIF   = EMA12 - EMA26;
  MACD9 = ( LAST_MACD("MACD9",1) * 8 + DIF * 2 ) /10;

  if(!strcmp("DIF",Type))
  {
    return DIF;
  }

  if(!strcmp("EMA12",Type))
  {
    return EMA12;
  }

  if(!strcmp("EMA26",Type))
  {
    return EMA26;
  }

  if(!strcmp("MACD9",Type))
  {
    return MACD9;
  }

  if(!strcmp("OSC",Type))
  {
    return DIF - MACD9;
  }  

  //
  // Should not reach here!
  //
  printf("Error:MACD");
  return 0;
}

int PriceCross(char* Type, char options) // 0 = up, 1 = down;
{
  float        MA5;
  float        MA10;
  float        MA20;
  float        MA60;  
  float        New5MA;
  float        New10MA;
  float        New20MA;
  float        New60MA;
  float        NewPrice, LastPrice;
  char       Corss5 = 0;
  char       Corss10 = 0;
  char       Corss20 = 0;
  char       Corss60 = 0;  

  NewPrice = PRICE();

  LastPrice = LAST_PRICE("end",1);

  MA5     = LAST_MA("5",1);
  MA10    = LAST_MA("10",1);
  MA20    = LAST_MA("20",1);
  MA60    = LAST_MA("60",1);
  New5MA  = MA("5");
  New10MA = MA("10");
  New20MA = MA("20");
  New60MA = MA("60");

  if (!options)
  {
    if (LastPrice < MA5 && NewPrice >= New5MA)
      Corss5 = 1;
    if (LastPrice < MA10 && NewPrice >= New10MA)
      Corss10 = 1;
    if (LastPrice < MA20 && NewPrice >= New20MA)
      Corss20 = 1;
    if (LastPrice < MA60 && NewPrice >= New60MA)
      Corss60 = 1;      
  } else
  {
    if (LastPrice >= MA5 && NewPrice < New5MA)
      Corss5 = 1;
    if (LastPrice >= MA10 && NewPrice < New10MA)
      Corss10 = 1;
    if (LastPrice >= MA20 && NewPrice < New20MA)
      Corss20 = 1;
    if (LastPrice >= MA60 && NewPrice < New60MA)
      Corss60 = 1;         
  }
  if(Corss5)
    printf("  crossed MA5 LP= %f MA5 = %f NP = %f N5MA = %f  ",LastPrice,MA5,NewPrice,New5MA);
  if(!strcmp(">MA5",Type) || !strcmp("<MA5",Type))
    return Corss5;
  if(!strcmp(">MA10",Type) || !strcmp("<MA10",Type))
    return Corss10;
  if(!strcmp(">MA20",Type) || !strcmp("<MA20",Type))
    return Corss20;
  if(!strcmp(">MA60",Type) || !strcmp("<MA60",Type))
    return Corss60;  

  //
  // Should not reach here!
  //
  printf("Error:MA_CROSS");
  return 0;   
}

int MACross(char* Type)
{
  if(!strcmp("MA5>10",Type))
    if( (Sim2Curr-1)->MA.MA5 < (Sim2Curr-1)->MA.MA10 && (Sim2Curr)->MA.MA5 > (Sim2Curr)->MA.MA10 )
      return 1;
  if(!strcmp("MA5>20",Type))
    if( (Sim2Curr-1)->MA.MA5 < (Sim2Curr-1)->MA.MA20 && (Sim2Curr)->MA.MA5 > (Sim2Curr)->MA.MA20 )
      return 1;
  if(!strcmp("MA5>60",Type))
    if( (Sim2Curr-1)->MA.MA5 < (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA5 > (Sim2Curr)->MA.MA60 )
      return 1;

  if(!strcmp("MA5<10",Type))
    if( (Sim2Curr-1)->MA.MA5 > (Sim2Curr-1)->MA.MA10 && (Sim2Curr)->MA.MA5 < (Sim2Curr)->MA.MA10 )
      return 1;
  if(!strcmp("MA5<20",Type))
    if( (Sim2Curr-1)->MA.MA5 > (Sim2Curr-1)->MA.MA20 && (Sim2Curr)->MA.MA5 < (Sim2Curr)->MA.MA20 )
      return 1;
  if(!strcmp("MA5<60",Type))
    if( (Sim2Curr-1)->MA.MA5 > (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA5 < (Sim2Curr)->MA.MA60 )
      return 1;

  if(!strcmp("MA10>20",Type))
    if( (Sim2Curr-1)->MA.MA10 < (Sim2Curr-1)->MA.MA20 && (Sim2Curr)->MA.MA10 > (Sim2Curr)->MA.MA20 )
      return 1;
  if(!strcmp("MA10>60",Type))
    if( (Sim2Curr-1)->MA.MA10 < (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA10 > (Sim2Curr)->MA.MA60 )
      return 1;

  if(!strcmp("MA10<20",Type))
    if( (Sim2Curr-1)->MA.MA10 > (Sim2Curr-1)->MA.MA20 && (Sim2Curr)->MA.MA10 < (Sim2Curr)->MA.MA20 )
      return 1;
  if(!strcmp("MA10<60",Type))
    if( (Sim2Curr-1)->MA.MA10 > (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA10 < (Sim2Curr)->MA.MA60 )
      return 1;

  if(!strcmp("MA20>60",Type))
    if( (Sim2Curr-1)->MA.MA20 < (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA20 > (Sim2Curr)->MA.MA60 )
      return 1;
  if(!strcmp("MA20<60",Type))
    if( (Sim2Curr-1)->MA.MA20 > (Sim2Curr-1)->MA.MA60 && (Sim2Curr)->MA.MA20 < (Sim2Curr)->MA.MA60 )
      return 1;

  return 0;    
}

int KDCross(char *Type)
{
  float NEW_K, NEW_D;
  float OLD_K, OLD_D;

  NEW_K = KDJ("K");
  NEW_D = KDJ("D");
  OLD_K = LAST_KDJ("K",1);
  OLD_D = LAST_KDJ("D",1);

  if(!strcmp("K>D",Type))
    if (OLD_D > OLD_K && NEW_D < NEW_K)
      return 1;

  if(!strcmp("K<D",Type))
    if (OLD_D < OLD_K && NEW_D > NEW_K)
      return 1;

  return 0;
 
}

int RSICross(char *Type)
{
  float NEW_6, NEW_12;
  float OLD_6, OLD_12;

  NEW_6  = RSI("6");
  NEW_12 = RSI("12");
  OLD_6  = LAST_RSI("6",1);
  OLD_12 = LAST_RSI("12",1);

  if(!strcmp("RSI6>12",Type))
    if (OLD_12 > OLD_6 && NEW_12 < NEW_6)
      return 1;

  if(!strcmp("RSI6<12",Type))
    if (OLD_12 < OLD_6 && NEW_12 > NEW_6)
      return 1;

  return 0;
}

int OSCCross(char *Type)
{
  float NEW_OSC, OLD_OSC;

  NEW_OSC = MACD("OSC");
  OLD_OSC = LAST_MACD("OSC",1);

  if(!strcmp("OSC>0",Type))
    if(OLD_OSC < 0 && NEW_OSC > 0)
      return 1;
  if(!strcmp("OSC<0",Type))    
    if(OLD_OSC > 0 && NEW_OSC < 0)
      return 1;

  return 0; 
}

int IsCross(char * Type)
{
  if(!strcmp(">MA5",Type) || !strcmp(">MA10",Type) || !strcmp(">MA20",Type) || !strcmp(">MA60",Type) )
      return PriceCross(Type, 0);
  else if (!strcmp("<MA5",Type) || !strcmp("<MA10",Type) || !strcmp("<MA20",Type) || !strcmp("<MA60",Type) )
      return PriceCross(Type, 1);

  if(!strcmp("MA5>10",Type)   || !strcmp("MA5>20",Type)   || !strcmp("MA5>60",Type)   || \
     !strcmp("MA5<10",Type)   || !strcmp("MA5<20",Type)   || !strcmp("MA5<60",Type)   || \
     !strcmp("MA10>20",Type)  || !strcmp("MA10>60",Type)  || \
     !strcmp("MA10<20",Type)  || !strcmp("MA10<60",Type)  || \
     !strcmp("MA20>60",Type)  || !strcmp("MA20<60",Type)
  )
    return MACross(Type);

  if(!strcmp("K>D",Type) || !strcmp("K<D",Type) )
    return KDCross(Type);

  if(!strcmp("RSI6>12",Type) || !strcmp("RSI6<12",Type))
    return RSICross(Type);

  if(!strcmp("OSC>0",Type) || !strcmp("OSC<0",Type))
    return OSCCross(Type);

  //
  // Should not reach here!
  //
  printf("Error:IsCross");
  return 0; 
}

int IsKeepOverIn(char * Type, int Days)
{
  float NEW_6, NEW_12, NEW_K,NEW_D;

  if(!strcmp("K,D",Type))
  {
    NEW_K = KDJ("K");
    NEW_D = KDJ("D");

    if (NEW_K > NEW_D)
      if ( NEW_K - NEW_D > Days)
        return 1;
    else if (NEW_K < NEW_D)
      if ( NEW_D - NEW_K > Days)
        return 1;

    return 0;
  }
  if(!strcmp("RSI6,RSI12",Type))
  {
    NEW_6  = RSI("6");
    NEW_12 = RSI("12");

    if (NEW_6 > NEW_12)
      if ( NEW_6 - NEW_12 > Days)
        return 1;
    else if (NEW_6 < NEW_12)
      if ( NEW_12 - NEW_6 > Days)
        return 1;
  }

  //
  // Should not reach here!
  //
  printf("Error:IsDiffOver");
  return 0;   
}

int IsDiffPostiveIn(char* Type, int Days)
{
  int  i,Condition;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Condition = 1;

for ( i = 1; i <= Days; i++)
  if(LAST_DIFF(Type, i) < 0)
    Condition = 0;

  return Condition;
}

int IsDiffNegativeIn(char* Type, int Days)
{
  int  i,Condition;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Condition = 1;

  for ( i = 1; i <= Days; i++)
    if(LAST_DIFF(Type, i) > 0)
      Condition = 0;

  return Condition;
}

int DiffTotal(char* Type, int Days) // Total in x days
{
  int Total, i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Total = 0;

  for ( i = 1; i <= Days; i++)
    Total += LAST_DIFF(Type, i);

  return Total;
  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  //return 0;  
}

int DiffAvg(char* Type, int Days) // Total in x days
{
  int Total, i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Total = 0;

  for ( i = 1; i <= Days; i++)
    Total += LAST_DIFF(Type, i);

  return Total / Days;
}

int CrossedDays(char * Type)
{
  int i,Crossed,Count;
  
  Crossed   = 0;
  Count = 0;

  for(int i = 10; i >= 1; i--)
  {
    MOVE_TO_X_DAYS_AGO(i)
    if(IsCross(Type))
    {
      Crossed = 1;
    }
    if(IsCross(Type))
    {
      Crossed = 0;
      Count = 0;
    }
    MOVE_TO_X_DAYS_FORWORD(i)
    if(Crossed);
      Count++;
  }
  return Count;
}

int MAOrder(char* Type)
{
  int        New5MA;
  int        New10MA;
  int        New20MA;
  int        New60MA;
  int        NewPrice;

  NewPrice = PRICE();

  New5MA   = MA("5");
  New10MA  = MA("10");
  New20MA  = MA("20");
  New60MA  = MA("60");

  if(!strcmp("ascend",Type))
  {
    if(New5MA >= New10MA && New10MA >= New20MA && New20MA >= New60MA)
    {
      return 1;
    } else 
    {
      return 0;
    }
  }
  if(!strcmp("descend",Type) )
  {
    if(New5MA < New10MA && New10MA < New20MA && New20MA < New60MA)
    {
      return 1;
    } else 
    {
      return 0;
    }
  }

  //
  // Should not reach here!
  //
  printf("Error:MA_ORDER");
  return 0;   
}

void AtOpen()
{
/*
  if( IsCross("MA5>20") && KDJ("K") - KDJ("D") > 0 )//&& CrossedDays("MA5>10") == 2 && MAOrder("ascend") )
  {
    Buy("Now",1);
    printf("Buy(C)\n");
  }

 if( IsCross("K>D") && KDJ("K") - KDJ("D") > 5 && MAOrder("ascend") )//&& CrossedDays("MA5>10") == 2 && MAOrder("ascend") )
  {
    Buy("Now",1);
    printf("Buy(C)\n");
  }
  if(IsCross("<MA5"))
  {
    Sell("Now",1);
    printf("Sell(O)\n");
  }   */
}

void AtClose()
{
  MA5Cross20Alg();
}

void TradeRule()
{
  OpenOrClose = 0;  //Price = Start
  AtOpen();
  OpenOrClose = 1;  //Price = End
  AtClose();
}

void AnalysisProfit2 (TRADE_RECORD2 *TradeRecords2)
{
  float  MoneyIn, MoneyOut;
  int    BuyCount, SellCount, Counter;
  int    Shares,SharesRemaining;
  float  TotalIn,TotalOut,LastDayPrice,Price;
  int    SharesInTotal,SharesOutTotal;
  float  TotalRemainValues;
  float  AvgBuyPrice, AvgSellPrice;
  FILE   *fp;
  char   *str;

  str = (char*) malloc(50);
  TotalIn   = 0;
  TotalOut  = 0;
  Counter   = 0;
  BuyCount  = 0;
  SellCount = 0;
  SharesOutTotal = 0;
  SharesInTotal  = 0;
  AvgBuyPrice    = 0;
  AvgSellPrice   = 0;
  
  str = "Result";
  printf(",,,\n",str);

  StrIDAppend(str,gID);

  fp = fopen(str,"w");

  fputs("Start\n",fp);
  WriteNum(gID,str,fp)

  do{
    MoneyIn  = 0;
    MoneyOut = 0;
    Shares   = 0;
    
    Price    = TradeRecords2->Price;
    Shares   = TradeRecords2->ShareTrades;
    if(TradeRecords2->BuyOrSell) // buy
    {
      MoneyIn        +=  Price * Shares;
      SharesInTotal  += Shares;
      AvgBuyPrice    += Price;
      BuyCount  += 1;
      TotalIn  += MoneyIn;        
    } else //sell
    {
      MoneyOut       += Price * Shares;
      SharesOutTotal += Shares;
      AvgSellPrice   += Price;
      SellCount  += 1;
      TotalOut += MoneyOut;        
    }

    //fwrite(&(TradeRecords2->Dates.Years), sizeof(int), 1, fp);
    //fwrite(&(TradeRecords2->Dates.Months), sizeof(int), 1, fp);
    //fwrite(&(TradeRecords2->Dates.Days), sizeof(int), 1, fp);
    WriteDates(TradeRecords2->Dates.Years,TradeRecords2->Dates.Months,TradeRecords2->Dates.Days,str,fp)

    printf("TradeRecords(%d)--%d/%d/%d--\n",Counter+1,TradeRecords2->Dates.Years,TradeRecords2->Dates.Months,TradeRecords2->Dates.Days);
    printf("Action: ");
 
    if(TradeRecords2->BuyOrSell){
      printf("In ,");
      fputs("In\n",fp);
    } else{
      printf("Out ,");
      fputs("Out\n",fp);
    }

    printf("Shares = %d, Price = %.1f, Total Price = %.1f, Shares Remaining = %d \n",Shares,Price,Shares*Price,TradeRecords2->SharesRemaining);
    printf("===============================================\n");

    WriteNum(Shares,str,fp)
    WriteFloatNum(Price,str,fp)
    WriteNum(TradeRecords2->SharesRemaining,str,fp)

    Counter += 1;
    if(TradeRecords2->Next == NULL)
    {
      SharesRemaining = TradeRecords2->SharesRemaining;
      LastDayPrice    = Sim2Curr->End;
    }
    TradeRecords2 = TradeRecords2->Next;

  } while(TradeRecords2 != NULL);

  AvgBuyPrice  = AvgBuyPrice / (float)BuyCount;   
  AvgSellPrice = AvgSellPrice / (float)SellCount;
  TotalRemainValues = (TotalOut - TotalIn) + (LastDayPrice*SharesRemaining);

  fputs("total\n",fp);
  WriteFloatNum(LastDayPrice,str,fp)
  WriteNum(SharesRemaining,str,fp)
  WriteFloatNum(TotalIn,str,fp)
  WriteFloatNum(TotalOut,str,fp)
  WriteNum(BuyCount,str,fp)
  WriteNum(SellCount,str,fp)
  WriteFloatNum(((float)TotalRemainValues / (float) TotalIn )*100,str,fp)
  fputs("end\n",fp);

  printf("Last Day Price = %.1f, Shares Remaining = %d, Remaining Shares Values = %.1f\n",LastDayPrice, SharesRemaining, LastDayPrice*SharesRemaining);
  printf("Input Money= %.1f, Output Money = %.1f, Buy/Sell/Total Count = %d/%d/%d, Average Buy/Sell Price(per shares treade) = %1.f, %1.f\n", TotalIn, TotalOut ,BuyCount,SellCount,Counter,AvgBuyPrice,AvgSellPrice);
  printf("Total Remain (Out - In)+Remaining Shares Values = %.1f\n", TotalRemainValues );
  printf("Returns(Total Remain)/Input = %.1f%%\n", ((float)TotalRemainValues / (float) TotalIn )*100 );

  fclose(fp);
}

void StockSimulator2(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecords2Head)
{
  int         Current;

  gID = InfoBuffer->StockID;

  for(Current = StartDayIndex; Current <= EndDayIndex; Current++)
  {
    Sim2Curr = InfoBuffer + Current -1; /* Move day pointer to next day */
    //printf("Day(%d) %d/%d/%d  \n",Sim2Curr->DayIndex,Sim2Curr->Dates.Years,Sim2Curr->Dates.Months,Sim2Curr->Dates.Days);
    TradeRule();
    //printf("End of Day\n");
  }

  *ReturnRecords2Head = Record2Head;
}

void MA5Cross20Alg()
{
  if(IsCross("MA5>20") && MA("20") > MA("60") )
  {
    Buy("Now",1);
  }

  if(IsCross("MA5<20"))
  {
    Sell("Now",1);
  }  
}