import os
from matplotlib import pyplot as plt
import copy
from FetchData import FetchHistoryData

PlotDataX = []
PlotDataYH = []
PlotDataYL = []
PlotDataYO = []
PlotDataYC = []

Result_X      = []
Result_Y      = []
Result_Action = []
Result_Share  = []
Result_Remain = []

TotalDay = 0

def GetData(stock_id,InputStart,InputEnd):

    YearStart  = int(InputStart[0:4])
    MonthStart = int(InputStart[4:6])

    YearEnd    = int(InputEnd[0:4])
    MonthEnd   = int(InputEnd[4:6])

    f = open("HistoryData/History"+stock_id+"_", mode='r')    

    i = 0
    count = 0

    for Da in f.readline():
        if Da == "end":
            print("end")
            continue
        if i % 11 == 0:
            Y = Da
        if i % 11 == 1:
            M = Da
        if i % 11 == 2:
            D = Da
            if ( YearStart < int(Y) and YearEnd > int(Y) ) or ( YearStart == int(Y) and MonthStart <= int(M) ) or ( YearEnd == int(Y) and MonthEnd >= int(M) ) :
                PlotDataX.append(Y+"/"+M+"/"+D)
                InRange = 1
                count += 1
            else:
                InRange = 0
        if i % 11 == 3 and InRange == 1:
            PlotDataYO.append(Da)
        if i % 11 == 4 and InRange == 1:
            PlotDataYC.append(Da)
        if i % 11 == 5 and InRange == 1:
            PlotDataYH.append(Da)
        if i % 11 == 6 and InRange == 1:
            PlotDataYL.append(Da)
        i += 1

    TotalDay = count

def SplitYearResult():

    global Result_X
    global Result_Y
    global Result_Action
    global Result_Share
    global Result_Remain

    #split by year
    firstdata = 1    
    YearNow   = 2000
    ResultInYear = []
    R_X  = []  # result X
    R_Y  = []  # result Y
    R_A  = []  # result Action
    R_S  = []  # result Shares
    R_R  = []  # result Remain

    for spli1,spli2,spli3,spli4,spli5 in zip(Result_X,Result_Y,Result_Action,Result_Share,Result_Remain):
        spli_year = spli1.split("/")
        if firstdata == 1:    
            YearNow = spli_year[0]
            firstdata = 0
        if YearNow != spli_year[0]:
            #print("year changed")
            ResultInYear.append(copy.deepcopy(R_X))
            ResultInYear.append(copy.deepcopy(R_Y))
            ResultInYear.append(copy.deepcopy(R_A))
            ResultInYear.append(copy.deepcopy(R_S))
            ResultInYear.append(copy.deepcopy(R_R))
            R_X.clear()
            R_Y.clear()
            R_A.clear()
            R_S.clear()
            R_R.clear()

        YearNow = spli_year[0]
        R_X.append(spli1)
        R_Y.append(spli2)
        R_A.append(spli3)
        R_S.append(spli4)
        R_R.append(spli5)

    ResultInYear.append(copy.deepcopy(R_X))
    ResultInYear.append(copy.deepcopy(R_Y))
    ResultInYear.append(copy.deepcopy(R_A))
    ResultInYear.append(copy.deepcopy(R_S))
    ResultInYear.append(copy.deepcopy(R_R))

    return ResultInYear

def SplitYearData():

    global PlotDataX
    global PlotDataYO
    global PlotDataYC
    global PlotDataYH
    global PlotDataYL

    #split by year
    firstdata = 1
    YearNow   = 2000
    DataInYear = []
    PD_X = []  # PlotDataX in years
    PD_O = []  # PlotDataY open price in years
    PD_C = []  # PlotDataY close price in years
    PD_H = []  # PlotDataY high price in years
    PD_L = []  # PlotDataY low price in years

    for spli1,spli2,spli3,spli4,spli5 in zip(PlotDataX,PlotDataYO,PlotDataYC,PlotDataYH,PlotDataYL):
        spli_year = spli1.split("/")
        #print("SY=",spli_year)
        if firstdata == 1:
            YearNow = spli_year[0]
            firstdata = 0
        if YearNow != spli_year[0]:
            #print("year changed")
            DataInYear.append(copy.deepcopy(PD_X))
            DataInYear.append(copy.deepcopy(PD_O))
            DataInYear.append(copy.deepcopy(PD_C))
            DataInYear.append(copy.deepcopy(PD_H))
            DataInYear.append(copy.deepcopy(PD_L))
            PD_X.clear()
            PD_O.clear()
            PD_C.clear()
            PD_H.clear()
            PD_L.clear()

        YearNow = spli_year[0]
        PD_X.append(spli1)
        PD_O.append(spli2)
        PD_C.append(spli3)
        PD_H.append(spli4)
        PD_L.append(spli5)

    DataInYear.append(copy.deepcopy(PD_X))
    DataInYear.append(copy.deepcopy(PD_O))
    DataInYear.append(copy.deepcopy(PD_C))
    DataInYear.append(copy.deepcopy(PD_H))
    DataInYear.append(copy.deepcopy(PD_L))

    return DataInYear

def ReadResult(ID):

    global Result_X
    global Result_Y
    global Result_Action
    global Result_Share
    global Result_Remain

    Result_X.clear()
    Result_Y.clear()
    Result_Action.clear()
    Result_Share.clear()
    Result_Remain.clear()

    f = open("Result"+ID, mode='r')

    SResult    = []

    ## Reading result
    for result in f.readlines():
        SResult.append( result.replace("\n","") )

    Check_Sim_exist = 0
    Counter = 0
    #print("======================")
    for ana in SResult:
        #print("ana = ",ana,"c = ",Counter)
        if ana == "Start":
            Check_Sim_exist = 1
            #print("check S")
        if ana == "total":
            break
        if Counter > 1 and Check_Sim_exist == 1 and ana != "end":
            #print("check E")
            if Counter % 5 == 2:
                Result_X.append(ana)
            if Counter % 5 == 3:
                Result_Action.append(ana)
            if Counter % 5 == 4:
                Result_Share.append(ana)
            if Counter % 5 == 0:
                print(ana)
                Result_Y.append(float(ana))
            if Counter % 5 == 1:
                Result_Remain.append(ana)
        Counter += 1


def GenResult(PlotByYear,ID):

    ReadResult(ID)
    ResultInYear = SplitYearResult()

    #for a,b,c in zip(Result_X,Result_Y,Result_Action):
    #    print("a,b,c",a,"--",b,"--",c)

    # Prepare data for K bar
    # -------------(PlotDataYH)--> (high)
    #  BarDiffT_H     #上影線
    # -------------(Topp)--> higher(open,close)
    #   BarDiff       #K棒
    # -------------(Butt)--> lower(open,close)
    #  BarDiffL_B     #下影線
    # --------------(PlotDataYL)--> (low)
    #     void        #小於low空白
    # --------------
    #os.system("CD Result")
    os.system("mkdir Result\\"+ID)
    #os.system("CD ..")    
    ind  = 0
    ind2 = 0

    for YD in PlotByYear:
        if ind % 5 == 0:
            PlotDataX = copy.deepcopy(YD)
        if ind % 5 == 1:
            PlotDataYC = copy.deepcopy(YD)
        if ind % 5 == 2:
            PlotDataYO = copy.deepcopy(YD)
        if ind % 5 == 3:
            PlotDataYH = copy.deepcopy(YD)
        if ind % 5 == 4:    
            PlotDataYL = copy.deepcopy(YD)
        #print("ind",ind)
        if ind % 5 != 4:  #If ind % 5 = 4, Gen Image result data
            ind += 1
            continue
        ind += 1
        
        Years = PlotDataX[0].split("/")

        #Get result in years
        ind2 = 0
        for YR in ResultInYear:
            #print("ind2 = ",ind2)
            #print("ResultInYear = ",ResultInYear)
            if ind2 % 5 == 0:
                Result_X = copy.deepcopy(YR)
            if ind2 % 5 == 1:
                Result_Y = copy.deepcopy(YR)
            if ind2 % 5 == 2:
                Result_Action = copy.deepcopy(YR)
            if ind2 % 5 == 3:
                Result_Share = copy.deepcopy(YR)
            if ind2 % 5 == 4:    
                Result_Remain = copy.deepcopy(YR)
            if ind2 % 5 != 4:  #If ind % 5 = 4, Gen Image result data
                ind2 += 1
                continue

            Years2 = Result_X[0].split("/")
            if Years[0] == Years2[0]: # data and result match
                #print("year match")
                break
            ind2 += 1
            Result_X.clear()
            Result_Y.clear()
            Result_Action.clear()
            Result_Share.clear()
            Result_Remain.clear()  

        #print("gen")

        #print("YY",Years[0])

        Butt       = []  # lower(open,close)
        Topp       = []  # higher(open,close)
        BarDiff    = []  # butt to top
        BarDiffL_B = []  # low to butt
        BarDiffT_H = []  # top to high
        colors     = []  # colors of K bar

        for p1,p2,p3,p4 in zip(PlotDataYC,PlotDataYO,PlotDataYH,PlotDataYL):
            #print("O=",p2,"C=",p1)
            if p1 > p2:
                Butt.append(p2)
                Topp.append(p1)
                BarDiff.append(p1-p2)
                BarDiffL_B.append(p2-p4)
                BarDiffT_H.append(p3-p1)
                colors.append("green")
            elif p1 < p2:
                Butt.append(p1)
                Topp.append(p2)
                BarDiff.append(p2-p1)
                BarDiffL_B.append(p1-p4)
                BarDiffT_H.append(p3-p2)
                colors.append("red")
            else:
                Topp.append(p1)
                Butt.append(p2)
                BarDiff.append(0.06)
                BarDiffL_B.append(p1-p4)
                BarDiffT_H.append(p3-p2)
                colors.append("black")
    
        MaxV = max(PlotDataYH) #Max value
        MinV = min(PlotDataYL) #Min value
        LenD = len(PlotDataX)  #Len of x axis
        RatioX = LenD/245
        RatioY = (MaxV - MinV)/150 #Y ratio
        DpiR   = 360*RatioX*RatioY

        print("RX = ",RatioX,"RY = ",RatioY,"DPI = ",DpiR)
        Years = PlotDataX[0].split("/")
        fig = plt.figure(figsize=(96,20),dpi=330)
        #plt.grid(True, axis='y')
        plt.grid(True,zorder=0)
        plt.title("Stock ID:"+ID+"    Years:"+Years[0],fontsize=80)
        plt.xlabel("Date",fontsize=40) 
        plt.ylabel("Price",fontsize=40)
        plt.bar(PlotDataX,BarDiffL_B,bottom=PlotDataYL,color=colors,width=0.06*RatioX,zorder=100)
        plt.bar(PlotDataX,BarDiffT_H,bottom=Topp,color=colors,width=0.06*RatioX,zorder=100)
        plt.bar(PlotDataX,BarDiff,bottom=Butt,color=colors,width=0.6*RatioX,zorder=100)
        plt.xticks(rotation=90,fontsize=20)
        plt.yticks(fontsize=20)

        plt.plot(Result_X,Result_Y)
        for a,b,c in zip(Result_X,Result_Y,Result_Action):
            plt.text( a, b+10*RatioY , c, ha='center', va= 'bottom',fontsize=20,zorder=100)
        #plt.show()
        for a,b,c,d,e in zip(PlotDataX,PlotDataYH,Topp,Butt,PlotDataYL):
            plt.text(a, b+1.3*RatioY, '%.1f' %b, ha='center', va= 'bottom',fontsize=2,zorder=100)
            plt.text(a, b+0.9*RatioY, '%.1f' %c, ha='center', va= 'bottom',fontsize=2,zorder=100)
            plt.text(a, b+0.5*RatioY, '%.1f' %d, ha='center', va= 'bottom',fontsize=2,zorder=100)
            plt.text(a, b+0.1*RatioY, '%.1f' %e, ha='center', va= 'bottom',fontsize=2,zorder=100)
    
        fig.set_figheight(20)
        fig.set_figwidth(96)
        PIC_NAME = "K_Bar"+ID+"_"+Years[0]+".png"
        fig.savefig(PIC_NAME)
        os.system("move "+PIC_NAME+" Result/"+ID+"/")
        #os.system("mkdir Result"+ID)
        #os.system("move K_Bar"+ID+"_"+Years[0]+".png \Result"+ID)
        plt.cla()
        plt.clf()
        plt.close(fig)

        Result_X.clear()
        Result_Y.clear()
        Result_Action.clear()
        Result_Share.clear()
        Result_Remain.clear()    

        Butt.clear()
        Topp.clear()
        BarDiff.clear()
        BarDiffL_B.clear()
        BarDiffT_H.clear()
        colors.clear()

f = open("IDList.txt", mode='r')
IDList = []
while 1:
    ID = f.read(4)
    IDList.append(ID)
    ID = f.read(1)
    if ID != ",":
        break
f.close()


#stock_id = input ("請輸入股票代碼: ")

InputStart = input ("請輸入起始年月 (ex:201911): ")
InputEnd = input ("請輸入結束年月(ex:202101): ")
os.system("rmdir Result /s /q")
os.system("mkdir Result")

for ID_LIST in IDList:
    print("ID:",ID_LIST)
    os.system("del Result"+ID_LIST)

for ID_LIST in IDList:
    GetData(ID_LIST,InputStart,InputEnd)
    os.system("StockEmulator.exe ""data"+ID_LIST+".xml "+str(TotalDay-61))
    print("Total Days = ",TotalDay)

    PlotByYear = SplitYearData()
    GenResult(PlotByYear,ID_LIST)
    PlotDataX.clear()
    PlotDataYO.clear()
    PlotDataYC.clear()
    PlotDataYH.clear()
    PlotDataYL.clear()


FetchHistoryData("2330","2019","2020")

#FetchHistoryData("2330","2018","2020")

#for AllX,AllY in PlotDataX,PlotDataY:
#    plt.plot(AllX,AllY)
#    plt.show()
#plt.plot(PlotDataX,PlotDataY,":")
#plt.show()


#f.close()
