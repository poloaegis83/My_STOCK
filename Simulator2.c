#include "DataDefine.h"

char           OpenOrClose;
DAILY_INFO     *Sim2Curr;
TRADE_RECORD2  *Record2Head = NULL;
TRADE_RECORD2  *Record2Current = NULL;

void BuyOrSell(char options, char *Type, int Shares)
{
  TRADE_RECORD2  *Record;

  Record = (TRADE_RECORD2 *) malloc(sizeof(TRADE_RECORD2));

  if(!strcmp("Now",Type))
  {
    Record->Price = PRICE();
    Record->DayIndex = Sim2Curr->DayIndex;
    Record->Dates    = Sim2Curr->Dates;    
  }
  if(!strcmp("NextBarStart",Type))
  {
    Record->Price = (Sim2Curr+1)->Start;
    Record->DayIndex = (Sim2Curr+1)->DayIndex;
    Record->Dates    = (Sim2Curr+1)->Dates;       
  }
  if(!strcmp("NextBarEnd",Type))
  {
    Record->Price = (Sim2Curr+1)->End;
    Record->DayIndex = (Sim2Curr+1)->DayIndex;
    Record->Dates    = (Sim2Curr+1)->Dates;       
  }
 
  Record->BuyOrSell = options ? 1 : 0;
  Record->SharesRemaining += options ? Shares : -Shares;
  Record->ShareTrades = Shares;
  Record->Next     = NULL;

  if(Record2Head == NULL)
  {
    Record2Head    = Record;
    Record2Current = Record;
    Record->SharesRemaining = options ? Shares : -Shares;
    return;
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
  int        NewPrice;
  int        Highest,Lowest;
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
  Highest = NewPrice;
  Lowest  = NewPrice;

  for(i = 1; i < RSV_n; i++)
  {
    if( (Sim2Curr-i)->End > Highest)
    {
      Highest = (Sim2Curr-i)->End;
    }
    if( (Sim2Curr-i)->End < Lowest )
    {
      Lowest = (Sim2Curr-i)->End;
    }
  }

  RSV = (float)(NewPrice - Lowest) / (float)(Highest - Lowest);

  K   = LAST_KDJ("K") * 3 /2;
  K  += RSV / 3;
  D   = LAST_KDJ("D") * 3 /2;
  D  += K;
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

float LAST_RSI(char* Type, ...)
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

float LAST_MACD(char* Type, ...)
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
  int   NewPrice;
  float EMA12, EMA26, DIF, MACD9;

  NewPrice = PRICE();
  
  EMA12 = ( LAST_MACD("EMA12") * 11 + (float)NewPrice * 2 ) /13;
  EMA26 = ( LAST_MACD("EMA26") * 25 + (float)NewPrice * 2 ) /27;
  DIF   = (EMA12 - EMA26)/100;
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

int LAST_PRICE(char* Type, ...)
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

int PRICE()
{
  if(OpenOrClose)
  {
    return Sim2Curr->End;
  } else
  {
    return Sim2Curr->Start;
  }
}

int DIFFERENCE(char* Type, ...) // days average
{
  if(!strcmp("leader",Type))
  {
    return (Sim2Curr-1)->LeaderDiff;
  }
  if(!strcmp("foreign",Type))
  {
    return (Sim2Curr-1)->ForeignInvestorsDiff;
  }
  if(!strcmp("investment",Type))
  {
    return (Sim2Curr-1)->InvestmentTrustDiff;
  }
  if(!strcmp("dealers",Type))
  {
    return (Sim2Curr-1)->DealersDiff;
  }

  //
  // Should not reach here!
  //
  printf("Error:DIFFERENCE");
  return 0;  
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
  int        New5MA;
  int        New10MA;
  int        New20MA;
  int        New60MA;
  int        NewPrice;

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

  if(!strcmp("bull",Type))
  {
    if(New5MA >= New10MA && New10MA >= New20MA)
    {
      return 1;
    } else 
    {
      return 0;
    }
  }
  if(!strcmp("bear",Type) )
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
  if( CrossUpMA("5") && MA_ORDER("bull") && KDCrossUp() )
  {
    Buy("Now",1000);
  }
}

void AtClose()
{
  if( CrossDownMA("5") && MA_ORDER("bear") && KDCrossDown() )
  {
    Sell("Now",1000);
  }
}

void TradeRule()
{
  OpenOrClose = 0;  //Price = Start
  AtOpen();
  OpenOrClose = 1;  //Price = End
  AtClose();
}

void AnalysisProfit2 (TRADE_RECORD2  *TradeRecords2)
{
  int  MoneyIn, MoneyOut;
  int  BuyCount, SellCount;
  int  Price,Shares,Counter;
  int  TotalIn,TotalOut,SharesInTotal,SharesOutTotal;
  float  AvgBuyPrice, AvgSellPrice;

  TotalIn   = 0;
  TotalOut  = 0;
  Counter   = 0;
  BuyCount  = 0;
  SellCount = 0;
  SharesOutTotal = 0;
  SharesInTotal  = 0;

  do{
    MoneyIn  = 0;
    MoneyOut = 0;
    Shares   = 0;
    
    Price    = TradeRecords2->Price;
    Shares   = TradeRecords2->ShareTrades;
    if(TradeRecords2->BuyOrSell) //sell
    {
      MoneyOut       += Price * Shares;
      SharesOutTotal += Shares;
      AvgSellPrice   += Price;
      SellCount  += 1;      
    } else // buy
    {
      MoneyIn        +=  Price * Shares;
      SharesInTotal  += Shares;
      AvgBuyPrice    += Price;
      BuyCount  += 1;
    }
    TotalIn  += MoneyIn;
    TotalOut += MoneyOut;
    AvgBuyPrice  = AvgBuyPrice  / (float)BuyCount;
    AvgSellPrice = AvgSellPrice / (float)SellCount;

    printf("TradeRecords(%d)--%d/%d/%d--\n",Counter+1,TradeRecords2->Dates.Years,TradeRecords2->Dates.Months,TradeRecords2->Dates.Days);
    printf("Action: ");

    if(TradeRecords2->BuyOrSell)
      printf("Out ,");
    else
      printf("In ,");

    printf("Shares = %d, Price = %d, Total Price = %d \n",Shares,Price,Shares*Price);
    printf("===============================================\n");

    Counter += 1;	 
    TradeRecords2 = TradeRecords2->Next;
  } while(TradeRecords2 != NULL);

  printf("\nTotal Earned = %d, Buy/Sell/Total Count = %d/%d/%d, Average Buy/Sell Price = %1.f/%1.f", TotalOut - TotalIn,BuyCount,SellCount,Counter,AvgBuyPrice,AvgSellPrice);
}

void StockSimulator2(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecordsHead)
{
  int         Current;

  for(Current = StartDayIndex; Current <= EndDayIndex; Current++)
  {
    Sim2Curr = InfoBuffer + Current -1; /* Move day pointer to next day */
    TradeRule();
  }
}

