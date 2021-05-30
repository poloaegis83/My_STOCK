import os
import requests
import json
import numpy    as np
import pandas   as pd
import xml.etree.cElementTree as ET
import lxml.etree as etree
from   bs4      import BeautifulSoup

headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

#print(res.encoding) #查看網頁返回的字符集類型 
#print(res.apparent_encoding) #自動判斷字符集類型
#os.mknod('test.xml')

stock_id = input ("請輸入股票代碼: ")
year = input ("請輸入year: ")
month = input ("請輸入mouth: ")
print(stock_id)
year = str(int(year)-1911)

url1 = "https://stock.wearn.com/cdata.asp?year="+year+"&month="+month+"&kind="+stock_id   # 股價
url2 = "https://stock.wearn.com/netbuy.asp?Year="+year+"&month="+month+"&kind="+stock_id  # 三大法人
url3 = "https://stock.wearn.com/zhuli.asp?Year="+year+"&month="+month+"&kind="+stock_id   # 主力進出

UrlArr = [url1,url2,url3]
Soup   = ['','','']
Tables = ['','','']
tab    = ['','','']

for i in range(3):
    print(UrlArr[i])
    res = requests.get (UrlArr[i], headers = headers)
    res.encoding = 'cp950'
    Soup[i] = BeautifulSoup (res.text, "html.parser")
    print(Soup[i].title)
    Tables[i] = Soup[i].findAll('table')
    tab[i]    = Tables[i][0]

# Init XML struct
StockData = ET.Element('StockData',Version="1.0")
StockId = ET.SubElement(StockData, 'StockId')
StockId.text = stock_id

i = 0
j = 0
length = len(tab[0].findAll('tr'))

for tr in tab[0].findAll('tr'):  #tab[0] = 股價
    if i >= 2:
        Daily = ET.SubElement(StockData, 'Daily') # Index="0",Years="0000",Months="00",Days="00")
        Daily.set('Index',str(length-i))
        Price = ET.SubElement(Daily, 'Price')
        Difference = ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0") # Will uptade in tab[1] and tab[2]
    for td in tr.findAll('td'):
        if i >= 2 and j % 6 == 1:  #Time
            Daily.set('Years',td.getText()[0:3])
            Daily.set('Months',td.getText()[4:6])
            Daily.set('Days',td.getText()[7:9])
        if i >= 2 and j % 6 == 2:  #Start
            Price.set('Start',(td.getText().rstrip()).replace('.',"") )  # replace '.' to '' and remove right space
        if i >= 2 and j % 6 == 3:  #High
            Price.set('High',(td.getText().rstrip()).replace('.',"") )
        if i >= 2 and j % 6 == 4:  #Low
            Price.set('Low',(td.getText().rstrip()).replace('.',"") )
        if i >= 2 and j % 6 == 5:  #End
            Price.set('End',(td.getText().rstrip()).replace('.',"") )
        j += 1
        #print('i=',i,'j=',j)
    i += 1

i = 0
j = 0
for tr in tab[1].findAll('tr'):  #tab[1] = 三大法人
    for td in tr.findAll('td'):
        #if i >= 2:
            #if j % 4 = 2:  # 投信
            #if j % 4 = 3:  # 自營商
            #if j % 4 = 0:  # 外資
        print('i=',i,'j=',j)
        print(td.getText())
        j += 1
    i += 1      


i = 0
j = 0
for tr in tab[2].findAll('tr'):  #tab[2] = 主力庫存
    for td in tr.findAll('td'):
        #if i >= 2:
            #if j % 4 = 0:  # 主力增減
        print('i=',i,'j=',j)
        print(td.getText())
        j += 1
    i += 1

#Daily.set('Index','1')
#Daily.set('Years','2021')
#Daily.set('Months','01')
#Daily.set('Days','01')

#Price.set('Low','money')
#Price.set('High','money')
#Price.set('End','money')
#Price.set('Start','money')

#Difference.set('DealersDiff','Diff')
#Difference.set('InvestmentTrustDiff','Diff')
#Difference.set('ForeignInvestorsDiff','Diff')
#Difference.set('LeaderDiff','Diff')

# XML 檔案
tree = ET.ElementTree(StockData)
tree.write("test.xml",xml_declaration=True,encoding='UTF-8',method="xml")

# 讀取 XML 檔案
root = etree.parse("test.xml")

# 輸出排版的 XML 資料
print(etree.tostring(root, pretty_print=True, encoding="unicode"))

# 將排版的 XML 資料寫入檔案
root.write("test.xml", encoding="utf-8")

#ET.dump(StockData)

#os.remove('test.xml')
