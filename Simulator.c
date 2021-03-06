#include "DataDefine.h"

#define WriteDates(x,y,z,fp)    PrepareDateStr( x, y, z, fp);\

#define WriteNum(x,fp)          PrepareNumberStr(x,fp);\

#define WriteFloatNum(x,fp)     FixPoint2(x);\
                                WriteFloatString(x,fp);\

//#define FPUT(Istr,fp)              FputCL(Istr,fp);\

#define MoveDayAgoPtr(x)   SimCurr -= x;\
                           ShiftDay = x;

#define MoveDayBackPtr(x)  SimCurr += x;

int ShiftDay;

typedef struct _Rule_Nodes RuleNodes;

#define Open_Op           1
#define High_Op           2
#define Low_Op            3
#define Close_Op          4
#define MA_Op             5
#define RSI_Op            6
#define KD_K_Op           7
#define KD_D_Op           8
#define MACD_Op           9
#define Vaule_Op          10

#define Bigger_Op         20
#define Smaller_Op        21
#define Equal_Bigger_Op   22
#define Equal_Smaller_Op  23
#define Equal_Op          24
#define Plus_Op           25
#define Sub_Op            26
#define Multi_Op          27
#define Divide_Op         28
#define CrossUp_Op        29
#define CrossDown_Op      30
#define Left_Bracket      31
#define Right_Bracket     32
#define And_Op            33
#define Or_Op             34

#define Lowest_function   40
#define Highest_function  41
#define Average_function  42
#define Sum_function      43

TRADE_RECORD  *RecordHead = NULL;
TRADE_RECORD  *RecordCurrent = NULL;

//int         Current;

struct _Rule_Nodes
{
  int    shares; // root only
  int    *SymbolList;
  int    Levels; // level 0 ([]and[]) <= if bracket only way is going to level 0 , level 1 ([]>[]), level 2 ([]+[]), level 3 ([]*[]) <= can calculate directly
  int    *Rule;
  float  RetValue;
  int    ChildCount;

  RuleNodes *Parent;
  RuleNodes **Child;

};

DAILY_INFO *SimCurr;

void  LoadRule();
void  BuildTree(RuleNodes *Root);
void  AddTree(RuleNodes *Root);
int   SplitRule (int *SplitSymbol, int SymbolCount, int *Rule, int **SplitRet, int **SymbolListRet);
void  PrintRule(int *Rule);
void  CheckLastBracket(int *Rule);
float Price(char *type,int day);
float MA(int day);
void  KD(float *K, float *D, int day);
float RSI(int day);
float MACD(int EMA1day, int EMA2day);
void  BuyOrSell(char Options, char *Type, int Shares);

int  StockId;

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

RuleNodes **RootBuyAll;
RuleNodes **RootSellAll;
RuleNodes **RootBuyNextAll;

char  *TechDataName;
int     FixFlag;

void InitTechDataName()
{
  int   i;
  char  *str;

  TechDataName = (char *)malloc(150);
  TechDataName = "TechResult";

  printf("Filename = %s\n",TechDataName);  

  i = 0;
  while(1)
  {
    if ( *(TechDataName+i) == '\0' )
    {
      str = TechDataName+i;
      break;
    }
    i++;
  }

  _itoa(StockId,str,10);

  printf("Filename = %s\n",TechDataName);  
}

/***
 To prevet date number string like "2020/1/1"
 should be "2020/01/01"
***/
void InsertZeroForDate(char *Dates)
{
  char Val;
  if(*(Dates+1) == '\0') // if single number
  {
    Val = *(Dates);
    *(Dates)   = '0';
    *(Dates+1) = Val;
    *(Dates+2) = '\0';
  }
}

void PrepareNumberStr(int Num, FILE *fp)
{
  char  *temp, *str;
  int    i = 0;

  str = (char*) malloc(50);
  temp = (char*) malloc(50);

  _itoa(Num,temp,10);
  
  while(1)
  {
    *(str+i) = *(temp+i);

    if( *(temp+i) == 0)
    {
      *(str+i) = 10; //New line
      break;
    }
    i++;
  }
  *(str+i+1) = 0;

  fputs(str,fp);
}

void PrepareDateStr(int y, int m, int d, FILE *fp)
{
  char  *temp;
  char  *str;

  str = (char*) malloc(50);
  temp = (char*) malloc(50);

  _itoa(y,temp,10);

  *str     = *temp;
  *(str+1) = *(temp+1);
  *(str+2) = *(temp+2); 
  *(str+3) = *(temp+3);
  *(str+4) = '/';

  _itoa(m,temp,10);

  InsertZeroForDate(temp);

  *(str+5)  = *temp;
  *(str+6)  = *(temp+1);
  *(str+7)  = '/';

  _itoa(d,temp,10);
  InsertZeroForDate(temp);

  *(str+8)   = *temp;
  *(str+9)   = *(temp+1);
  *(str+10)  = 10; // newline
  *(str+11)  = 0;  // end

  fputs(str,fp);

  printf("========%s",str);

}
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

void FputCL(char *str, FILE *fp)
{
  int i = 0;
  char  *temp;

  temp = (char*) malloc(50);

  printf("FPUTCL = %d,%d,%d,%d ",*str,*(str+1),*(str+2),*(str+3));

  while(1)
  {
    *(temp+i) = *(str+i);
    if( *(str+i) == 0)
    {
      *(temp+i) = 10; //New line
      break;
    }
    i++;
  }
  *(temp+i+1) = 0;
  printf("temp = %s ",temp);
  fputs(temp,fp);
}
/***
 To prevet float number string like "200."
 should be "200.0"
***/
void WriteFloatString(float x, FILE *fp)
{
  int  i;
  char TheLastIsPoint = 0;
  char IsPoint = 0;
  char *Num;

  Num = (char*) malloc(50);

  gcvt(x,6,Num);

  i = 0;

  while(*(Num+i) != '\0')  // case of 85. => fill it to 85.00
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
    *(Num+i+2) =  0;
  }
    
  i = 0;
  while(*(Num+i) != '\0')  // case of 85.1 => fill it to 85.10
  {
    if ( *(Num+i) == '.')
    {
      if(*(Num+i+2) == '\0')
      {
        *(Num+i+2) = '0';
      }
      *(Num+i+3) =  10;//change line
      *(Num+i+4) =  0;
      break;
    }
    i += 1;
  }
  
  i = 0;
  while(*(Num+i) != '\0')  //  make sure XX.XX 
  {
    if ( *(Num+i) == '.')
    {
      *(Num+i+3) = 10; //change line
      *(Num+i+4) = 0;
      break;
    }
    i += 1;
  }

  if(FixFlag == 1)  // case of  0 < value < 0.1
  {
    *(Num+3) = *(Num);
    *(Num)   = '0';
    *(Num+2) = '0';
    *(Num+4) =  10; //change line
    *(Num+5) =  0;
  }
  if(FixFlag == 2) // case of  0 > value > -0.1
  {
    *(Num+4)  = *(Num+1);
    *(Num+1)  = '0';
    *(Num+3)  = '0';
    *(Num+5)  =  10;  //change line
    *(Num+6)  =   0;
  }
  printf("float = %s  ",Num);
  fputs(Num,fp);
}

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


void ReadLine(FILE *fp,char *str)
{
  fscanf(fp,"%s",str);
}

float Price(char *type,int day)
{
  float Ret;

  MoveDayAgoPtr(day)
  if(!strcmp("Open",type))
    Ret = SimCurr->Start;
  else if (!strcmp("High",type))
    Ret = SimCurr->High;
  else if (!strcmp("Low",type))
    Ret = SimCurr->Low;
  else if (!strcmp("Close",type))
    Ret = SimCurr->End;
  MoveDayBackPtr(day)
  //printf("Price:%f\n",Ret);
  return Ret;
}

float MA(int day)
{
  float Ret;
  char  *str;
  FILE  *fp;
  int i,j,CountNum;
  int match;
  int length;
  int Counter;

  str = (char *)malloc(50);

  match   = 0;
  Counter = 0;
  CountNum = 0;
  Ret = 0;

  //printf("MA(%d)\n",day);

  for(i = 0; i < MA_count; i++)
  {
    if(*(MA_value+i) == day)
    {
      CountNum = i;
    }
  }

  //printf("CountNum = %d\n",CountNum);
  fp = fopen(TechDataName,"r");
  if (fp == NULL)
  {
    printf("Fail to open!");
    return -1;
  }
  //printf("MA~~\n");

  ReadLine(fp,str);
  ReadLine(fp,str);
  length = (int)atoi(str);
  //printf("length = %d\n",length);

  while(1)
  {
    ReadLine(fp,str);

    if(!strcmp("MA",str))
    {
      //printf("MA lebal\n");
      match = 1;
      continue;
    }

    if (Counter % length == 0  && Counter/length == CountNum)
    {
      //printf("match2!!\n");
      match = 2;
    }

    //if (match == 4)
    //printf("Counter mod length = %d ,DayInedx = %d \n",Counter%length,(SimCurr->DayIndex - StartDayIndex));
    //printf("counter = %d\n",Counter);
    if  (Counter % length == (SimCurr->DayIndex - StartDayIndex) && match == 2)
    {
      Ret = (float)atof(str);
      //printf("Counter mod length = %d ,DayInedx = %d \n",Counter%length,(SimCurr->DayIndex - StartDayIndex));
      //printf("find = %f ,%d-%d-%d\n",Ret,SimCurr->Dates.Years,SimCurr->Dates.Months,SimCurr->Dates.Days);
      match = 0;
      break;
    }

    if(match > 0)
    {
      Counter++;
    }

  }

  fclose(fp);

  return Ret;
}

void KD(float *K, float *D, int day)
{
  char  *str,*str1;
  FILE  *fp;
  int i,j,CountNum;
  int match;
  int length;
  int Counter;

  str = (char *)malloc(50);
  str1 = (char *)malloc(20);

  for(i = 0; i < KD_count; i++)
  {
    if(*(KD_value+i) == day)
    {
      CountNum = i;
    }
  }

  match   = 0;
  Counter = 0;

  fp = fopen(TechDataName,"r");

  ReadLine(fp,str);
  ReadLine(fp,str);
  length = (int)atoi(str);
  printf("length = %d\n",length);

  while(1)
  {
    ReadLine(fp,str);

    if(!strcmp("KD",str))
    {
      //printf("KD lebal\n");
      match = 1;
      continue;
    }

    if (Counter % length == 0  && Counter/length == CountNum)
    {
      //printf("match2!!\n");
      match = 2;
    }

    //if (match == 4)
    //printf("Counter mod length = %d ,DayInedx = %d \n",Counter%length,(SimCurr->DayIndex - StartDayIndex));
    //printf("counter = %d\n",Counter);
    if  (Counter % length == (SimCurr->DayIndex - StartDayIndex) && match == 2)
    {
      //printf("find\n");
      // Cutting string with ','
      j = 0;
      while(1)
      {
        if(*(str+j) == ',')
        {
          str1 = str+j;
          *D   = (float)atof(str1+1);
          *(str+j) = '\0';
          *K   = (float)atof(str);
        }
      }
      //(float)atof(str);
      match = 0;
      break;
    }

    if(match > 0)
    {
      Counter++;
    }
  }

  fclose(fp);

}

float RSI(int day)
{
  float Ret;
  char  *str;
  FILE  *fp;
  int i,CountNum;
  int match;
  int length;
  int Counter;

  str = (char *)malloc(50);

  for(i = 0; i < RSI_count; i++)
  {
    if(*(RSI_value+i) == day)
    {
      CountNum = i;
    }
  }

  match   = 0;
  Counter = 0;
  Ret = 0;

  fp = fopen(TechDataName,"r");

  ReadLine(fp,str);
  ReadLine(fp,str);
  length = (int)atoi(str);
  printf("length = %d\n",length);

  while(1)
  {
    ReadLine(fp,str);

    ReadLine(fp,str);

    if(!strcmp("RSI",str))
    {
      //printf("RSI lebal\n");
      match = 1;
      continue;
    }

    if (Counter % length == 0  && Counter/length == CountNum)
    {
      //printf("match2!!\n");
      match = 2;
    }

    //if (match == 4)
    //printf("Counter mod length = %d ,DayInedx = %d \n",Counter%length,(SimCurr->DayIndex - StartDayIndex));
    //printf("counter = %d\n",Counter);
    if  (Counter % length == (SimCurr->DayIndex - StartDayIndex) && match == 2)
    {
      Ret = (float)atof(str);
      //printf("Counter mod length = %d ,DayInedx = %d \n",Counter%length,(SimCurr->DayIndex - StartDayIndex));
      //printf("find = %f ,%d-%d-%d\n",Ret,SimCurr->Dates.Years,SimCurr->Dates.Months,SimCurr->Dates.Days);
      match = 0;
      break;
    }

    if(match > 0)
    {
      Counter++;
    }

  }

  fclose(fp);

  return Ret;
}

float MACD(int EMA1day, int EMA2day)
{
  float Ret;
  Ret = 0;
  return Ret;
}

void Buy(char *Type, int Shares)
{
  BuyOrSell(1,Type,Shares);
}

void Sell(char *Type, int Shares)
{
  BuyOrSell(0,Type,Shares);
}

void BuyOrSell(char Options, char *Type, int Shares)
{
  TRADE_RECORD  *Record;
  float PRICE;
  int   DayIndex;
  DATE  Dates;

  if(!strcmp("Now",Type))
  {
    PRICE    = Price ("Close",0);
    printf("price = %f,shares = %d\n",PRICE,Shares);
    DayIndex = SimCurr->DayIndex;
    Dates    = SimCurr->Dates;    
  }
  if(!strcmp("BuyNext",Type))
  {
    PRICE    = (SimCurr+1)->Start;
    DayIndex = (SimCurr+1)->DayIndex;
    Dates    = (SimCurr+1)->Dates;       
  }

  if (RecordHead == NULL)
  {
    if(Options) //Block sell with no shares remain
    {
      Record = (TRADE_RECORD *) malloc(sizeof(TRADE_RECORD));
      Record->Price     = PRICE;
      Record->DayIndex  = DayIndex;
      Record->Dates     = Dates; 

      Record->BuyOrSell = 1;
      Record->ShareTrades = Shares;  
      Record->SharesRemaining = Shares;
      Record->Next     = NULL;
      RecordHead    = Record;
      RecordCurrent = Record;      
    }
    return;
  }

  if(!Options) //Block sell with no shares remain
  {
    if(RecordCurrent->SharesRemaining < Shares)
    {
      // Shares remain not enough
      return;
    }
  } else  //Block buy with shares remain
  {
    //if(RecordCurrent->SharesRemaining != 0)
    //{
    //  // Shares remains
    //  return;
    //}
  }

  Record = (TRADE_RECORD *) malloc(sizeof(TRADE_RECORD));
  printf("NEW TRADE_RECORD\n");
  Record->Price    = PRICE;
  Record->DayIndex = DayIndex;
  Record->Dates    = Dates; 

  Record->BuyOrSell = Options ? 1 : 0;
  Record->ShareTrades = Shares;
  Record->Next     = NULL;

  if(Options)
  {
  Record->SharesRemaining = RecordCurrent->SharesRemaining + Shares;
  } else
  {
  Record->SharesRemaining = RecordCurrent->SharesRemaining - Shares;
  }

  RecordCurrent->Next = Record;
  RecordCurrent = Record;
}

float HandleFunctionItem(int *Rule)
{
  int   i;
  int   op;
  int   op_value;
  int   op2;
  int   op2_value;
  int   *NewRule;
  float Ret;
  float Highest;
  float Lowest;

  NewRule = (int *) malloc(5);

  Ret = 0;
  Highest = Ret;
  Lowest  = Ret;

  op        = *(Rule+1);
  op_value  = *(Rule+2);
  op2       = *(Rule+3);
  op2_value = *(Rule+4);

  *NewRule     = *Rule -2;  // length - 2
  *(NewRule+1) = op2;
  *(NewRule+2) = op2_value;

  for (i = 0; i < op_value; i++)
  {
    MoveDayAgoPtr(i)

    if (op == Sum_function || Average_function)
      Ret += CalRule(NewRule);

    if (op == Highest_function)
    {
      Ret = CalRule(NewRule);
      if (Ret > Highest)
        Highest = Ret;
    }

    if (op == Lowest_function)
    {
      Ret = CalRule(NewRule);
      if (Ret < Lowest)
        Lowest = Ret;
    }

    MoveDayBackPtr(i)
  }

  if (op == Average_function)
    Ret /= op_value;

  free(NewRule);

  return Ret;
}

void TraceNode(RuleNodes *Root)
{
  int NumChild;
  int i;

  NumChild = Root->ChildCount;
  printf("=========\nRoot Address = %x\n",Root);
  printf("Child Num = %d\n",NumChild);
  printf("Level%d Rule->",Root->Levels);
  PrintRule(Root->Rule);
  printf("\n=========\n");
  for( i = 0; i < NumChild; i++ )
  {
    printf("To Child = %x\n",*(Root->Child+i));
    TraceNode(*(Root->Child+i));
  }
}

float GetVaule(int *Rule)
{
  float Ret;
  int op;
  int op_value;
  int op2;
  int op2_value;
  float K,D;

  op        = *(Rule+1);
  op_value  = *(Rule+2);

  //printf("Op = %d, OpV = %d\n",op,op_value);

  if (op == Open_Op)
  {
    Ret = Price("Open", op_value);
  } else if (op == High_Op)
  {
    Ret = Price("High", op_value);
  } else if (op == Low_Op)
  {
    Ret = Price("Low", op_value);
  } else if (op == Close_Op)
  {
    Ret = Price("Close", op_value);
  } else if (op == MA_Op)
  {
    Ret = MA(op_value);
  } else if (op == RSI_Op)
  {
    Ret = RSI(op_value);    
  } else if (op == KD_K_Op || op == KD_D_Op)
  {
    KD(&K,&D,op_value);    
    Ret = (op == KD_K_Op) ? K:D;
  }  else if (op == MACD_Op)
  {
    //Ret = MACD_(op, op_value);    
  } else if(op == Lowest_function || op == Highest_function  || op == Average_function  || op == Sum_function )
  {
    Ret = HandleFunctionItem(Rule);
  }

  return Ret;
}

float CalRule(RuleNodes *Root)
{
  float value;
  int i,j;
  int ChildC;
  int op;

  value = 0;
  ChildC = Root->ChildCount;
  //printf("\n=======================\nCalRule Entry:\n");
  //PrintRule(Root->Rule);

  if (ChildC == 0)
    return GetVaule ( Root->Rule );

  //printf("\nThe  most left item V");
  value +=  CalRule(*(Root->Child));

  for (i= 1; i < ChildC ; i++)
  {
    //printf("Child(%d)(op%d)(%d)\n",i-1,op,i);
    op = *(Root->SymbolList+i); // [Child1] symbol1(op) [Child2] symbol2 [Child3] ....
    //printf("Child(%d)(op%d)(%d)\n",i-1,op,i);
    if(op == Plus_Op)
    {
      value += CalRule(*(Root->Child+i));
    }
    else if (op == Sub_Op)
    {
      value -= CalRule(*(Root->Child+i));
    }
    else if (op == Multi_Op)
    {
      value *= CalRule(*(Root->Child+i));
    }
    else if (op == Divide_Op)
    {
      value /= CalRule(*(Root->Child+i));
    }

    if(op == Bigger_Op)
    {
      if ( CalRule(*(Root->Child+i-1)) > CalRule(*(Root->Child+i)) )
      {
        //printf("  --Bigger\n");
        value = 1; //True
      } else
      {
        //printf("  --not Bigger\n");
        value = 0;
      }
    } else if(op == Smaller_Op)
    {
      if ( CalRule(*(Root->Child+i-1)) < CalRule(*(Root->Child+i)) )
      {
        //printf("  --Smaller\n");
        value = 1; //True
      } else
      {
        //printf("  --not Smaller\n");
        value = 0;
      }
    } else if(op == Equal_Bigger_Op)
    {
      if ( CalRule(*(Root->Child+i-1)) >= CalRule(*(Root->Child+i)) )
      {
        value = 1; //True
      } else
      {
        value = 0;
      }
    }else if(op == Equal_Smaller_Op)
    {
      if ( CalRule(*(Root->Child+i-1)) <= CalRule(*(Root->Child+i)) )
      {
        value = 1; //True
      } else
      {
        value = 0;
      }
    } else if(op == Equal_Op)
    {
      if ( CalRule(*(Root->Child+i-1)) == CalRule(*(Root->Child+i)) )
      {
        value = 1; //True
      } else
      {
        value = 0;
      }
    } else if(op == CrossUp_Op)
    {
      //printf("\nOp CrossUp\n");
      // Assign False at first 
      value = 0;
      //printf("SimCurr=%x\n",SimCurr);
      MoveDayAgoPtr(1)
      //printf("SimCurr=%x,ShiftDayAgo 1\n",SimCurr);
      if ( CalRule(*(Root->Child+i-1)) < CalRule(*(Root->Child+i)) )  // left op < right op
      {
        MoveDayBackPtr(1)
        //printf("\nSimCurr=%x,ShiftDayBack\n",SimCurr);
        if ( CalRule(*(Root->Child+i-1)) > CalRule(*(Root->Child+i)) ) // left op > right op
          {
            //printf("CroosUp success!!!\n");
            value = 1; //True
          }
      }
    } else if(op == CrossDown_Op)
    {
      // Assign False at first
      value = 0;
      MoveDayAgoPtr(1)
      if ( CalRule(*(Root->Child+i-1)) > CalRule(*(Root->Child+i)) )  // left op > right op
      {
        MoveDayBackPtr(1)
        if ( CalRule(*(Root->Child+i-1)) < CalRule(*(Root->Child+i)) )  // left op < right op
          {
            value = 1;  //True
          }
      }
    }

    if (op == And_Op)
    {
      value = (int)value & (int)(CalRule(*(Root->Child+i)));
    } else if (op == Or_Op)
    {
      value = (int)value | (int)(CalRule(*(Root->Child+i)));
    }
  }
  //printf("cal final = %f \n==========\n\n",value);
  return value;
}

void PrintRule(int *Rule)
{
  int len;
  int i;

  len = *Rule;
  //printf("len = %d ",len);
  printf("rule(%d): ",len);
  for (i = 1; i <= len; i++)
  {
    if(i%2 == 1)
    printf(" %d,",*(Rule+i));
    if(i%2 == 0)
    {
      printf("(%d) ",*(Rule+i));
      continue;
    }
    if(*(Rule+i) == Open_Op)
      printf("Open");
    else if(*(Rule+i) == High_Op)
      printf("High");
    else if(*(Rule+i) == Low_Op)
      printf("Low");
    else if(*(Rule+i) == Close_Op)
      printf("Close");
    else if(*(Rule+i) == MA_Op)
      printf("MA");
    else if(*(Rule+i) == RSI_Op)
      printf("RSI");
    else if(*(Rule+i) == KD_K_Op)
      printf("KD_K");
    else if(*(Rule+i) == KD_D_Op)
      printf("KD_D");      
    else if(*(Rule+i) == MACD_Op)
      printf("MACD");
    else if(*(Rule+i) == Vaule_Op)
      printf("VALUE");
    else if(*(Rule+i) == Bigger_Op)
      printf(">");
    else if(*(Rule+i) == Smaller_Op)
      printf("<");
    else if(*(Rule+i) == Equal_Bigger_Op)
      printf(">=");
    else if(*(Rule+i) == Equal_Smaller_Op)
      printf("<=");
    else if(*(Rule+i) == Equal_Op)
      printf("=");
    else if(*(Rule+i) == Plus_Op)
      printf("+");
    else if(*(Rule+i) == Sub_Op)
      printf("-");
    else if(*(Rule+i) == Multi_Op)
      printf("*");
    else if(*(Rule+i) == Divide_Op)
      printf("/");
    else if(*(Rule+i) == CrossUp_Op)
      printf("CrossUP");
    else if(*(Rule+i) == CrossDown_Op)
      printf("Crossdown");
    else if(*(Rule+i) == Left_Bracket)
      printf("(");
    else if(*(Rule+i) == Right_Bracket)
      printf(")");
    else if(*(Rule+i) == And_Op)
      printf("And");
    else if(*(Rule+i) == Or_Op)
      printf("Or");                                              
  }
  printf("-- \n");
}

void MainProcess()
{
  int *RuleBuyPtr;
  int *RuleSellPtr;
  int *RuleBuyNextPtr;
  int i,j,k;
  int len;

  RuleNodes **RootBuyAllPtr;
  RuleNodes **RootSellAllPtr;
  RuleNodes **RootBuyNextAllPtr;

  RuleNodes *RootBuy;
  RuleNodes *RootSell;
  RuleNodes *RootBuyNext;

  LoadRule();

  RootBuyAll     = (RuleNodes **) malloc (sizeof(RuleNodes **)*50);
  RootSellAll    = (RuleNodes **) malloc (sizeof(RuleNodes **)*50);
  RootBuyNextAll = (RuleNodes **) malloc (sizeof(RuleNodes **)*50);

  RootBuyAllPtr     = RootBuyAll;
  RootSellAllPtr    = RootSellAll;
  RootBuyNextAllPtr = RootBuyNextAll;

  RuleBuyPtr     = RuleBuy;
  RuleSellPtr    = RuleSell;
  RuleBuyNextPtr = RuleBuyNext;
  //printf("%x\n",malloc(100));
  //printf("%x\n",malloc(100));
  j = 0;
  while(j < RuleBuyCount)
  {
    RootBuy = (RuleNodes *) malloc(sizeof(RuleNodes)*2);

    RootBuy->Parent = NULL;
    RootBuy->shares = *(RuleBuyPtr+1);
    *(RuleBuyPtr) = *(RuleBuyPtr) -1;
    len = *RuleBuyPtr;

    //shift shares
    for(int k = 1; k <= len; k++)
    {   
      *(RuleBuyPtr+k) = *(RuleBuyPtr+k+1);  
    }
    RootBuy->Levels = CheckRuleLevel(RuleBuyPtr);
    RootBuy->Rule   = RuleBuyPtr;
    RootBuy->ChildCount = 0;
    printf("root level = %d  ",RootBuy->Levels );
 
    BuildTree(RootBuy);

    RuleBuyPtr += len+2;
    *RootBuyAllPtr = RootBuy;
    printf("\n Root address: %x\n",RootBuy);
    RootBuyAllPtr++;
    j++;
  }
  //printf("\n\na = %x,b = %x \n\n",*RootBuyAll,RootBuy);
  //printf("\n\na = %x,b = %x \n\n",*(RootBuyAll+1),RootBuy);
  //TraceNode(RootBuy);
  //for(j = 0; j < RuleBuyCount; j++)
  //{
    printf("\n\n\n ");
    //TraceNode(*(RootBuyAll+i));
    //TraceNode(RootBuy);
    printf("\n\n\n ");
  //}  
  //TraceNode(*RootBuyAll);

  
  j = 0;
  while(j < RuleSellCount)
  {
    RootSell = (RuleNodes *) malloc(sizeof(RuleNodes)*2);
    printf("\nBuild Sell root(%d) \n=============\n",j);

    RootSell->Parent = NULL;
    RootSell->shares = *(RuleSellPtr+1);
    *(RuleSellPtr) = *(RuleSellPtr) -1;
    len = *RuleSellPtr;

    //shift shares
    for(int k = 1; k <= len; k++)
    {   
      //printf("b = %d ,b2 = %d, ",*(RuleBuyPtr+k),*(RuleBuyPtr+k+1));
      *(RuleSellPtr+k) = *(RuleSellPtr+k+1);
      //printf("a = %d \n",*(RuleBuyPtr+k));      
    }

    RootSell->Levels = CheckRuleLevel(RuleSellPtr);
    RootSell->Rule   = RuleSellPtr;
    RootSell->ChildCount = 0;
    //PrintRule(RuleSellPtr);
    BuildTree(RootSell);

    RuleSellPtr += len+2;
    //printf("RuleSellPtr += len+1 = %d %d %d %d %d\n",*RuleSellPtr,*(RuleSellPtr+1),*(RuleSellPtr+2),*(RuleSellPtr+3),*(RuleSellPtr+4));
    *RootSellAllPtr = RootSell;
    RootSellAllPtr++;
    //printf("\n Root address: %x\n",RootSell);
    j++;
    //TraceNode(RootSell);
  }
  //printf("\n\na = %x\n\n",*RootSellAll);
  //printf("\n\na = %x\n\n",*(RootSellAll+1));
  //printf("\n\na = %x\n\n",*(RootSellAll+2));

  printf("\n\n\n ");
  for(j = 0; j < RuleSellCount; j++)
  {
    TraceNode(*(RootSellAll+j));
  }
  printf("\n\n\n ");  
    //calloc(100,sizeof(int));
  j = 0;
  while(j < RuleBuyNextCount)
  {
    RootBuyNext = (RuleNodes *) malloc(sizeof(RuleNodes)*2);

    RootBuyNext->Parent = NULL;
    RootBuyNext->shares = *(RuleBuyNextPtr+1);
    *(RuleBuyNextPtr) = *(RuleBuyNextPtr) -1;
    len = *RuleBuyNextPtr;

    //shift shares
    for(int k = 1; k <= len; k++)
    {   
      //printf("b = %d ,b2 = %d, ",*(RuleBuyPtr+k),*(RuleBuyPtr+k+1));
      *(RuleBuyNextPtr+k) = *(RuleBuyNextPtr+k+1);
      //printf("a = %d \n",*(RuleBuyPtr+k));      
    }
    RootBuyNext->Levels = CheckRuleLevel(RuleBuyNextPtr);
    RootBuyNext->Rule   = RuleBuyNextPtr;
    RootBuyNext->ChildCount = 0;
    PrintRule(RuleBuyNextPtr);
    BuildTree(RootBuyNext);

    RuleBuyNextPtr += len+2;
    *RootBuyNextAllPtr = RootBuyNext;
    RootBuyNextAllPtr++;
    j++;
  }

}

void LoadRule ()
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

  TradeMode = 0;

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

  TradeMode = 0;

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

void BuildTree(RuleNodes *Root)
{
  int i,ChildC;
  RuleNodes *ChildN;

  printf("\nbuild tree Root:\n");
  PrintRule(Root->Rule);

  AddTree(Root);

  ChildC = Root->ChildCount;
  printf("child Num of this root = %d\n",ChildC);
  // scan all the child
  for(i = 0; i < ChildC; i++)
  {
    printf("\nbuild child(%d) of root = %x\n",i,*(Root->Child + i));
    ChildN = *(Root->Child + i);
    //printf("Child?\n");
    BuildTree(ChildN);
    printf("build child(%d) done\n\n",i);
  }
  printf("build child all done for :");
  PrintRule(Root->Rule);
  printf("======================\n");
}

int CheckRuleLevel(int *Rule)
{
  int AndOr[] = {And_Op, Or_Op};
  int Condition[] = {Bigger_Op, Smaller_Op, Equal_Bigger_Op, Equal_Smaller_Op, Equal_Op, CrossUp_Op, CrossDown_Op};
  int Operater1[] = {Plus_Op, Sub_Op};
  int Operater2[] = {Multi_Op, Divide_Op};

  int lenA,lenC,lenO;
  int len;
  int i,j;
  int Level;
  // check symbol

  i    = 0;
  lenA = 2;
  lenC = 7;
  lenO = 2;
  len = *Rule;

  Rule++;

  Level = 4;

  while(i < len)
  {
    //printf("item = %d,i = %d\n",*Rule,i);
    if( i %2 == 0)
    {
      for (j = 0; j < lenO; j++)
      {
        if(*Rule == Operater1[j] && Level > 3)
          Level = 3;
      }

      for (j = 0; j < lenC; j++)
      {
        if(*Rule == Condition[j] && Level > 2)
          Level = 2;
      }

      for (j = 0; j < lenA; j++)
      {
        if(*Rule == AndOr[j] && Level > 1)
          Level = 1;
      }
    }

    Rule++;
    i++;
  }
  return Level;
}

void AddTree(RuleNodes *Root)
{
  // Parsing level bracket -> 'and' 'or' -> '>' '<' '=' cross -> '+' '-' -> '*' '/'
  int AndOr[] = {And_Op, Or_Op};
  int Condition[] = {Bigger_Op, Smaller_Op, Equal_Bigger_Op, Equal_Smaller_Op, Equal_Op, CrossUp_Op, CrossDown_Op};
  int Operater1[] = {Plus_Op, Sub_Op};
  int Operater2[] = {Multi_Op, Divide_Op};

  int *SplitBuffer;
  int *SplitPtr;
  int *SymbolBuffer;
  int ChildNum;
  int i,j,len;
  int *Rule;
  int level;
  //int IsBracketDetect;

  printf("Root = %x\n",Root);

  RuleNodes *NewNode;
  //printf("add tree1\n");
  
  Rule = Root->Rule;
  level = Root->Levels;

  ChildNum = 0;

  SplitRule(AndOr,2,Rule,&SplitBuffer,&SymbolBuffer);
  if (*SymbolBuffer == 0)
    SplitRule(Condition,7,Rule,&SplitBuffer,&SymbolBuffer);
  if (*SymbolBuffer == 0)
    SplitRule(Operater1,2,Rule,&SplitBuffer,&SymbolBuffer);
  if (*SymbolBuffer == 0)
    SplitRule(Operater2,2,Rule,&SplitBuffer,&SymbolBuffer);

  if (*SymbolBuffer == 0 ) {
    Root->Levels = 4;
  } else if (*SymbolBuffer != 0) {   //If split

    ChildNum = (*SymbolBuffer) + 1; // child num = symbol + 1
    printf("child num: %d\n\n",ChildNum);
    Root->Child = (RuleNodes **) calloc(sizeof(RuleNodes *),ChildNum);
    printf("Root->Child = %x\n",Root->Child);
    Root->SymbolList = SymbolBuffer;

    SplitPtr = SplitBuffer;

    for (i = 0; i < ChildNum; i++)
    {
      NewNode = (RuleNodes *) malloc(sizeof(RuleNodes)*2);
      printf("New child(%d) = %x\n",i,NewNode);
      NewNode->Parent = Root;

      j = 0;

      while(j < i+1)
      {
        len = *SplitPtr;
        printf("len(%d) = %d  ",i,len);
        j++;
      }
      printf("child(%d)rule : ",i);
      PrintRule(SplitPtr);
      CheckLastBracket(SplitPtr);
      NewNode->Rule   = SplitPtr;
      NewNode->Child  = NULL;
      NewNode->Levels = CheckRuleLevel(NewNode->Rule);
      NewNode->ChildCount = 0;

      *(Root->Child+i) = NewNode;
      Root->ChildCount += 1;

      SplitPtr += len+1;
    }
  }

  //SplitRule
}

void CheckLastBracket(int *Rule)
{
  int   len;
  int   balance;
  int   *Rhead;
  int   Not;
  int   i;

  i   = 0;
  Not = 0;
  balance = 0;
  Rhead = Rule;
  len = *Rule;
  Rule++;

  if (*Rule != 31)
  {
    Not = 1;
  }

  // Yes, the first one is bracket
  Rule++;
  i++;
  balance++;
  //printf("len? =   %d",len);
  while(i < len-2)
  {
    //printf("cmp rule = %d,i=%d\n",*Rule,i);
    if(balance == 0 )
    {
      Not = 1;
    }

    if (*Rule == Left_Bracket && i % 2 == 1)
    {
      balance++;
    }
    if (*Rule == Right_Bracket && i % 2 == 1)
    {
      balance--;
    }
    //printf(" balance = %d ",balance);
    Rule++;
    i++;
  }

  //printf("Not = %d\n",Not);
  if(Not != 1)
  {
    //printf("detect! before\n");
    //PrintRule(Rhead);
    //remove first and last
    *Rhead = *Rhead - 4;
    for(i = 1; i <= len-1 ; i++)
    {
      *(Rhead+i) = *(Rhead+i+2);
    }
    //printf("after\n");
    //PrintRule(Rhead);
  }

}

// Return bracket detect
int SplitRule (int *SplitSymbol, int SymbolCount, int *Rule, int **SplitRet, int **SymbolListRet)
{
  int  *RulePtr;

  //int  *Split;
  int  *SplitPtr;
  int  *SplitPtrHead;
  int  SplitCount;

  //int  *SymbolList;
  int  *SymbolListPtr;
  int  *SymbolListPtrHead;
  int  SymbolListCount;

  int  IsBracketDetect;
  int  InBracket;

  int  match;
  int  i;
  int  ind;
  int  len;
  int  balance;

  printf("split symbol by %d,%d, c= %d\n",*SplitSymbol,*(SplitSymbol+1),SymbolCount);

  balance = 0;
  IsBracketDetect = 0;

  // don't slip in bracket

  //SplitPtr       = SplitRet;
  //SplitPtrHead   = SplitRet;         // place len info in split rule's head
  SplitPtr       = (int *)malloc(300);  
  *SplitRet      = SplitPtr;
  SplitPtrHead   = SplitPtr;
  SplitCount     = 0;
  SplitPtr++;

  //SymbolListPtr     = SymbolListRet;
  //SymbolListPtrHead = SymbolListRet; // place len info in Symbol List's head

  SymbolListPtr     = (int *)malloc(300);
  *SymbolListRet    = SymbolListPtr;
  //SymbolListPtrHead = SymbolListPtr; // place len info in Symbol List's head
  SymbolListCount = 0;
  SymbolListPtr++;

  i = 0;

  len     = *Rule;      // len is place in fisrt item of rule
  RulePtr = Rule+1;
  match = 0;
  printf("split len = %d\n",len);
  while(i < len)
  {
    //printf("RulePtr = %d\n",*RulePtr);
    if (*RulePtr == Left_Bracket && i%2 == 0 )  // ignore 1 level bracket and near symbol
    {
      //printf("balance+");
      balance++;
    }

    if (*RulePtr == Right_Bracket && i%2 == 0 &&  balance != 0)
    {
      //printf("balance-");
      balance--;
    }

    match = 0;

    for(ind = 0; ind < SymbolCount; ind++)
    {
      if( *(SplitSymbol+ind) == *RulePtr && i%2 == 0 && balance == 0) //match
      {
        printf("match -split count = %d\n",SplitCount);
        *SymbolListPtr = *RulePtr;
        *SplitPtrHead  = SplitCount;
        printf("   SplitPtrHead =%d = %d = %d =  %d =  %d  ",*SplitPtrHead,*(SplitPtrHead+1),*(SplitPtrHead+2),*(SplitPtrHead+3),*(SplitPtrHead+4));
        SplitCount   = 0;
        
        PrintRule(SplitPtrHead);

        //SplitPtr++;
        SplitPtrHead = SplitPtr;
        SplitPtr++;
        SymbolListPtr++;
        SymbolListCount++;
   
        RulePtr +=2;
        match = 1;
        i++;
        break;
      }
    }

    i++;

    if(match == 1)
      continue;

    *SplitPtr = *RulePtr;

    SplitCount++;
    SplitPtr++;
    RulePtr++;
  }

  *SplitPtrHead  = SplitCount;
  //PrintRule(SplitPtrHead);

  **SymbolListRet = SymbolListCount;

  //printf("SymbolListCount = %d\n",SymbolListCount);

 // for(int i = 0; i< 50; i++)
 // {
 //   printf(" s=%d ",*(*SplitRet+i));
 // }
  //*SplitRet      = SplitPtrHead;
  //*SymbolListRet = SymbolListPtrHead;

  return IsBracketDetect;
}

void TradeRule()
{
  int i;
  int shares;

  //printf("aaaa = %f \n" ,CalRule(*(RootBuyAll)));
  for (i = 0; i < RuleBuyCount; i++)
  {
    if (CalRule(*(RootBuyAll+i)))
    {
      printf("Buy Pass!(%d)\n\n",i);
      //Buy
      shares = (*(RootBuyAll+i))->shares;
      Buy("Now",shares);
    }
  }

  for (i = 0; i < RuleSellCount; i++)
  {
    if (CalRule(*(RootSellAll+i)))
    {
      //Sell
      shares = (*(RootSellAll+i))->shares;
      Sell("Now",shares);
    }
  }

  for (i = 0; i < RuleBuyNextCount; i++)
  {
    if (CalRule(*(RootBuyNextAll+i)))
    {
      //BuyNext
      shares = (*(RootBuyNextAll+i))->shares;
      Buy("BuyNext",shares);
    }
  }
}

void AnalysisProfit (TRADE_RECORD *TradeRecords)
{
  float  MoneyIn, MoneyOut;
  int    BuyCount, SellCount, Counter;
  int    Shares,SharesRemaining;
  float  TotalIn,TotalOut,LastDayPrice,Price;
  int    SharesInTotal,SharesOutTotal;
  float  TotalRemainValues;
  float  AvgBuyPrice, AvgSellPrice;
  FILE   *fp;
  char   *Str;

  Str  = (char*) malloc(150);

  TotalIn   = 0;
  TotalOut  = 0;
  Counter   = 0;
  BuyCount  = 0;
  SellCount = 0;
  SharesOutTotal = 0;
  SharesInTotal  = 0;
  AvgBuyPrice    = 0;
  AvgSellPrice   = 0;
  
  Str = "Result";
  printf(",,,\n",Str);

  StrIDAppend(Str,StockId);

  fp = fopen(Str,"w");

  if (fp == NULL)
  {
    printf("FAILE create result file=====================\n");
  }

  //fputs("Start\n",fp);
  //FPUT("Start",fp)
  FputCL("Start",fp);

  WriteNum(StockId,fp)

  do{
    MoneyIn  = 0;
    MoneyOut = 0;
    Shares   = 0;
    
    Price    = TradeRecords->Price;
    Shares   = TradeRecords->ShareTrades;
    if(TradeRecords->BuyOrSell) // buy
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
    //WriteDates(TradeRecords->Dates.Years,TradeRecords->Dates.Months,TradeRecords->Dates.Days,str,fp)

    WriteDates(TradeRecords->Dates.Years,TradeRecords->Dates.Months,TradeRecords->Dates.Days,fp)

    printf("TradeRecords(%d)--%d/%d/%d--\n",Counter+1,TradeRecords->Dates.Years,TradeRecords->Dates.Months,TradeRecords->Dates.Days);
    printf("Action: ");
 
    if(TradeRecords->BuyOrSell){
      printf("In ,");
      FputCL("In.",fp);
      //FPUT("In",fp)
      //fprintf(fp,"In");
      //fputs("In",fp);
    } else{
      printf("Out ,");
      FputCL("Out.",fp);
      //FPUT("Out",fp)
      //FPUT("In",fp)
      //fprintf(fp,"Out");
      //fputs("Out",fp);
    }

    printf("Shares = %d, Price = %.1f, Total Price = %.1f, Shares Remaining = %d \n",Shares,Price,Shares*Price,TradeRecords->SharesRemaining);
    printf("===============================================\n");

    WriteNum(Shares,fp)
    WriteFloatNum(Price,fp)
    WriteNum(TradeRecords->SharesRemaining,fp)

    Counter += 1;
    if(TradeRecords->Next == NULL)
    {
      SharesRemaining = TradeRecords->SharesRemaining;
      LastDayPrice    = SimCurr->End;
    }
    TradeRecords = TradeRecords->Next;

  } while(TradeRecords != NULL);

  AvgBuyPrice  = AvgBuyPrice / (float)BuyCount;   
  AvgSellPrice = AvgSellPrice / (float)SellCount;
  TotalRemainValues = (TotalOut - TotalIn) + (LastDayPrice*SharesRemaining);

  //fputs("total\n",fp);
  //FPUT("total",fp)
  FputCL("total",fp);
  WriteFloatNum(LastDayPrice,fp)
  WriteNum(SharesRemaining,fp)
  WriteFloatNum(TotalIn,fp)
  WriteFloatNum(TotalOut,fp)
  WriteNum(BuyCount,fp)
  WriteNum(SellCount,fp)
  WriteFloatNum(((float)TotalRemainValues / (float) TotalIn )*100,fp)
  //fputs("end\n",fp);
  //FPUT("end",fp)
  FputCL("end",fp);

  printf("Last Day Price = %.1f, Shares Remaining = %d, Remaining Shares Values = %.1f\n",LastDayPrice, SharesRemaining, LastDayPrice*SharesRemaining);
  printf("Input Money= %.1f, Output Money = %.1f, Buy/Sell/Total Count = %d/%d/%d, Average Buy/Sell Price(per shares treade) = %1.f, %1.f\n", TotalIn, TotalOut ,BuyCount,SellCount,Counter,AvgBuyPrice,AvgSellPrice);
  printf("Total Remain (Out - In)+Remaining Shares Values = %.1f\n", TotalRemainValues );
  printf("Returns(Total Remain)/Input = %.1f%%\n", ((float)TotalRemainValues / (float) TotalIn )*100 );

  //free(Str);
  fclose(fp);
}

void StockSimulator(int length, TRADE_RECORD  **ReturnRecordsHead)
{
  int Current;

  Current   = 0;

  InitTechDataName();

  for(Current = 0; Current < length-StartDayIndex; Current++)
  {
    SimCurr = InfoBuffer + StartDayIndex + Current; /* Move day pointer to next day */
    //SimCurr = InfoBuffer + StartDayIndex + Current; /*move one day, for cross calculate*/
    printf("Day(%d) %d/%d/%d  \n",SimCurr->DayIndex,SimCurr->Dates.Years,SimCurr->Dates.Months,SimCurr->Dates.Days);
    TradeRule();
    printf("End of Day\n");
  }

  *ReturnRecordsHead = RecordHead;
}