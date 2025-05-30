CXX       := g++
CXXFLAGS  := -std=c++17 -Wall \
              -I. \
              -Idatabase \
              -Imodels \
              -IRepository \
              -Iservice
LDFLAGS   := -lsqlite3

SRC := \
    main.cpp \
    database/database.cpp \
    database/statement.cpp \
    database/database_init.cpp

OBJ := $(SRC:.cpp=.o)
TARGET := shop_app

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
