import requests
import pandas as pd
import json
import numpy as np
import datetime

def crawl_price(stock_id):

    d = datetime.datetime.now()
    url = "https://query1.finance.yahoo.com/v8/finance/chart/"+stock_id+"?period1=0&period2="+str(int(d.timestamp()))+"&interval=1d&events=history&=hP2rOschxO0"

    res = requests.get(url)
    data = json.loads(res.text)
    df = pd.DataFrame(data['chart']['result'][0]['indicators']['quote'][0], index=pd.to_datetime(np.array(data['chart']['result'][0]['timestamp'])*1000*1000*1000))
    return df

df = crawl_price("2330.TW")
df.close.plot()


import os
import requests
import json
import numpy    as np
import pandas   as pd
from   bs4      import BeautifulSoup

headers = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.212 Safari/537.36"}

stock_id = input ("請輸入股票代碼(ex:2330):")
year = input ("請輸入year")
mouth = input ("請輸入mouth")
print(stock_id)

url = "https://www.twse.com.tw/exchangeReport/STOCK_DAY?response=html&date="+year+""+mouth+"01&stockNo="+stock_id
print(url)
res = requests.get (url, headers = headers)
res.encoding = "utf-8"

soup = BeautifulSoup (res.text, "html.parser")
print(soup.prettify())

os.mknod('test.xml')
os.remove('test.xml')
