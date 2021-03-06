#ifndef _STOCK_DATA_
#define _STOCK_DATA_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
   float MA240;
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
  int       DayIndex;
  int       BuyOrSell;  /* 1 = buy, 0 = sell */
  float     Price;
  int       ShareTrades;
  int       SharesRemaining;
  DATE      Dates;

  TRADE_RECORD  *Next;
};

extern DAILY_INFO  *InfoBuffer;        /*Global DailyInfo Buffer*/

//
// Emulator
//
extern TRADE_RECORD  *RecordHead;
extern TRADE_RECORD  *RecordCurrent;

void StockSimulator(int len, TRADE_RECORD  **ReturnRecordsHead);
void AnalysisProfit (TRADE_RECORD  *TradeRecords2);
void  ReadLine(FILE *fp,char *str);
//int   FindTotalLen(FILE *fp);

void LoadRule();
void MainProcess();

extern int         *MA_value;
extern int         MA_count;

extern int         *KD_value;
extern int         KD_count;

extern int         *MACD_value;
extern int         MACD_count;

extern int         *RSI_value;
extern int         RSI_count;
extern int         StockId;
extern int         StartDayIndex;
#endif /*_STOCK_DATA_*/