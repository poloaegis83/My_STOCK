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

for i in range(3):  # Dump all 3 urls data
    #print(UrlArr[i])
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

i = 0 # Daily index
j = 0 # tab[0] td index
k = 0 # tab[1] td index
l = 0 # tab[2] td index
length = len(tab[0].findAll('tr'))

for tr1,tr2,tr3 in zip(tab[0].findAll('tr'), tab[1].findAll('tr'), tab[2].findAll('tr')):
    if i >= 2:  # 新增 daily element
        Daily = ET.Element('Daily')
        StockData.insert(1,Daily)
        Daily.set('Index',str(length-i))
        Price = ET.SubElement(Daily, 'Price')
        Difference = ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0") # Will uptade in tab[1] and tab[2]

    for td in tr1.findAll('td'): # 股價
        if i >= 2:
            if j % 6 == 1:  # 日期
                Daily.set('Years',str(int(td.getText()[0:3])+1911))
                Daily.set('Months',td.getText()[4:6])
                Daily.set('Days',td.getText()[7:9])
            if j % 6 == 2:  # 開盤
                Price.set('Start',(td.getText().rstrip()).replace('.',"") ) 
            if j % 6 == 3:  # 最高
                Price.set('High',(td.getText().rstrip()).replace('.',"") )
            if j % 6 == 4:  # 最低
                Price.set('low',(td.getText().rstrip()).replace('.',"") )              
            if j % 6 == 5:  # 收盤             
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
