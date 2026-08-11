CXX=g++
CXXFLAGS=-Wall -Wextra -std=c++23 -ggdb -Isrc -Ivendor -Iinclude
LIBFLAGS=-lpqxx -lsodium -lssl -lcrypto
SRCS=$(wildcard src/*.cpp) $(wildcard src/routes/*.cpp) $(wildcard src/utils/*.cpp)
OBJS=$(patsubst src/%.cpp, build/%.o, $(SRCS))
TARGET=build/backend
PCH=build/pch.hpp.gch

build: $(TARGET)
$(PCH): src/pch.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -x c++-header $< -o $@

build/%.o: src/%.cpp $(PCH)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -include build/pch.hpp -c $< -o $@

$(TARGET): $(OBJS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LIBFLAGS) 

serve: $(TARGET)
	./$(TARGET)

db-setup:
	PGPASSWORD=student_reg psql -h localhost -U srg_user -d student_reg -f database.sql -f database_seed.sql

db-seed:
	PGPASSWORD=student_reg psql -h localhost -U srg_user -d student_reg -f database_seed.sql

clean:
	rm -rf build

.PHONY: serve clean build db-setup db-seed
