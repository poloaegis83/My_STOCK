CC  = cl
OBJ = StockEmulator.obj
XMLlib = ExpatLib\libexpat.lib
STOCK = StockEmulator.c

all:
	$(CC) $(STOCK) $(XMLlib)
    del	  $(OBJ)
	
.PHONY:cln
cln:
	del $(EXE) $(OBJ)