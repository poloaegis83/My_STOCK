#ifndef _STOCK_DATA_
#define _STOCK_DATA_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MA5_OVER_MA10 100.1
#define STOP_LOSS_LIMIT 0.02
#define MA_MAX          60     /*For MA calculate*/
#define FIRST_DAILY_DATA_INDEX 62 /*MA_MAX + 2*/
#define BUFF_SIZE        300000
#define DEBUG(Expression)        \
          if(DebugFlag){         \
		  	printf(Expression);      \
		  }                          \

typedef struct _DailyInfo DAILY_INFO;
typedef struct _Date DATE;
typedef struct _MA_Value   MA_VALUE;
typedef struct _MACD_Value MACD_VALUE;
typedef struct _KDJ_Value  KDJ_VALUE;
typedef struct _RSI_Value  RSI_VALUE;
typedef struct _Trade_Record  TRADE_RECORD;
typedef struct _Trade_Record2  TRADE_RECORD2;

struct _Date {
  int Years;
  int Months;
  int Days;
};

struct _MA_Value {
   float MA5;
   float MA10;
   float MA20;
   float MA60;
   float MA120;
};

struct _MACD_Value {
   float EMA12;
   float EMA26;
   float DIF;
   float MACD9;
   float OSC;
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

   float Start;
   float End;
   float High;
   float Low;

   int LeaderDiff;
   int ForeignInvestorsDiff;
   int InvestmentTrustDiff;  
   int DealersDiff;

   MA_VALUE    MA;
   MACD_VALUE  MACD;
   RSI_VALUE   RSI;
   KDJ_VALUE   KDJ;

   int     DayIndex;
   DATE    Dates;
};

struct _Trade_Record {
  int       BuyDayIndex;
  int       SellDayIndex;

  float       BuyPrice;
  float       SellPrice;

  DATE          BuyDates;
  DATE          SellDates;

  TRADE_RECORD  *Next;
};

struct _Trade_Record2 {
  int       DayIndex;
  int       BuyOrSell;  /* 1 = buy, 0 = sell */
  float     Price;
  int       ShareTrades;
  int       SharesRemaining;
  DATE      Dates;

  TRADE_RECORD2  *Next;
};

extern DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/
extern char DebugFlag;

//
// Emulator 1
//
extern int         ChipAnalysisFlag; /*Featrue*/
extern float       MA5_Over_MA10;

void StockSimulator1 (int StartDayIndex, int EndDayIndex, TRADE_RECORD **ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords);

//
// Emulator 2
//
extern char   OpenOrClose;
extern TRADE_RECORD2  *Record2Head;
extern TRADE_RECORD2  *Record2Current;

void StockSimulator2(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecordsHead);
void AnalysisProfit2 (TRADE_RECORD2  *TradeRecords2);

#endif /*_STOCK_DATA_*/