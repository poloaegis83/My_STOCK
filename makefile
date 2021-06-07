CC  = cl
OBJ1 = StockEmulator.obj
OBJ2 = Simulator1.obj
OBJ3 = Simulator2.obj
XMLlib = ExpatLib\libexpat.lib
STOCK1 = StockEmulator.c
STOCK2 = Simulator1.c
STOCK3 = Simulator2.c

all:
	$(CC) $(STOCK1) $(STOCK2) $(STOCK3) $(XMLlib)
    del	  $(OBJ1) $(OBJ2) $(OBJ3)
	
.PHONY:cln
cln:
	del $(EXE) $(OBJ1) $(OBJ2) $(OBJ3)