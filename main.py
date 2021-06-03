import requests
import xml.etree.cElementTree as ET
import lxml.etree as etree
from   bs4      import BeautifulSoup
import time
import random

#headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

user_agent_list = ["Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36",
                    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36",
                    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36",
                    "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)",
                    "Mozilla/5.0 (Macintosh; U; PPC Mac OS X 10.5; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15",
                   ]

StockData = ET.Element('StockData',Version="1.0") # global Stock data

def Get_Data(stock_id,year,month,IsTWSE):

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

    tab2 = tab[1].findAll('tr')
    tab2 = tab2[1:]
    tab2.reverse()

    tab3 = tab[2].findAll('tr')
    tab3 = tab3[1:]
    tab3.reverse()

    i = 0 # Daily index
    j = 0 # tab[0] td index2
    k = 0 # tab[1] td index
    l = 0 # tab[2] td index
    length = len(tab1)

    # 先嘗試修改函式屬性的值，如果出現AttributeError
    # 表示該屬性不存在，這時候就建立該屬性
    try:
        Get_Data.count += length   # 加函式屬性的值(static variable)
    except AttributeError: 
        Get_Data.count = length + 1    # 建立函式的屬性(static variable)

    count1 = Get_Data.count - length   # For daily index 屬性累加     

    global StockData

    for tr1,tr2,tr3 in zip(tab1, tab2, tab3):
        Daily = ET.Element('Daily')    # 新增 daily element
        StockData.append(Daily)
        Daily.set('Index',str(count1+i))
        Price = ET.SubElement(Daily, 'Price')
        Difference = ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0") # Will uptade in tab[1] and tab[2]
        for td in tr1.findAll('td'):  # 股價
            if j % 9 == 0:  # 日期
                Daily.set('Years',str(int(td.getText()[0:3])+1911))
                Daily.set('Months',td.getText()[4:6])
                Daily.set('Days',td.getText()[7:9])
            if j % 9 == 3:  # 開盤
                Price.set('Start',(td.getText().rstrip()).replace('.',"") ) 
            if j % 9 == 4:  # 最高
               Price.set('High',(td.getText().rstrip()).replace('.',"") )
            if j % 9 == 5:  # 最低
               Price.set('Low',(td.getText().rstrip()).replace('.',"") )              
            if j % 9 == 6:  # 收盤             
               Price.set('End',(td.getText().rstrip()).replace('.',"") )
            j += 1
        for td in tr2.findAll('td'):  # 三大法人
            if k % 4 == 1:  # 投信
                Difference.set('InvestmentTrustDiff',(td.getText().rstrip()).replace(',',"") )
            if k % 4 == 2:  # 自營商
                Difference.set('DealersDiff',(td.getText().rstrip()).replace(',',""))
            if k % 4 == 3:  # 外資
                Difference.set('ForeignInvestorsDiff',(td.getText().rstrip()).replace(',',"") )
            k += 1
        for td in tr3.findAll('td'): # 主力進出
            if l % 3 == 2:  # 主力增減
                Difference.set('LeaderDiff',(td.getText().rstrip()).replace(',',"") )
            l += 1
        i += 1


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


stock_id = input ("請輸入股票代碼: ")

InputStart = input ("請輸入起始年月 (ex:201911): ")
InputEnd = input ("請輸入結束年月(ex:202101): ")

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
        time.sleep(2)
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
        time.sleep(2)
        Get_Data(stock_id, YearData, MonthData, IsTWSE)
else:
    BaseException: Error

''' debug
IsTWSE = Is_TWSE_Listed(stock_id)
Get_Data(stock_id,'2020','01',IsTWSE)
Get_Data(stock_id,'2020','02',IsTWSE)
Get_Data(stock_id,'2020','03',IsTWSE)
Get_Data(stock_id,'2020','04',IsTWSE)
Get_Data(stock_id,'2020','05',IsTWSE)
'''
    # 建立 XML 檔案
tree = ET.ElementTree(StockData)
tree.write("test.xml",xml_declaration=True,encoding='UTF-8',method="xml")

    # 讀取 XML 檔案
root = etree.parse("test.xml")

    # 輸出與排版 XML 資料
print(etree.tostring(root, pretty_print=True, encoding="unicode"))

    # 將排版的 XML 資料寫入檔案
root.write("test.xml", encoding="utf-8")

#ET.dump(StockData)

#os.remove('test.xml')
