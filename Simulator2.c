#include "DataDefine.h"

char           OpenOrClose;
DAILY_INFO     *Sim2Curr;
TRADE_RECORD2  *Record2Head = NULL;
TRADE_RECORD2  *Record2Current = NULL;

float PRICE()
{
  if(OpenOrClose)
  {
    return Sim2Curr->End;
    printf("PRICE() = %.1f", Sim2Curr->End);
  } else
  {
    return Sim2Curr->Start;
    printf("PRICE() = %.1f", Sim2Curr->End);
  }
}

float LAST_PRICE(char* Type)
{
  if(!strcmp("high",Type))
  {
    return (Sim2Curr-1)->High;
  }
  if(!strcmp("low",Type))
  {
    return (Sim2Curr-1)->Low;
  }
  if(!strcmp("start",Type))
  {
    return (Sim2Curr-1)->Start;
  }
  if(!strcmp("end",Type))
  {
    return (Sim2Curr-1)->End;
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
      Record->Price = Price;
      Record->Price = DayIndex;
      Record->Dates = Dates; 

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
  }

  Record = (TRADE_RECORD2 *) malloc(sizeof(TRADE_RECORD2));

  Record->Price = Price;
  Record->Price = DayIndex;
  Record->Dates = Dates; 

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

float LAST_KDJ(char* Type)
{
  if(!strcmp("K",Type))
  {
    return (Sim2Curr-1)->KDJ.K;
  }
  if(!strcmp("D",Type))
  {
    return (Sim2Curr-1)->KDJ.D;
  }
  if(!strcmp("J",Type))
  {
    return (Sim2Curr-1)->KDJ.J;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_KDJ");
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
    if( (Sim2Curr-i)->High > Highest)
    {
      Highest = (Sim2Curr-i)->High;
    }
    if( (Sim2Curr-i)->Low < Lowest )
    {
      Lowest = (Sim2Curr-i)->Low;
    }
  }
 
  RSV = (NewPrice - Lowest) / (Highest - Lowest) * 100;
  //printf("LAST_KDJ(K) = %f, LAST_KDJ(D) = %f, RSV = %f ,after  Highest = %f ,Lowest = %f \n",LAST_KDJ("K"),LAST_KDJ("D"),RSV,Highest,Lowest);
  K   = LAST_KDJ("K") * 2 / 3;
  K  += RSV / 3;
  D   = LAST_KDJ("D") * 2 / 3;
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

float LAST_RSI(char* Type)
{
  if(!strcmp("6",Type))
  {
    return (Sim2Curr-1)->RSI.RSI_6;
  }
  if(!strcmp("12",Type))
  {
    return (Sim2Curr-1)->RSI.RSI_12;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_RSI");
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
  LastPrice = LAST_PRICE("end");

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

float LAST_MACD(char* Type)
{

  if(!strcmp("DIF",Type))
  {
    return (Sim2Curr-1)->MACD.DIF;
  }

  if(!strcmp("EMA12",Type))
  {
    return (Sim2Curr-1)->MACD.EMA12;
  }

  if(!strcmp("EMA26",Type))
  {
    return (Sim2Curr-1)->MACD.EMA26;
  }

  if(!strcmp("MACD9",Type))
  {
    return (Sim2Curr-1)->MACD.MACD9;
  }

  if(!strcmp("OSC",Type))
  {
    return (Sim2Curr-1)->MACD.OSC;
  }  

  //
  // Should not reach here!
  //
  printf("Error:LAST_MACD");
  return 0;
}

float MACD(char* Type)
{
  float   NewPrice;
  float EMA12, EMA26, DIF, MACD9;

  NewPrice = PRICE();
  
  EMA12 = ( LAST_MACD("EMA12") * 11 + NewPrice * 2 ) /13;
  EMA26 = ( LAST_MACD("EMA26") * 25 + NewPrice * 2 ) /27;
  DIF   = EMA12 - EMA26;
  MACD9 = ( LAST_MACD("MACD9") * 8 + DIF * 2 ) /10;

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

int DIFF_AT(char* Type, int Days)
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

int DIFF_TOTAL_IN(char* Type, int Days) // Total in x days
{
  int Total, i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Total = 0;

  if (!strcmp("leader",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("leader", i);
    }    
  }
  if(!strcmp("foreign",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("foreign", i);
    }    
  }
  if(!strcmp("investment",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("investment", i);
    }    
  }
  if(!strcmp("dealers",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("dealers", i);
    }    
  }
  return Total;
  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  //return 0;  
}

int DIFF_AVG_IN(char* Type, int Days) // Total in x days
{
  int Total, i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Total = 0;

  if (!strcmp("leader",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("leader", i);
    }    
  }
  if(!strcmp("foreign",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("foreign", i);
    }    
  }
  if(!strcmp("investment",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("investment", i);
    }    
  }
  if(!strcmp("dealers",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      Total += DIFF_AT("dealers", i);
    }    
  }
  return Total / Days;
  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  //return 0;  
}

int IS_DIFF_POSTIVE_IN(char* Type, int Days) // days average
{
  char Condition;
  int  i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Condition = 1;

  if (!strcmp("leader",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("leader", i) < 0)
      {
        Condition = 0;
      }
    }    
  }
  if(!strcmp("foreign",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("foreign", i) < 0)
      {
        Condition = 0;
      }
    }     
  }
  if(!strcmp("investment",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("investment", i) < 0)
      {
        Condition = 0;
      }
    }    
  }
  if(!strcmp("dealers",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("dealers", i) < 0)
      {
        Condition = 0;
      }
    }     
  }
  
  return Condition;

  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  //return 0;  
}

int IS_DIFF_NEGATIVE_IN(char* Type, int Days) // days average
{
  char Condition;
  int  i;

  if (Days <= 0)
  {
    printf("Error:DIFFERENCE Days Should > 0\n");
    return 0;
  }

  Condition = 1;

  if (!strcmp("leader",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("leader", i) > 0)
      {
        Condition = 0;
      }
    }    
  }
  if(!strcmp("foreign",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("foreign", i) > 0)
      {
        Condition = 0;
      }
    }     
  }
  if(!strcmp("investment",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("investment", i) > 0)
      {
        Condition = 0;
      }
    }    
  }
  if(!strcmp("dealers",Type))
  {
    for ( i = 1; i <= Days; i++)
    {
      if(DIFF_AT("dealers", i) > 0)
      {
        Condition = 0;
      }
    }     
  }

  return Condition;

  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  //return 0;  
}

float LAST_MA(char* Type)
{
  if(!strcmp("5",Type))
  {
    return (Sim2Curr-1)->MA.MA5;
  }
  if(!strcmp("10",Type))
  {
    return (Sim2Curr-1)->MA.MA10;
  }
  if(!strcmp("20",Type))
  {
    return (Sim2Curr-1)->MA.MA20;
  }
  if(!strcmp("60",Type))
  {
    return (Sim2Curr-1)->MA.MA60;
  }
  if(!strcmp("120",Type))
  {
    return (Sim2Curr-1)->MA.MA120;
  }

  //
  // Should not reach here!
  //
  printf("Error:LAST_MA");
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

int LAST_MA_ORDER(char* Type)
{
  if(!strcmp("bull",Type))
  {
    if((Sim2Curr-1)->MA.MA5 >= (Sim2Curr-1)->MA.MA10 && (Sim2Curr-1)->MA.MA10 >= (Sim2Curr-1)->MA.MA20)
    {
      return 1;
    } else 
    {
      return 0;
    }
  }
  if(!strcmp("bear",Type))
  {
    if((Sim2Curr-1)->MA.MA5 < (Sim2Curr-1)->MA.MA10 && (Sim2Curr-1)->MA.MA10 < (Sim2Curr-1)->MA.MA20)
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
  printf("Error:LAST_MA_ORDER");
  return 0;   
}

int MA_ORDER(char* Type)
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
 
  //New60MA  = (Sim2Curr-1)->MA.MA5 * 60;
  //New60MA -= (Sim2Curr-60)->End;
  //New60MA  = (New60MA + NewPrice)/60

  if(!strcmp("ascend",Type))
  {
    if(New5MA >= New10MA && New10MA >= New20MA)
    {
      return 1;
    } else 
    {
      return 0;
    }
  }
  if(!strcmp("descend",Type) )
  {
    if(New5MA < New10MA && New10MA < New20MA)
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

int IsCrossMA(char* Type, char options) // 0 = up, 1 = down;
{

  int        MA5;
  int        MA10;
  int        MA20;
  int        MA60;  
  int        New5MA;
  int        New10MA;
  int        New20MA;
  int        New60MA;
  int        NewPrice, LastPrice;
  char       Corss5 = 0;
  char       Corss10 = 0;
  char       Corss20 = 0;

  NewPrice = PRICE();

  LastPrice = LAST_PRICE("end");

  MA5     = LAST_MA("5");
  MA10    = LAST_MA("10");
  MA20    = LAST_MA("20");
  New5MA  = MA("5");
  New10MA = MA("10");
  New20MA = MA("20");

  if (options)
  {
    if (LastPrice < MA5 && NewPrice >= New5MA)
      Corss5 = 1;
    if (LastPrice < MA10 && NewPrice >= New10MA)
      Corss10 = 1;
    if (LastPrice < MA20 && NewPrice >= New20MA)
      Corss20 = 1;
  } else
  {
    if (LastPrice >= MA5 && NewPrice < New5MA)
      Corss5 = 1;
    if (LastPrice >= MA10 && NewPrice < New10MA)
      Corss10 = 1;
    if (LastPrice >= MA20 && NewPrice < New20MA)
      Corss20 = 1;
  }

  if(!strcmp("5",Type))
    return Corss5;
  if(!strcmp("10",Type))
    return Corss10;
  if(!strcmp("20",Type))
    return Corss20;

  //
  // Should not reach here!
  //
  printf("Error:MA_CROSS");
  return 0;   
}

int CrossUpMA (char* Type)
{
  return IsCrossMA(Type, 0);
}

int CrossDownMA (char* Type)
{
  return IsCrossMA(Type, 1);
}

int KDCrossUp()
{
  float NEW_K, NEW_D;
  float OLD_K, OLD_D;

  NEW_K = KDJ("K");
  NEW_D = KDJ("D");
  OLD_K = LAST_KDJ("K");
  OLD_D = LAST_KDJ("D");

  //printf("\nKDCrossUpCheck(C), NEW K = %1.f, NEW D = %1.f, OLD K = %1.f, OLD D = %1.f\n",NEW_K,NEW_D,OLD_K,OLD_D);

  if (OLD_D > OLD_K && NEW_D < NEW_K)
  {
    return 1;
  } else
  {
    return 0;
  }

  //
  // Should not reach here!
  //
  printf("Error:KDCrossUp");
  return 0;    
}

int KDCrossDown()
{
  float NEW_K, NEW_D;
  float OLD_K, OLD_D;

  NEW_K = KDJ("K");
  NEW_D = KDJ("D");
  OLD_K = LAST_KDJ("K");
  OLD_D = LAST_KDJ("D");

  if (OLD_K > OLD_D && NEW_K < NEW_D)
  {
    return 1;
  } else
  {
    return 0;
  }

  //
  // Should not reach here!
  //
  printf("Error:KDCrossDown");
  return 0;   
}

int KDOverRange(float range)
{
  float NEW_K, NEW_D;

  NEW_K = KDJ("K");
  NEW_D = KDJ("D");

  if (NEW_K > NEW_D)
  {
    if ( NEW_K - NEW_D > range)
      return 1;
  } else if (NEW_K < NEW_D)
  {
    if ( NEW_D - NEW_K > range)
      return 1;
  }

  return 0;
}

int RSICrossUp()
{
  float NEW_6, NEW_12;
  float OLD_6, OLD_12;

  NEW_6  = RSI("6");
  NEW_12 = RSI("12");
  OLD_6  = LAST_RSI("6");
  OLD_12 = LAST_RSI("12");

  if (OLD_6 < OLD_12 && NEW_6 > NEW_12)
  {
    return 1;
  } else
  {
    return 0;
  }

  //
  // Should not reach here!
  //
  printf("Error:RSICrossUp");
  return 0;    
}

int RSICrossDown()
{
  float NEW_6, NEW_12;
  float OLD_6, OLD_12;

  NEW_6  = RSI("6");
  NEW_12 = RSI("12");
  OLD_6  = LAST_RSI("6");
  OLD_12 = LAST_RSI("12");

  if (OLD_6 > OLD_12 && NEW_6 < NEW_12)
  {
    return 1;
  } else
  {
    return 0;
  }

  //
  // Should not reach here!
  //
  printf("Error:RSICrossDown");
  return 0;    
}

int OSCCrossUp ()
{
  float NEW_OSC, OLD_OSC;

  NEW_OSC = MACD("OSC");
  OLD_OSC = LAST_MACD("OSC");

  if(OLD_OSC < 0 && NEW_OSC > 0)
  {
    return 1;
  } else 
  {
    return 0;
  }
  //
  // Should not reach here!
  //
  printf("Error:OSCCrossUp");
  return 0; 
}

int OSCCrossDown ()
{

  float NEW_OSC, OLD_OSC;

  NEW_OSC = MACD("OSC");
  OLD_OSC = LAST_MACD("OSC");

  if(OLD_OSC > 0 && NEW_OSC < 0)
  {
    return 1;
  } else 
  {
    return 0;
  }
  //
  // Should not reach here!
  //
  printf("Error:OSCCrossDown");
  return 0; 
}

void AtOpen()
{
  //if( CrossDownMA("5") && MA_ORDER("descend") )
  if ( KDCrossDown() )
  {
    Sell("Now",1);
    printf("Sell(O)\n");
  }
}

void AtClose()
{
  //if( CrossUpMA("5") && MA_ORDER("ascend") && KDCrossUp())
  if ( KDCrossUp() )// && KDOverRange(5))
  {
    Buy("Now",1);
    printf("Buy(C)\n");
  }  
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

  TotalIn   = 0;
  TotalOut  = 0;
  Counter   = 0;
  BuyCount  = 0;
  SellCount = 0;
  SharesOutTotal = 0;
  SharesInTotal  = 0;
  AvgBuyPrice    = 0;
  AvgSellPrice   = 0;

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
      AvgBuyPrice  = AvgBuyPrice  / (float)BuyCount;   
      TotalIn  += MoneyIn;        
    } else //sell
    {
      MoneyOut       += Price * Shares;
      SharesOutTotal += Shares;
      AvgSellPrice   += Price;
      SellCount  += 1;
      AvgSellPrice = AvgSellPrice / (float)SellCount;
      TotalOut += MoneyOut;        
    }

    printf("TradeRecords(%d)--%d/%d/%d--\n",Counter+1,TradeRecords2->Dates.Years,TradeRecords2->Dates.Months,TradeRecords2->Dates.Days);
    printf("Action: ");

    if(TradeRecords2->BuyOrSell)
      printf("In ,");
    else
      printf("Out ,");

    printf("Shares = %d, Price = %.1f, Total Price = %.1f, Shares Remaining = %d \n",Shares,Price,Shares*Price,TradeRecords2->SharesRemaining);
    printf("===============================================\n");

    Counter += 1;
    if(TradeRecords2->Next == NULL)
    {
      SharesRemaining = TradeRecords2->SharesRemaining;
      LastDayPrice    = Sim2Curr->End;
    }
    TradeRecords2 = TradeRecords2->Next;

  } while(TradeRecords2 != NULL);

  TotalRemainValues = (TotalOut - TotalIn) + (LastDayPrice*SharesRemaining);

  printf("Last Day Price = %.1f, Shares Remaining = %d, Remaining Shares Values = %.1f\n",LastDayPrice, SharesRemaining, LastDayPrice*SharesRemaining);
  printf("Input Money= %.1f, Output Money = %.1f, Buy/Sell/Total Count = %d/%d/%d, Average Buy/Sell Price(per shares treade) = %1.f, %1.f\n", TotalIn, TotalOut ,BuyCount,SellCount,Counter,AvgBuyPrice,AvgSellPrice);
  printf("Total Remain (Out - In)+Remaining Shares Values = %.1f\n", TotalRemainValues );
  printf("Returns(Total Remain)/Input = %.1f%%\n", ((float)TotalRemainValues / (float) TotalIn )*100 );
}

void StockSimulator2(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecords2Head)
{
  int         Current;

  for(Current = StartDayIndex; Current <= EndDayIndex; Current++)
  {
    Sim2Curr = InfoBuffer + Current -1; /* Move day pointer to next day */
    printf("Day(%d) %d/%d/%d  \n",Sim2Curr->DayIndex,Sim2Curr->Dates.Years,Sim2Curr->Dates.Months,Sim2Curr->Dates.Days);
    TradeRule();
    printf("End of Day\n");
  }

  *ReturnRecords2Head = Record2Head;
}

