CC  = cl
OBJ1 = StockEmulator.obj
OBJ2 = Simulator1.obj
OBJ3 = Simulator2.obj
OBJ4 = Simulator3.obj
OBJ5 = CalculateData.obj
XMLlib = ExpatLib\libexpat.lib
STOCK1 = StockEmulator.c
STOCK2 = Simulator1.c
STOCK3 = Simulator2.c
STOCK5 = CalculateData.c
STOCK4 = Simulator3.c

all:
	$(CC) $(STOCK1) $(STOCK2) $(STOCK3) $(STOCK4) $(XMLlib)
	$(CC) $(STOCK5)
    del	  $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) 
	
.PHONY:cln
cln:
	del $(EXE) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(OBJ5) 