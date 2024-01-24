.PHONY : build rebuild

default :
	@echo "======================================="
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make rebuild' command to build it.."
	@echo "======================================="

INCLUDES += -I../ -I../../ -I../../../ -I../../src
LIBS = -L../ -L../../ -L../../../

CFLAGS = -Wall -O3 -fPIC --shared -Wl,-rpath,. -Wl,-rpath,..

rebuild:
	@$(CC) -o toml.so ltoml.c toml.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore
	@mv *.so ../

build:
	@$(CC) -o toml.so ltoml.c toml.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore
	@mv *.so ../
