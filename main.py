import requests
import os
import xml.etree.cElementTree as ET
import lxml.etree as etree
from itertools import zip_longest
from   bs4      import BeautifulSoup
import time
import random
import numpy as np
from matplotlib import pyplot as plt
import copy

PlotDataX = []
PlotDataYH = []
PlotDataYL = []
PlotDataYO = []
PlotDataYC = []

#headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

user_agent_list = ["Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36",
                    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36",
                    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
                    "Mozilla/5.0 (Macintosh; U; PPC Mac OS X 10.5; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15",
                   ]

#StockData = ET.Element('StockData',Version="1.0") # global Stock data

def Get_Data(stock_id,year,month,IsTWSE,StockData):

    yeart = str(int(year)-1911)

    if IsTWSE == 1:
        url1 = "https://www.twse.com.tw/exchangeReport/STOCK_DAY?response=html&date="+year+month+"01&stockNo="+stock_id                              # 股價 (上市)
    else:
        url1 = "https://www.tpex.org.tw/web/stock/aftertrading/daily_trading_info/st43_print.php?l=zh-tw&d="+yeart+"%2F"+month+"&stkno="+stock_id    # 股價 (上櫃)

    url2 = "https://stock.wearn.com/netbuy.asp?Year="+yeart+"&month="+month+"&kind="+stock_id  # 三大法人
    url3 = "https://stock.wearn.com/zhuli.asp?Year="+yeart+"&month="+month+"&kind="+stock_id   # 主力進出

    headers={
    "User-Agent":random.choice(user_agent_list)
    }

    res = requests.get(url1, headers = headers)
    res.encoding = 'cp950'

    UrlArr = [url1,url2,url3]
    Soup   = ['','','']
    Tables = ['','','']
    tab    = ['','','']

    for i in range(3):  # Dump all 3 urls data
        print(UrlArr[i])
        res = requests.get (UrlArr[i])
        res.encoding = 'cp950'
        Soup[i] = BeautifulSoup (res.text, "html.parser")
        #print(Soup[i].title)
        Tables[i] = Soup[i].findAll('table')
        tab[i]    = Tables[i][0]

    if IsTWSE == 1:
        tab1 = tab[0].findAll('tr')
        tab1 = tab1[2:]
    else:
        tab1 = tab[0].findAll('tr')
        tab1 = tab1[2:len(tab1)-1]

    #A special case for 2018 10  (2317)

    tab2 = tab[1].findAll('tr')
    tab2 = tab2[2:]
    tab2.reverse()

    tab3 = tab[2].findAll('tr')
    tab3 = tab3[2:]
    tab3.reverse()

    i = 1 # Daily index
    j = 0 # tab[0] td index2
    k = 0 # tab[1] td index
    l = 0 # tab[2] td index
    length = len(tab1)
    length2 = len(tab2)

    # 先嘗試修改函式屬性的值，如果出現AttributeError
    # 表示該屬性不存在，這時候就建立該屬性
    try:
        Get_Data.count += length   # 加函式屬性的值(static variable)
    except AttributeError: 
        Get_Data.count += length   # 建立函式的屬性(static variable)

    count1 = Get_Data.count - length   # For daily index 屬性累加     
    print("length=",length,"length2 = ",length2)
    #global StockData
    global PlotDataX
    global PlotDataY
    for tr1 in tab1:
        Daily = ET.Element('Daily')    # 新增 daily element
        StockData.append(Daily)
        Daily.set('Index',str(count1+i))
        Price = ET.SubElement(Daily, 'Price')
        ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0") # Will uptade in tab[1] and tab[2]
        for td in tr1.findAll('td'):  # 股價
            if j % 9 == 0:  # 日期
                Daily.set('Years',str(int(td.getText()[0:3])+1911))
                Daily.set('Months',td.getText()[4:6])
                Daily.set('Days',td.getText()[7:9])
                Y = str(int(td.getText()[0:3])+1911)
                M = td.getText()[4:6]
                D = td.getText()[7:9]
                PlotDataX.append(Y+"/"+M+"/"+D)
            if j % 9 == 3:  # 開盤
                Price.set('Start',(td.getText().rstrip()).replace('.',"") ) 
                value = (td.getText().rstrip()).replace('.',"")
                PlotDataYO.append(int(value)/100)
            if j % 9 == 4:  # 最高
                Price.set('High',(td.getText().rstrip()).replace('.',"") )
                value = (td.getText().rstrip()).replace('.',"")
                PlotDataYH.append(int(value)/100)
            if j % 9 == 5:  # 最低
                Price.set('Low',(td.getText().rstrip()).replace('.',"") )
                value = (td.getText().rstrip()).replace('.',"")
                PlotDataYL.append(int(value)/100)
            if j % 9 == 6:  # 收盤             
                Price.set('End',(td.getText().rstrip()).replace('.',"") )
                value = (td.getText().rstrip()).replace('.',"")
                PlotDataYC.append(int(value)/100)
            j += 1
        i += 1

    DiffDay1 = []
    DiffDay2 = []
    DiffDataI = []
    DiffDataD = []
    DiffDataF = []
    DiffDataL = []

    for tr2 in tab2:  #catch data
        for td in tr2.findAll('td'):  # 三大法人
            if k % 4 == 0:
                DiffDay1.append(td.getText())
            if k % 4 == 1:  # 投信
                 #Difference.set('InvestmentTrustDiff',(td.getText().rstrip()).replace(',',"") )
                DiffDataI.append((td.getText().rstrip()).replace(',',""))
            if k % 4 == 2:  # 自營商
                DiffDataD.append((td.getText().rstrip()).replace(',',""))
            if k % 4 == 3:  # 外資
                DiffDataF.append((td.getText().rstrip()).replace(',',""))
            k += 1
    for tr3 in tab3:  #catch data
        for td in tr3.findAll('td'): # 主力進出
            if l % 3 == 0:
                DiffDay2.append(td.getText())            
            if l % 3 == 2:  # 主力增減
                DiffDataL.append((td.getText().rstrip()).replace(',',""))
            l += 1

    # update to xml
    count2 = 0
    count3 = 0
    for DATA1 in StockData:
        if count2 >= count1:
            count3 = 0
            for DIF in DATA1:
                if count3 == 1: # Find diff Update
                    for DiffInd1,DiffInd2,DiffInd3,DiffInd4  in zip(DiffDay1,DiffDataI,DiffDataD,DiffDataF):
                        if DiffInd1 == str(int(DATA1.get('Years'))-1911)+"/"+DATA1.get('Months')+"/"+DATA1.get('Days'):  #if day match
                            DIF.set('InvestmentTrustDiff',DiffInd2)
                            DIF.set('DealersDiff',DiffInd3)
                            DIF.set('ForeignInvestorsDiff',DiffInd4)                             
                count3 += 1
        count2 += 1

    # update to xml
    count2 = 0
    count3 = 0
    for DATA1 in StockData:
        if count2 >= count1:
            count3 = 0
            for DIF in DATA1:
                if count3 == 1: # Find diff Update
                    for DiffInd1,DiffInd2  in zip(DiffDay2,DiffDataL):
                        if DiffInd1 == str(int(DATA1.get('Years'))-1911)+"/"+DATA1.get('Months')+"/"+DATA1.get('Days'):  #if day match
                            DIF.set('LeaderDiff',DiffInd2)
                count3 += 1
        count2 += 1



def Is_TWSE_Listed(stock_id):
    url = "https://www.twse.com.tw/exchangeReport/STOCK_DAY?response=html&date=20200101&stockNo="+stock_id # Test 上市或上櫃
    #global user_agent_list
    #headers['User-Agent'] = random.choice(user_agent_list)
    headers={
    "User-Agent":random.choice(user_agent_list)
    }
    res = requests.get(url, headers = headers)
    res.encoding = 'cp950'
    Soup = BeautifulSoup (res.text, "html.parser")
    Tables = Soup.findAll('table')
    if len(Tables) == 0:
        return 0            # 上櫃
    else:
        return 1            # 上市

def GetDataByID(stock_id,InputStart,InputEnd):

    YearStart  = int(InputStart[0:4])
    MonthStart = int(InputStart[4:6])

    YearEnd    = int(InputEnd[0:4])
    MonthEnd   = int(InputEnd[4:6])

    YearDiff   = YearEnd - YearStart

    if YearDiff == 0:
        MonthDiff = MonthEnd - MonthStart
    elif YearDiff > 0:
        MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
    else:
        BaseException: Error

    StockData = ET.Element('StockData',Version="1.0") # global Stock data
    StockId = ET.SubElement(StockData, 'StockId')
    StockId.text = stock_id
    IsTWSE = Is_TWSE_Listed(stock_id)

    Get_Data.count = 0

    if YearDiff == 0:
        MonthDiff = MonthEnd - MonthStart
        for monthI in range(MonthDiff+1):
            YearData  = str(YearStart)
            MonthData = str(MonthStart+monthI).rjust(2,'0')
            print('Y/M',YearData,MonthData)
            time.sleep(6)
            Get_Data(stock_id, YearData, MonthData, IsTWSE,StockData)        
    elif YearDiff > 0:
        MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
        yearI = 0
        for monthI in range(MonthDiff+1):
            YearData  = str(YearStart+yearI)
            MonthData = str(((MonthStart+monthI)%12)).rjust(2,'0')
            if MonthData == '00':
                MonthData = '12'
                yearI += 1
            print('Y/M',YearData,MonthData)
            time.sleep(6)
            Get_Data(stock_id, YearData, MonthData, IsTWSE,StockData)
    else:
        BaseException: Error
    '''
    global PlotDataAllX
    global PlotDataAllY
    global PlotDataX
    global PlotDataY

    PlotDataAllX.append(PlotDataX)
    PlotDataAllY.append(PlotDataY)
    PlotDataX = []
    PlotDataY = []
    '''
    print("total count = ",Get_Data.count)
        # 建立 XML 檔案
    tree = ET.ElementTree(StockData)
    tree.write("data"+stock_id+".xml",xml_declaration=True,encoding='UTF-8',method="xml")

        # 讀取 XML 檔案
    root = etree.parse("data"+stock_id+".xml")

        # 輸出與排版 XML 資料
    #print(etree.tostring(root, pretty_print=True, encoding="unicode"))

        # 將排版的 XML 資料寫入檔案
    root.write("data"+stock_id+".xml", encoding="utf-8")

def SplitYearData():
    #split by year
    firstdata = 1
    YearNow   = 2000
    DataInYear = []
    PD_X = []  # PlotDataX in years
    PD_O = []  # PlotDataY open price in years
    PD_C = []  # PlotDataY close price in years
    PD_H = []  # PlotDataY high price in years
    PD_L = []  # PlotDataY low price in years
    global PlotDataX
    global PlotDataYO
    global PlotDataYC
    global PlotDataYH
    global PlotDataYL

    for spli1,spli2,spli3,spli4,spli5 in zip(PlotDataX,PlotDataYO,PlotDataYC,PlotDataYH,PlotDataYL):
        spli_year = spli1.split("/")
        #print("SY=",spli_year)
        if firstdata == 1:
            YearNow = spli_year[0]
            firstdata = 0
        if YearNow != spli_year[0]:
            print("year changed")
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

def GenResult(PlotByYear,ID):
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
    ind = 0

    for YD in PlotByYear:
        if ind % 5 == 0:
            PlotDataX = YD
        if ind % 5 == 1:
            PlotDataYC = YD
        if ind % 5 == 2:
            PlotDataYO = YD
        if ind % 5 == 3:
            PlotDataYH = YD
        if ind % 5 == 4:    
            PlotDataYL = YD
        #print("ind",ind)
        if ind % 5 != 4:  #If ind % 5 = 4, Gen Image result data
            ind += 1
            continue
        ind += 1
        #print("gen")
        Years = PlotDataX[0].split("/")
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
    GetDataByID(ID_LIST,InputStart,InputEnd)
    os.system("StockEmulator.exe ""data"+ID_LIST+".xml "+str(Get_Data.count-60))
    print("Total Days = ",Get_Data.count)

    PlotByYear = SplitYearData()
    GenResult(PlotByYear,ID_LIST)
    PlotDataX.clear()
    PlotDataYO.clear()
    PlotDataYC.clear()
    PlotDataYH.clear()
    PlotDataYL.clear()

'''
f = open("Result"+ID_LIST, mode='r')

SResultAll = []
SResult    = []

## Reading result
for result in f.readlines():
    if result == "end\n":
        SResultAll.append(SResult)
        SResult    = []
        continue
    SResult.append( result.replace("\n","") )

print("======================")
for ana1 in SResultAll:
    for ana2 in ana1:
        print(ana2)
    print("======================")
'''
#for AllX,AllY in PlotDataX,PlotDataY:
#    plt.plot(AllX,AllY)
#    plt.show()
#plt.plot(PlotDataX,PlotDataY,":")
#plt.show()


#f.close()
'''
YearStart  = int(InputStart[0:4])
MonthStart = int(InputStart[4:6])

YearEnd    = int(InputEnd[0:4])
MonthEnd   = int(InputEnd[4:6])

YearDiff   = YearEnd - YearStart

if YearDiff == 0:
    MonthDiff = MonthEnd - MonthStart
elif YearDiff > 0:
    MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
else:
    BaseException: Error

# Init XML struct
StockId = ET.SubElement(StockData, 'StockId')
StockId.text = stock_id

IsTWSE = Is_TWSE_Listed(stock_id)

if YearDiff == 0:
    MonthDiff = MonthEnd - MonthStart
    for monthI in range(MonthDiff+1):
        YearData  = str(YearStart)
        MonthData = str(MonthStart+monthI).rjust(2,'0')
        print('Y/M',YearData,MonthData)
        time.sleep(6)
        Get_Data(stock_id, YearData, MonthData, IsTWSE)        
elif YearDiff > 0:
    MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
    yearI = 0
    for monthI in range(MonthDiff+1):
        YearData  = str(YearStart+yearI)
        MonthData = str(((MonthStart+monthI)%12)).rjust(2,'0')
        if MonthData == '00':
            MonthData = '12'
            yearI += 1
        print('Y/M',YearData,MonthData)
        time.sleep(6)
        Get_Data(stock_id, YearData, MonthData, IsTWSE)
else:
    BaseException: Error
'''
''' debug
IsTWSE = Is_TWSE_Listed(stock_id)
Get_Data(stock_id,'2020','01',IsTWSE)
Get_Data(stock_id,'2020','02',IsTWSE)
Get_Data(stock_id,'2020','03',IsTWSE)
Get_Data(stock_id,'2020','04',IsTWSE)
Get_Data(stock_id,'2020','05',IsTWSE)

    # 建立 XML 檔案
tree = ET.ElementTree(StockData)
tree.write("data"+stock_id+".xml",xml_declaration=True,encoding='UTF-8',method="xml")

    # 讀取 XML 檔案
root = etree.parse("data"+stock_id+".xml")

    # 輸出與排版 XML 資料
print(etree.tostring(root, pretty_print=True, encoding="unicode"))

    # 將排版的 XML 資料寫入檔案
root.write("data"+stock_id+".xml", encoding="utf-8")
'''
#ET.dump(StockData)

#os.remove('test.xml')