#include "DataDefine.h"
#define CROSS_OVER(a,b)  MoveTheDay(-1)&&(a < b)&&MoveTheDay(1)&&(a > b)
#define CROSS_UNDER(a,b) MoveTheDay(-1)&&(a > b)&&MoveTheDay(1)&&(a < b)

DAILY_INFO *Sim3Curr;

int MoveTheDay(int Days)
{
  if(Days == 1 || Days == -1)
  {
    Sim3Curr += Days;
    return 1;
  }
  return 0;
}

float C(int Len)
{
  return (Sim3Curr-Len)->End;
}

float O(int Len)
{
  return (Sim3Curr-Len)->Start;
}

float H(int Len)
{
  return (Sim3Curr-Len)->High;
}

float L(int Len)
{
  return (Sim3Curr-Len)->Low;
}

float HEIGEST(char *Type, int Len)
{
  int i;
  float Value,Highest;

  if(!strcmp("C",Type))
    Highest = (Sim3Curr)->End;
  if(!strcmp("O",Type))
    Highest = (Sim3Curr)->Start;
  if(!strcmp("H",Type))
    Highest = (Sim3Curr)->High;
  if(!strcmp("L",Type))
    Highest = (Sim3Curr)->Low;            
  
  for(int i = 1; i < Len; i++)
  {
    if(!strcmp("C",Type))
      Value = (Sim3Curr)->End;
    if(!strcmp("O",Type))
      Value = (Sim3Curr)->Start;
    if(!strcmp("H",Type))
      Value = (Sim3Curr)->High;
    if(!strcmp("L",Type))
      Value = (Sim3Curr)->Low;
    if(Value > Highest)
      Highest = Value;
  }

  return Highest;
}

float LOWEST(char *Type, int Len)
{
  int i;
  float Value,Lowest;

  if(!strcmp("C",Type))
    Lowest = (Sim3Curr)->End;
  if(!strcmp("O",Type))
    Lowest = (Sim3Curr)->Start;
  if(!strcmp("H",Type))
    Lowest = (Sim3Curr)->High;
  if(!strcmp("L",Type))
    Lowest = (Sim3Curr)->Low;            
  
  for(int i = 1; i < Len; i++)
  {
    if(!strcmp("C",Type))
      Value = (Sim3Curr)->End;
    if(!strcmp("O",Type))
      Value = (Sim3Curr)->Start;
    if(!strcmp("H",Type))
      Value = (Sim3Curr)->High;
    if(!strcmp("L",Type))
      Value = (Sim3Curr)->Low;
    if(Value < Lowest)
      Lowest = Value;
  }

  return Lowest;
}

float AVERAGE(char *Type, int Len)
{
  int i;
  float Total;

  Total = 0;

  for(int i = 0; i < Len; i++)
  {
    if(!strcmp("C",Type))
      Total += (Sim3Curr+Len)->End;
    if(!strcmp("O",Type))
      Total += (Sim3Curr+Len)->Start;
    if(!strcmp("H",Type))
      Total += (Sim3Curr+Len)->High;
    if(!strcmp("L",Type))
      Total += (Sim3Curr+Len)->Low;
  }

  return Total/(float)Len;
}

float KD(char *Type,int Len)
{
  int i;
  float  Today,Price,Highest,Lowest,LastRSV;
  float  NewK,OldK,NewD,OldD;

  if(!strcmp("C",Type))
    Today = (Sim3Curr+Len)->End;
  if(!strcmp("O",Type))
    Today = (Sim3Curr+Len)->Start;
  
  Highest = Today;
  Lowest  = Today;

  for(i = 1; i < Len; i++)
  {
    if(!strcmp("C",Type))
    {
      Price = (Sim3Curr-i)->End;
      if( (Sim3Curr-i)->End > Highest)
        Highest = Price;
      if( (Sim3Curr-i)->End < Lowest)
        Lowest = Price;
    }
    if(!strcmp("O",Type))
    {
      Price = (Sim3Curr-i)->Start;
      if( (Sim3Curr-i)->Start > Highest)
        Highest = Price;
      if( (Sim3Curr-i)->Start < Lowest)
        Lowest = Price;
    }
  }

  LastRSV = Today-Lowest / Highest-Lowest;
}

float CalOldKD(int Index, int Len)
{
  
}

float RSI(char *Type,int Len)
{

}

float CalOldRSI(int Index, int Len)
{
  
}

float MACD(char *Type,int Len)
{

}

float CalOldMACD(int Index, int Len)
{
  
}

void TradeRule()
{

}

void StockSimulator2(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecords2Head)
{
  int         Current;

  for(Current = StartDayIndex; Current <= EndDayIndex; Current++)
  {
    Sim3Curr = InfoBuffer + Current -1; /* Move day pointer to next day */
    printf("Day(%d) %d/%d/%d  \n",Sim3Curr->DayIndex,Sim3Curr->Dates.Years,Sim3Curr->Dates.Months,Sim3Curr->Dates.Days);
    TradeRule();
    printf("End of Day\n");
  }

  *ReturnRecords2Head = Record2Head;
}