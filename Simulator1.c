#include "DataDefine.h"

void FindBuyPoint (int StartDayIndex, int EndDayIndex, int *BuyDayIndex ,int *BuyPrice);
void FindSellPoint (int BuyDayIndex, int EndDayIndex, int BuyPrice, int *SellDayIndex, int *SellPrice);

void FindBuyPoint(int StartDayIndex, int EndDayIndex, int *BuyDayIndex ,int *BuyPrice)
{
  //
  // Find buying point
  //
  int        CurrentIndex;
  DAILY_INFO *Daily;     
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  int        NewPriceS;
  int        NewPriceE;  
  int        New5MA;
  int        New10MA;
  int        New20MA;  
  char       MA_checkS = 0;
  char       MA_checkE = 0;  
  char       LD_check  = 0;
  char       RSI_check = 0;
  char       KD_check  = 0;
  float      Percent;

  Daily = InfoBuffer + StartDayIndex-1; /* +22 */ 

  for (CurrentIndex = StartDayIndex; CurrentIndex <= EndDayIndex; CurrentIndex++)
  {
    LastDay =  Daily-1;
    Last2Day = LastDay-1;
    printf("SearchBuy-%d/%d/%d LastDay(%d),LD2(%d),CurrInd = %d,%d,%d\n",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,LastDay->DayIndex,Last2Day->DayIndex, LastDay->Start, Last2Day->Start, CurrentIndex);

    //
    // Check MA Start
    //
    NewPriceS = Daily->Start;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceS)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceS)/10;
	  New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceS)/20;
	  //printf("Price(S) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceS,New5MA,New10MA,New20MA);
    if(NewPriceS > New5MA && New5MA >= New10MA && New10MA >= New20MA)
    {
	  Percent = ((float)New5MA/(float)New10MA)*100;
      if (Percent >= MA5_OVER_MA10) //MA5 should over MA10 101%
      {
        MA_checkS = 1;
      }
    }

    //
    // Check MA End
    //
	  NewPriceE = Daily->End;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceE)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceE)/10;
	  New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceE)/20;
  	//printf("Price(E) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceE,New5MA,New10MA,New20MA);	
    if(NewPriceE > New5MA && New5MA >= New10MA && New10MA >= New20MA)
    {
	  Percent = ((float)New5MA/(float)New10MA)*100;	
      if( Percent >= MA5_OVER_MA10) //MA5 should over MA10 101%
      {
        MA_checkE = 1;
      }
    }


    //
    // Check LeaderDifference, should be a postive number 2 days.
    //
	if(ChipAnalysisFlag)
  	{
      if(LastDay->LeaderDiff >0 && Last2Day->LeaderDiff >0)
      {
       LD_check = 1;
      }
	} else
      {
	  LD_check = 1;
	  }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
	  if(MA_checkS && LD_check)
    {
      *BuyPrice    = NewPriceS;
      *BuyDayIndex = CurrentIndex;
	    //printf("V(S)\n");
	    return;	
    }
	  if (MA_checkE && LD_check) 
    {
      *BuyPrice    = NewPriceE;
      *BuyDayIndex = CurrentIndex;
    	//printf("V(E)\n");
	    return;
	  }
      
    Daily += 1;
  }

  //
  // No buy point
  //
  *BuyDayIndex = 0;
  *BuyPrice    = 0;

  DEBUG("FindBuyPoint Not Find\n"); 
  DEBUG("FindBuyPoint End\n");  
}

void FindSellPoint(int BuyDayIndex, int EndDayIndex, int BuyPrice, int *SellDayIndex, int *SellPrice)
{
  //
  // Find Selling point
  //
  int        CurrentIndex;
  DAILY_INFO *Daily;
  DAILY_INFO *LastDay;
  DAILY_INFO *Last2Day;
  int        NewPriceS;
  int        NewPriceE; 
  char       ConditionCheckS = 0;
  char       ConditionCheckE = 0;  
  char       RSI_check      = 0;
  char       KD_check       = 0;
  int        New5MA;
  int        New10MA;
  int        New20MA;  
  float      Percent;
  
  DEBUG("FindSellPoint Start\n"); 

  Daily = InfoBuffer + BuyDayIndex-1;

  for (CurrentIndex = BuyDayIndex; CurrentIndex <= EndDayIndex; CurrentIndex++)
  {
    LastDay   = Daily-1;
    Last2Day  = LastDay-1;

    printf("SearchSell-%d/%d/%d ,LastDay(%d),LD2(%d),CurrentIndex = %d,%d,%d\n",Daily->Dates.Years,Daily->Dates.Months,Daily->Dates.Days,LastDay->DayIndex,Last2Day->DayIndex, LastDay->Start, Last2Day->Start,CurrentIndex);

    NewPriceS = Daily->Start;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceS)/5;
	  New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceS)/10;
  	New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceS)/20;
  	//printf("Price(S) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceS,New5MA,New10MA,New20MA);
    //
    // Check MA Start
    //
    // 3 Cases of LeaderDifference
    //
    if(ChipAnalysisFlag)
    {
      if (LastDay->LeaderDiff > 0 && Last2Day->LeaderDiff > 0) 
        {
          if(NewPriceS < New10MA) 
	      {
            ConditionCheckS = 1;
          }
	    }
      if (LastDay->LeaderDiff > 0) 
        {
          if(NewPriceS < New10MA) 
	      {
            ConditionCheckS = 1;	
          }
        } 
      if(LastDay->LeaderDiff <= 0 && ChipAnalysisFlag) 
	    {
          if(NewPriceS < New5MA) 
	      {
            ConditionCheckS = 1;
          }
        }
    }else
    {
      if(NewPriceS < New5MA) 
	    {
          ConditionCheckS = 1;
        }
    }

	/*
    if(NewPriceS < New5MA) {
      ConditionCheckS = 1;
    }*/
    //
    // Check for stop loss order
    //
	//print("stoploss = %f \n",(((float)BuyPrice - (float)NewPriceS) /(float)BuyPrice) );
	
	  Percent = (float)(BuyPrice - NewPriceS) /(float)BuyPrice;
    if ( (NewPriceS < BuyPrice) && (Percent > STOP_LOSS_LIMIT) )// if loss more than 15% 
    {
	    //printf("  Percent = %.2f\n",Percent);
      ConditionCheckS = 1;
    }

    NewPriceE = Daily->End;
	  New5MA = ((LastDay->MA.MA5)*5 - (Daily-5)->End + NewPriceE)/5;
  	New10MA = ((LastDay->MA.MA10)*10 - (Daily-10)->End + NewPriceE)/10;
  	New20MA = ((LastDay->MA.MA20)*20 - (Daily-20)->End + NewPriceE)/20;
  	//printf("Price(E) = %d, New5MA = %d, New10MA = %d, New20MA = %d\n",NewPriceE,New5MA,New10MA,New20MA);
	
    //
    // Check MA End
    //
    // 3 Cases of LeaderDifference
    //

    if(ChipAnalysisFlag)
    {
      if (LastDay->LeaderDiff > 0 && Last2Day->LeaderDiff > 0) 
        {
          if(NewPriceE < New20MA) 
	      {
            ConditionCheckE = 1;
          }
	    }
      if (LastDay->LeaderDiff > 0) 
        {
          if(NewPriceE < New10MA) 
	      {
            ConditionCheckE = 1;	
          }
        } 
      if(LastDay->LeaderDiff <= 0 && ChipAnalysisFlag) 
	    {
          if(NewPriceE < New5MA) 
	      {
            ConditionCheckE = 1;
          }
        }
    }else
    {
      if(NewPriceE < New5MA) 
	    {
          ConditionCheckE = 1;
        }
    }
	/*
    if(NewPriceE < New5MA) {
      ConditionCheckE = 1;
    }*/
    //
    // Check for stop loss order
    //
    Percent = (float)(BuyPrice - NewPriceE) /(float)BuyPrice;
    if ( (NewPriceE < BuyPrice) && (Percent > STOP_LOSS_LIMIT))// if loss more than 10% 
    {
	  //printf("  Percent = %.2f\n",Percent);	
      ConditionCheckE = 1;
    }

    //
    // Check RSI, KD
    //


    //
    // Return day index and Price
    //
    if(ConditionCheckS)
    {
      *SellDayIndex = CurrentIndex;
      *SellPrice    = NewPriceS;
	    //printf("V(S)\n");  
      return;
    }
	if (ConditionCheckE)
	{
      *SellDayIndex = CurrentIndex;
      *SellPrice    = NewPriceE;
	    //printf("V(E)\n");  	  
      return;
	}
	
    Daily += 1;
  }
  //
  // No sell point
  //
  *SellDayIndex = 0;
  *SellPrice    = 0;
  DEBUG("SellPoint Not Find\n"); 	     
  DEBUG("FindSellPoint End\n");   
}

void AnalysisProfit (TRADE_RECORD  *TradeRecords)
{
   int   Count;
   int   WinCount;
   int   LoseCount;
   int   EarnedMoney;
   int   LoseMoney;
   float Percent;
   float AverWin;
   float AverLose;   
   DAILY_INFO *Daily; 

   
   Daily = InfoBuffer;
   
   DEBUG("AnalysisProfit Start\n");
   EarnedMoney = 0;
   LoseMoney   = 0;
   WinCount    = 0;
   LoseCount   = 0;
   Count       = 1;

  printf("===============================================\n");   
   do{
       printf("\nTradeRecord: %d\n", Count);

       if((TradeRecords->BuyPrice != 0 || TradeRecords->BuyDayIndex != 0) && (TradeRecords->SellPrice != 0 || TradeRecords->SellDayIndex != 0)) /*if buy point and sell point exist*/ 
       {
         printf("[Buy]: DayIndex:%d, price:%d  ====> [Sell]: DayIndex:%d, price:%d\n", TradeRecords->BuyDayIndex+1, TradeRecords->BuyPrice, TradeRecords->SellDayIndex+1, TradeRecords->SellPrice);
		 printf("%d/%d/%d-------------------",(Daily+TradeRecords->BuyDayIndex)->Dates.Years,(Daily+TradeRecords->BuyDayIndex)->Dates.Months,(Daily+TradeRecords->BuyDayIndex)->Dates.Days);                    
         printf("-----------------%d/%d/%d\n",(Daily+TradeRecords->SellDayIndex)->Dates.Years,(Daily+TradeRecords->SellDayIndex)->Dates.Months,(Daily+TradeRecords->SellDayIndex)->Dates.Days);
		 if(TradeRecords->BuyPrice <= TradeRecords->SellPrice)
         {
           EarnedMoney += (TradeRecords->SellPrice - TradeRecords->BuyPrice);
           printf("EarnedMoney = %d\n", TradeRecords->SellPrice - TradeRecords->BuyPrice);
		   WinCount++;
         } else {
           LoseMoney += (TradeRecords->BuyPrice - TradeRecords->SellPrice);
           printf("LoseMoney = %d\n",TradeRecords->BuyPrice - TradeRecords->SellPrice);
		   LoseCount++;   
         }
         Count++;
       }
	 TradeRecords = TradeRecords->Next;
     printf("===============================================\n");    	 
   } while(TradeRecords != NULL);

  Percent = (float)WinCount/(float)Count;
  AverWin  = (float)EarnedMoney/(float)WinCount;
  AverLose = (float)LoseMoney/(float)LoseCount;
  printf("\nTotal Earned = %d, Average Win/Lose money per trade = %.1f/%.1f \n",EarnedMoney - LoseMoney, AverWin, AverLose);
  printf("Win/Lose/Total = %d/%d/%d , %.1f %%Chance to Wins\n", WinCount, LoseCount, Count, Percent*100);
  printf("===============================================\n");    
  DEBUG("AnalysisProfit End\n");    
}

void StockSimulator1(int StartDayIndex, int EndDayIndex, TRADE_RECORD  **ReturnRecordsHead)
{
  int       StartDayIndex1;
  int       BuyDayIndex;
  int       BuyPrice;
  int       SellDayIndex;
  int       SellPrice;
  int       Count;
  TRADE_RECORD  *OldRecords;
  TRADE_RECORD  *NewRecords; 

  DEBUG("StockSimulator Start\n");

  Count = 0;
  OldRecords = NULL;
  NewRecords = NULL;
  SellDayIndex = 0;
  SellPrice    = 0;
  StartDayIndex1 = StartDayIndex;

  while(SellDayIndex+2 < EndDayIndex) //Need at least 2 days left, for buypoint and sellpoint check.
  {  
    //
    // Time order: StartDayIndex-->BuyDayIndex-->SellDayIndex-->EndDayIndex
    //             [---------------FindBuyPoint---------------------------]
    //                            [-------------FindSellPoint-------------]        
    //
    FindBuyPoint(StartDayIndex1, EndDayIndex, &BuyDayIndex, &BuyPrice);

    if((BuyDayIndex != 0 && BuyPrice != 0))
	  {
      printf("<===>\n");
      FindSellPoint(BuyDayIndex+1, EndDayIndex, BuyPrice, &SellDayIndex, &SellPrice); /*if buy, move to next day to find sell*/
	  }

    if ( (BuyDayIndex == 0 && BuyPrice == 0) || (SellDayIndex == 0 && SellPrice == 0) ) /*no sell or buy info ==> finish*/
    {
      break;
    }

    //
    // Record trade events
    //
    NewRecords = (TRADE_RECORD*) malloc(sizeof(TRADE_RECORD));

    NewRecords->BuyDayIndex  = BuyDayIndex;
    NewRecords->SellDayIndex = SellDayIndex;
    NewRecords->BuyPrice     = BuyPrice;
    NewRecords->SellPrice    = SellPrice;
    NewRecords->Next         = 0;

    if(Count == 0) {
      *ReturnRecordsHead = NewRecords;
    } else {	
      OldRecords->Next = NewRecords;
    }
      OldRecords = NewRecords;
    //
    // Next time StartDayIndex equals SellDayIndex+1 at this times
    //
    StartDayIndex1 = SellDayIndex+1;
    Count += 1;
	  printf(" \n");  
  }

  DEBUG("StockSimulator End\n");  
}
