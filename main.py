import requests
import xml.etree.cElementTree as ET
import lxml.etree as etree
from   bs4      import BeautifulSoup

headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

#print(res.encoding) #查看網頁返回的字符集類型 
#print(res.apparent_encoding) #自動判斷字符集類型


def Get_Data(stock_id,year,month):
    yearw = str(int(year)+1911)
    url1 = "https://www.twse.com.tw/exchangeReport/STOCK_DAY?response=html&date="+yearw+month+"01&stockNo="+stock_id   # 股價
    url2 = "https://stock.wearn.com/netbuy.asp?Year="+year+"&month="+month+"&kind="+stock_id  # 三大法人
    url3 = "https://stock.wearn.com/zhuli.asp?Year="+year+"&month="+month+"&kind="+stock_id   # 主力進出

    UrlArr = [url1,url2,url3]
    Soup   = ['','','']
    Tables = ['','','']
    tab    = ['','','']

    for i in range(3):  # Dump all 3 urls data
        print(UrlArr[i])
        res = requests.get (UrlArr[i], headers = headers)
        res.encoding = 'cp950'
        Soup[i] = BeautifulSoup (res.text, "html.parser")
        #print(Soup[i].title)
        Tables[i] = Soup[i].findAll('table')
        tab[i]    = Tables[i][0]

    i = 0 # Daily index
    j = 0 # tab[0] td index2
    k = 0 # tab[1] td index
    l = 0 # tab[2] td index
    length = len(tab[0].findAll('tr'))

    # Reverse the tab[0] becasue tab[0] direction not same as tab[1] and tab[2]
    #tt = tab[0].findAll('tr')
    #print(tt)

    # 先嘗試修改函式屬性的值，如果出現AttributeError
    # 表示該屬性不存在，這時候就建立該屬性
    try:
        Get_Data.count += length-2   # 加函式屬性的值(static variable)
    except AttributeError: 
        Get_Data.count = length   # 建立函式的屬性(static variable)
    
    count1 = Get_Data.count - length # For Insert element 用
    count2 = Get_Data.count          # For daily index 屬性累加

    for tr1,tr2,tr3 in zip(tab[0].findAll('tr'), tab[1].findAll('tr'), tab[2].findAll('tr')):
        if i >= 2:  # 新增 daily element
            Daily = ET.Element('Daily')
            StockData.insert(1+count1,Daily)
            Daily.set('Index',str(count2-i))
            Price = ET.SubElement(Daily, 'Price')
            Difference = ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0") # Will uptade in tab[1] and tab[2]

        for td in tr1.findAll('td'): # 股價
            if i >= 2:
                if j % 9 == 0:  # 日期
                    Daily.set('Years',str(int(td.getText()[0:3])+1911))
                    Daily.set('Months',td.getText()[4:6])
                    Daily.set('Days',td.getText()[7:9])
                if j % 9 == 3:  # 開盤
                    Price.set('Start',(td.getText().rstrip()).replace('.',"") ) 
                if j % 9 == 4:  # 最高
                    Price.set('High',(td.getText().rstrip()).replace('.',"") )
                if j % 9 == 5:  # 最低
                    Price.set('low',(td.getText().rstrip()).replace('.',"") )              
                if j % 9 == 6:  # 收盤             
                    Price.set('End',(td.getText().rstrip()).replace('.',"") )
            j += 1
        for td in tr2.findAll('td'):  # 三大法人
            if i >= 2:
                if k % 4 == 2:  # 投信
                    Difference.set('InvestmentTrustDiff',(td.getText().rstrip()).replace(',',"") )
                if k % 4 == 3:  # 自營商
                    Difference.set('DealersDiff',(td.getText().rstrip()).replace(',',""))
                if k % 4 == 0:  # 外資
                    Difference.set('ForeignInvestorsDiff',(td.getText().rstrip()).replace(',',"") )
            k += 1
        for td in tr3.findAll('td'): # 主力進出
            if i >= 2:
                if l % 3 == 0:  # 主力增減
                    Difference.set('LeaderDiff',(td.getText().rstrip()).replace(',',"") )
            l += 1
        i += 1


stock_id = input ("請輸入股票代碼: ")
'''
InputStart = input ("請輸入起始年月 (ex:201911): ")
InputEnd = input ("請輸入結束年月(ex:202101): ")

YearStart  = int(InputStart[0:4])-1911
MonthStart = int(InputStart[4:6])

YearEnd    = int(InputEnd[0:4])-1911
MonthEnd   = int(InputEnd[4:6])

YearDiff   = YearEnd - YearStart

if YearDiff == 0:
    MonthDiff = MonthEnd - MonthStart
elif YearDiff > 0:
    MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
else:
    BaseException: Error
'''
# Init XML struct
StockData = ET.Element('StockData',Version="1.0")
StockId = ET.SubElement(StockData, 'StockId')
StockId.text = stock_id
'''
if YearDiff == 0:
    MonthDiff = MonthEnd - MonthStart
    print('Mdiff',MonthDiff)
    for monthI in range(MonthDiff+1):
        YearData  = str(YearStart)
        MonthData = str(MonthStart+monthI).rjust(2,'0')
        Get_Data(stock_id, YearData, MonthData)        
elif YearDiff > 0:
    MonthDiff = ((12 - MonthStart) + MonthEnd) + (YearDiff-1)*12
    print('Mdiff',MonthDiff)
    yearI = 0
    for monthI in range(MonthDiff+1):
        YearData  = str(YearStart+yearI)
        MonthData = str(((MonthStart+monthI)%12)).rjust(2,'0')
        if MonthData == '00':
            MonthData = '12'
            yearI += 1
        print('Y/M',YearData,MonthData)
        #Get_Data(stock_id, YearData, MonthData)
else:
    BaseException: Error
'''
Get_Data(stock_id,'109','01')
'''
Get_Data(stock_id,'109','02')
Get_Data(stock_id,'109','03')
Get_Data(stock_id,'109','04')
Get_Data(stock_id,'109','05')
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
