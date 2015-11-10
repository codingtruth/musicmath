SRC_DIR = src
INC_DIR = include
APP_DIR = apps
SRC_MODULES = $(SRC_DIR)/prime.cpp $(SRC_DIR)/frac.cpp $(SRC_DIR)/vibro.cpp 
INC_MODULES = $(INC_DIR)/prime.h $(INC_DIR)/frac.h $(INC_DIR)/vibro.h

SFML_ROOT=../../../../libs/SFML-2.3-windows-gcc-4.9.2-mingw-32-bit/SFML-2.3
SFML_LIB=$(SFML_ROOT)/lib
SFML_INC=$(SFML_ROOT)/include

test_prime: src/prime.cpp include/prime.h tests/prime_test.cpp src/frac.cpp include/frac.h
	gcc -I include src/prime.cpp src/frac.cpp tests/prime_test.cpp -o bin/prime_test.exe
	./bin/prime_test.exe > out/prime_test.txt

scale:	src/prime.cpp include/prime.h src/frac.cpp include/frac.h src/scale.cpp
	gcc -I include src/prime.cpp src/frac.cpp src/scale.cpp -o bin/scale.exe
	./bin/scale.exe > out/scale.txt

vibrokeys: $(SRC_MODULES) $(INC_MODULES) $(APP_DIR)/vibrokeys.cpp
	g++ $(APP_DIR)/vibrokeys.cpp $(SRC_MODULES) -I$(INC_DIR) -L$(SFML_LIB) -I$(SFML_INC) -lsfml-system -lsfml-audio-d -o bin/vibrokeys.exe

vibroplayer: $(SRC_MODULES) $(INC_MODULES) $(SRC_DIR)/player.cpp
	g++ $(SRC_DIR)/player.cpp $(SRC_MODULES) -I$(INC_DIR) -L$(SFML_LIB) -I$(SFML_INC) -lsfml-system -lsfml-audio-d -o bin/vibroplayer.exe
