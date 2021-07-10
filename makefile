CC  = cl
OBJ1 = StockEmulator.obj
OBJ2 = Simulator1.obj
OBJ3 = Simulator2.obj
OBJ4 = CalculateData.obj
XMLlib = ExpatLib\libexpat.lib
STOCK1 = StockEmulator.c
STOCK2 = Simulator1.c
STOCK3 = Simulator2.c
STOCK4 = CalculateData.c

all:
	$(CC) $(STOCK1) $(STOCK2) $(STOCK3) $(XMLlib)
	$(CC) $(STOCK4)
    del	  $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) 
	
.PHONY:cln
cln:
	del $(EXE) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) 