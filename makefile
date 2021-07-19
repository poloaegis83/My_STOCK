CC  = cl
OBJ1 = StockEmulator.obj
OBJ2 = Simulator.obj
OBJ3 = CalculateData.obj

STOCK1 = StockEmulator.c
STOCK2 = Simulator.c
STOCK3 = CalculateData.c

all:
	$(CC) $(STOCK1) $(STOCK2)
	$(CC) $(STOCK3)
    del	  $(OBJ1) $(OBJ2) $(OBJ3)
	
.PHONY:cln
cln:
	del $(EXE) $(OBJ1) $(OBJ2) $(OBJ3)