import os
import requests
import json
import numpy    as np
import pandas   as pd
import xml.etree.cElementTree as ET
import lxml.etree as etree
from   bs4      import BeautifulSoup

headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

stock_id = input ("請輸入股票代碼(ex:2330):")
year = input ("請輸入year")
month = input ("請輸入mouth")
print(stock_id)
yearint = int(year)
yearint -= 1911
year = str(yearint)
#url = "https://www.twse.com.tw/exchangeReport/STOCK_DAY?response=html&date="+year+""+mouth+"01&stockNo="+stock_id
url = "https://stock.wearn.com/cdata.asp?year="+year+"&month="+month+"&kind="+stock_id
#https://stock.wearn.com/netbuy.asp?Year=108&month=05&kind=2330
#https://stock.wearn.com/zhuli.asp?Year=110&month=05&kind=2330
print(url)
res = requests.get (url, headers = headers)
print(res.encoding) #查看網頁返回的字符集類型 
print(res.apparent_encoding) #自動判斷字符集類型
res.encoding = 'cp950'

soup = BeautifulSoup (res.text, "html.parser")
#print(soup.prettify())

title_tag = soup.title
print(title_tag)

a_tags = soup.find_all('stockalllistbg1')

#for tag in a_tags:
  # 輸出超連結的文字
 # print(tag.string)

link2_tag = soup.find_all(class_='stockalllistbg1')
#print(link2_tag)

link2_tag = soup.find_all(class_='stockalllistbg2')
#print(link2_tag)

#for tag in link2_tag:
  # 輸出超連結的文字
 # print(tag.string)


#os.mknod('test.xml')


StockData = ET.Element('StockData',Version="1.0")
StockId = ET.SubElement(StockData, 'StockId')
Daily = ET.SubElement(StockData, 'Daily', Index="0",Years="0000",Months="00",Days="00")
Price = ET.SubElement(Daily, 'Price', Start="0",End="0",High="0",Low="0")
Difference = ET.SubElement(Daily, 'Difference',LeaderDiff="0",ForeignInvestorsDiff="0",InvestmentTrustDiff="0",DealersDiff="0")

StockId.text = "Stock ID"

Daily.set('Index','1')
Daily.set('Years','2021')
Daily.set('Months','01')
Daily.set('Days','01')

Price.set('Low','money')
Price.set('High','money')
Price.set('End','money')
Price.set('Start','money')

Difference.set('DealersDiff','Diff')
Difference.set('InvestmentTrustDiff','Diff')
Difference.set('ForeignInvestorsDiff','Diff')
Difference.set('LeaderDiff','Diff')

tree = ET.ElementTree(StockData)
tree.write("test.xml",xml_declaration=True,encoding='UTF-8',method="xml")

# 讀取 XML 檔案
root = etree.parse("test.xml")

# 輸出排版的 XML 資料
print(etree.tostring(root, pretty_print=True, encoding="unicode"))

# 將排版的 XML 資料寫入檔案
root.write("test.xml", encoding="utf-8")

ET.dump(StockData)

#os.remove('test.xml')
