#include "DataDefine.h"

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

DAILY_INFO *Sim3Curr;

void LoadRule();
void BuildTree(RuleNodes *Root);
void AddTree(RuleNodes *Root);
int SplitRule (int *SplitSymbol, int SymbolCount, int *Rule, int **SplitRet, int **SymbolListRet);
void PrintRule(int *Rule);
void CheckLastBracket(int *Rule);
float Price(int type,int day);
float MA_(int day);
float KD_(int *K, int *D, int day);
float RSI_(int day);
float MACD_ (int EMA1day, int EMA2day);

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


float Price(int type,int day)
{
  float Ret;
  return Ret;
}

float MA_(int day)
{
  float Ret;
  return Ret;
}

float KD_(int *K, int *D, int day)
{
  float Ret;
  return Ret;
}

float RSI_(int day)
{
  float Ret;
  return Ret;
}

float MACD_(int EMA1day, int EMA2day)
{
  float Ret;
  return Ret;
}

void TraceNode(RuleNodes *Root)
{
  int NumChild;
  int i;

  NumChild = Root->ChildCount;
  printf("=========\nRoot Address = %x\n",Root);
  printf("Child Num = %d\n",NumChild);
  printf("Level(%d)Rule->",Root->Levels);
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

  op = *Rule;

  if(op == Lowest_function || op == Highest_function  || op == Average_function  || op == Sum_function )
  {
    op_value  = *(Rule+1);
    op2       = *(Rule+2);
    op2_value = *(Rule+3);
  }

  if (op == Open_Op || op == High_Op || op == Low_Op || op == Close_Op)
  {
    Ret = Price(op, op_value);
  } else if (op == MA_Op)
  {
    Ret = MA(op_value);
  } else if (op == RSI_Op)
  {
    Ret = RSI(op_value);    
  } else if (op == KD_K_Op)
  {
    //Ret = KD(op_value);    
  } else if (op == KD_D_Op)
  {
    //Ret = KD(op_value);    
  }  else if (op == MACD_Op)
  {
    Ret = MACD(op, op_value);    
  } else if (op == Sum_function)
  {
    
  } else if (op == Lowest_function)
  {

  } else if (op == Highest_function)
  {
    
  } else if (op == Average_function)
  {
    
  } else if (op == Sum_function)
  {
    
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

  if (ChildC == 0)
    return GetVaule ( Root->Rule );

  value +=  CalRule(*(Root->Child));

  for (i= 1; i < ChildC ; i++)
  { 
    for (j = 0; j < ChildC; j++)
    {
      op = *(Root->SymbolList+j);
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
    }
  }

  return value;
}

void PrintRule(int *Rule)
{
  int len;
  int i;

  len = *Rule;
  printf("len = %d ",len);
  printf("rule: ");
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

void MainProcess2() 
{
  int *RuleBuyPtr;
  int *RuleSellPtr;
  int *RuleBuyNextPtr;
  int i,j,k;
  int len;

  RuleNodes **RootBuyAll;
  RuleNodes **RootSellAll;
  RuleNodes **RootBuyNextAll;

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
    printf("root level = %d  ",RootBuy->Levels );
 
    BuildTree(RootBuy);

    RuleBuyPtr += len+2;
    *RootBuyAllPtr = RootBuy;
    RootBuyAllPtr++;
    j++;
  }
printf("\n\n\n ");
  TraceNode(RootBuy);
printf("\n\n\n ");
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

    PrintRule(RuleSellPtr);
    BuildTree(RootSell);

    RuleSellPtr += len+2;
    //printf("RuleSellPtr += len+1 = %d %d %d %d %d\n",*RuleSellPtr,*(RuleSellPtr+1),*(RuleSellPtr+2),*(RuleSellPtr+3),*(RuleSellPtr+4));
    *RootSellAllPtr = RootSell;
    RootSellAllPtr++;
    j++;
    TraceNode(RootSell);
  }
printf("\n\n\n ");
  for(j = 0; j < RuleBuyCount; j++)
  {
   // TraceNode(*(RootSellAllPtr+i));
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
  //calloc(100,sizeof(int));
  //if (Root->Levels == 4)
  //  return;
  //printf("add tree build tree\n");
  AddTree(Root);

  ChildC = Root->ChildCount;
  //calloc(100,sizeof(int));
  // scan all the child
  for(i = 0; i < ChildC; i++)
  {
    printf("\nbuild child(%d) of root\n",i);
    ChildN = *(Root->Child + i);
    //if(ChildN->Levels != 4)
      BuildTree(ChildN);
    printf("build child(%d) done\n\n",i);
  }

printf("build child done alll\n");
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
  //a();
  ChildNum = 0;
  //printf("add tree1.1\n");
//calloc(100,sizeof(int));
  //SplitBuffer   = (int *) calloc(100,sizeof(int));
  //SymbolBuffer  = (int *) calloc(100,sizeof(int));
//printf("add tree2.1\n");
/*
  if (level == 1)
  {
    SplitRule(AndOr,sizeof(AndOr)/AndOr[0],Rule,&SplitBuffer,&SymbolBuffer);
  } else if (level == 2)
  {
    SplitRule(Condition,sizeof(Condition)/Condition[0],Rule,&SplitBuffer,&SymbolBuffer);
  } else if (level == 3)
  {
    SplitRule(Operater,sizeof(Operater)/Operater[0],Rule,&SplitBuffer,&SymbolBuffer);
  }*/

  //printf("add tree2\n");
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
    //printf("Root->Child = %x\n",Root->Child);
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
    printf("detect! before\n");
    PrintRule(Rhead);
    //remove first and last
    *Rhead = *Rhead - 4;
    for(i = 1; i <= len-1 ; i++)
    {
      *(Rhead+i) = *(Rhead+i+2);
    }
    printf("after\n");
    PrintRule(Rhead);
  }

}

void HandleFunctionItem(int *Rule)
{

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
    printf("RulePtr = %d\n",*RulePtr);
    if (*RulePtr == Left_Bracket && i%2 == 0 )  // ignore 1 level bracket and near symbol
    {
      printf("balance+");
      balance++;
    }

    if (*RulePtr == Right_Bracket && i%2 == 0 &&  balance != 0)
    {
      printf("balance-");
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
  PrintRule(SplitPtrHead);

  **SymbolListRet = SymbolListCount;

  printf("SymbolListCount = %d\n",SymbolListCount);

 // for(int i = 0; i< 50; i++)
 // {
 //   printf(" s=%d ",*(*SplitRet+i));
 // }
  //*SplitRet      = SplitPtrHead;
  //*SymbolListRet = SymbolListPtrHead;

  return IsBracketDetect;
}

/*
void OperationParsing(int *Rule, int **SplitRet, int **OperationRet, int len)
{
  // do '+' '-' first , split by + -
  int   *RulePtr;
  int   *mark;
  int i;

  i = 0;
  while(i < len)
  {

    if(*RulePtr == 25)
    {

    } else if(*RulePtr == 26)
    {

    }
    // mark and find i-1 i+1

    RulePtr++;
    i++;
  }
  
}

// For > < = crossup crsoodown
void ConditionParsing(int *Rule, int **SplitRet, int **ConditionRet, int len)
{
  int *RulePtr;
  int *Split;
  int *SplitPtr;
  int *ConditionList;
  int i;

  i = 0;
  RulePtr = Rule;
  SplitPtr     = (int *) malloc(100*sizeof(int));
  ConditionRet = (int *) malloc(100*sizeof(int));

  //Find  > < = crossup crsoodown
  RulePtr++; // Skip shares

  while(i < len)
  {
    if ( (*RulePtr >= 20 && *RulePtr <= 24) || *RulePtr == 29 || *RulePtr == 30 )//  > < = crossup crsoodown
    {
      if(*RulePtr == 20)
        *ConditionList = 1;
      else if(*RulePtr == 21)
        *ConditionList = 2;
      else if(*RulePtr == 22)
        *ConditionList = 3;        
      else if(*RulePtr == 23)
        *ConditionList = 4;        
      else if(*RulePtr == 24)
        *ConditionList = 5;        
      else if(*RulePtr == 29)
        *ConditionList = 6;        
      else if(*RulePtr == 30)
        *ConditionList = 7;        

      ConditionList++;
      SplitPtr++;
      *SplitPtr = -99;
    }

    *SplitPtr = *RulePtr;

    RulePtr++;
    SplitPtr++;
    i++;
  }

  SplitPtr++;
  *SplitPtr = -99;

  *SplitRet = SplitPtr;
  *ConditionRet = ConditionList;
}

void AndOrParsing(int *Rule, int **SplitRet, int **AndOrRet, int len)
{
  // do first with bracket 

  // split [statement] and [statement]
  // save each statement to array

  // After finish satement in bracket
  // then do it all again
  int *RulePtr;
  int *Split;
  int *SplitPtr;
  int *AndOrList;
  int i;

  Split    = (int *) malloc(100*sizeof(int));
  AndOrRet = (int *) malloc(100*sizeof(int));

  i = 0;
  RulePtr = Rule;
  SplitPtr = Split;

  //Find And Or
  RulePtr++; // Skip shares

  while(i < len)
  {
    if(*RulePtr == 33 || *RulePtr == 34) // and or
    {
      if(*RulePtr == 33)
        *AndOrList = 1;
      else
        *AndOrList = 0;

      AndOrList++;
      SplitPtr++;
      *SplitPtr = -99;
    }

    *SplitPtr = *RulePtr;

    RulePtr++;
    SplitPtr++;
    i++;
  }

  SplitPtr++;
  *SplitPtr = -99;

  *SplitRet = SplitPtr;
  *AndOrRet = AndOrList;
}
*/

void TradeRule2()
{

}

void StockSimulator3(int StartDayIndex, int EndDayIndex, TRADE_RECORD2  **ReturnRecords2Head)
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