import requests
import os
from   bs4      import BeautifulSoup
import time
import random

TotalY = 0

#headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

user_agent_list = ["Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36",
                    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36",
                    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
                    "Mozilla/5.0 (Macintosh; U; PPC Mac OS X 10.5; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15",
                   ]


def Get_Data(f,stock_id,year,month,IsTWSE):

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

    Dates  = []
    Price_O = []
    Price_C = []
    Price_H = []
    Price_L = []
    Dates2  = []
    DIff_I  = []
    Diff_F  = []
    Diff_D  = []
    Dates3  = []
    Diff_L  = []

    for tr1 in tab1:
        for td in tr1.findAll('td'):  # 股價
            if j % 9 == 0:  # 日期
                Y = str(int(td.getText()[0:3])+1911)
                M = td.getText()[4:6]
                D = td.getText()[7:9]
                Dates.append(Y+"/"+M+"/"+D)
            if j % 9 == 3:  # 開盤
                Price_O.append( (td.getText().rstrip()).replace('.',"") )
            if j % 9 == 4:  # 最高
                Price_H.append( (td.getText().rstrip()).replace('.',"") )
            if j % 9 == 5:  # 最低
                Price_L.append( (td.getText().rstrip()).replace('.',"") )
            if j % 9 == 6:  # 收盤             
                Price_C.append( (td.getText().rstrip()).replace('.',"") )
            j += 1
        i += 1

    for tr2 in tab2:  #catch data
        for td in tr2.findAll('td'):  # 三大法人
            if k % 4 == 0:
                Dates2.append(td.getText())
            if k % 4 == 1:  # 投信
                DIff_I.append( (td.getText().rstrip()).replace(',',"") )
            if k % 4 == 2:  # 自營商
                Diff_D.append( (td.getText().rstrip()).replace(',',"") )
            if k % 4 == 3:  # 外資
                Diff_F.append( (td.getText().rstrip()).replace(',',"") )
            k += 1
    for tr3 in tab3:  #catch data
        for td in tr3.findAll('td'): # 主力進出
            if l % 3 == 0:
                Dates3.append(td.getText())            
            if l % 3 == 2:  # 主力增減
                Diff_L.append( (td.getText().rstrip()).replace(',',"") )
            l += 1

    # Write to file
    Mtach1 = 0
    Match2 = 0
    for Da1,O,C,H,L in zip(Dates,Price_O,Price_C,Price_H,Price_L):
        Mtach1 = 0
        Match2 = 0
        Y = Da1[0:4]
        M = Da1[5:7]
        D = Da1[8:10]
        f.write(Y+'\n'+M+'\n'+D+'\n'+O+'\n'+C+'\n'+H+'\n'+L+'\n')
        '''
        f.write(M+'\n')
        f.write(D+'\n')
        f.write(O+'\n')
        f.write(C+'\n')
        f.write(H+'\n')
        f.write(L+'\n')
        '''
        for Da2,DI1,DI2,DI3  in zip(Dates2,DIff_I,Diff_D,Diff_F):
            Da2W = str(int(Da2[0:3])+1911)+Da2[3:]
            if Da2W == Da1:  
            #if day match, if not match = 0
                f.write(DI1+'\n'+DI2+'\n'+DI3+'\n')
                #print("DD= ",DI1)
                '''
                f.write(DI1+'\n')
                f.write(DI2+'\n')
                f.write(DI3+'\n')
                '''
                Mtach1 = 1
        if Mtach1 == 0:
            f.write("0\n0\n0\n")
            '''
            f.write("0\n")
            f.write("0\n")
            f.write("0\n")
            '''
        for Da3,DI4 in zip(Dates3,Diff_L):
            Da3W = str(int(Da3[0:3])+1911)+Da3[3:]
            if Da3W == Da1:  
            #if day match, if not match = 0
                f.write(DI4+"\n")
                Match2 = 1
        if Match2 == 0:
            f.write("0\n")

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

    os.system("mkdir HistoryData")    

    YearStart  = int(InputStart[0:4])
    MonthStart = int(InputStart[4:6])

    YearEnd    = int(InputEnd[0:4])
    MonthEnd   = int(InputEnd[4:6])

    YearDiff   = YearEnd - YearStart

    FName = "History"+stock_id+"_"+InputStart+"_"+InputEnd
    os.system("del HistoryData/"+FName)
    f = open(FName, 'a')

    if YearDiff == 0:
        MonthDiff = MonthEnd - MonthStart
    elif YearDiff > 0:
        MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
    else:
        return #BaseException: Error

    IsTWSE = Is_TWSE_Listed(stock_id)

    Get_Data.count = 0

    if YearDiff == 0:
        MonthDiff = MonthEnd - MonthStart
        for monthI in range(MonthDiff+1):
            YearData  = str(YearStart)
            MonthData = str(MonthStart+monthI).rjust(2,'0')
            print('Y/M',YearData,MonthData)
            time.sleep(6)
            Get_Data(f,stock_id, YearData, MonthData, IsTWSE)        
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
            Get_Data(f,stock_id, YearData, MonthData, IsTWSE)
    else:
        return #BaseException: Error

    f.write("end\n")
    f.close()
    os.system("move "+FName+" HistoryData")

def FetchHistoryData(stock_id,YearStart,YearEnd):
    global TotalY
    TotalY = int(YearEnd) - int(YearStart) +1
    GetDataByID(stock_id,YearStart+"01",YearEnd+"12")

#FetchHistoryData("2330","2017","2020")
