import requests
url = "https://goodinfo.tw/StockInfo/StockBzPerformance.asp?STOCK_ID=2330"
res = requests.get(url)
res.encoding = "utf-8"
res.text
