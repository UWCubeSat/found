
SRCS := $(wildcard src/*.cpp)

TEST_BIN := $(wildcard test/*.cpp)

OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TEST_OBJS := $(patsubst %.cpp,%.o,$(TESTS) $(filter-out %/main.o, $(OBJS)))

BIN := found

all: $(BIN)

LIBS     := # -lcairo
CXXFLAGS := $(CXXFLAGS) -Ivendor -Isrc -Idocumentation -Wall -Wextra -Wno-missing-field-initializers -pedantic --std=c++11

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(BIN) $(BSC) $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS)
	$(CXX) $(LDFLAGS) -o $(TEST_BIN) $(TEST_OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(BIN)