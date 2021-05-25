"""
Get Stock Information tool
"""

import requests
import json
import os
import numpy    as np
import pandas   as pd
import bs4      as BeautifulSoup

headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

def crawl_price(stock_id):

    url = "https://goodinfo.tw/StockInfo/ShowK_Chart.asp?STOCK_ID="+stock_id+"&CHT_CAT2=DATE"
    print(url)
    res = requests.get (url, headers = headers)
    res.encoding = "utf-8"
#    soup = BeautifulSoup (res.text, "html.parser")
#    print(soup.prettift())
    
os.system("cls")
print ('\n')
print ('                       ----------------------')
print ('                       | 股價資訊【股價資訊】 |')
print ('                       ----------------------')
print('*******************************************************************************')
print('*                             Function :                                      *')
print('*                       1.Inquire Stock Price                                 *')
print('*******************************************************************************')
stockId = input ("請輸入股票代碼(ex:2330):")
crawl_price(stockId)


#df.close.plot()