CXX = crg++

CXXFLAGS = 

SRC = \
	src/rootfile_analyzer.cpp \
	src/hit_selection.cpp \
	src/feature_analyzer.cpp

BIN_DIR = bin
BUILD_DIR = build
RESULT_DIR = result

DIRS = $(BIN_DIR) $(BUILD_DIR) $(RESULT_DIR)

OBJ = \
	  $(BUILD_DIR)/rootfile_analyzer.o \
	  $(BUILD_DIR)/hit_selection.o \
	  $(BUILD_DIR)/feature_analyzer.o

APPS = \
	   feature_extraction \
	   feature_summary

TARGETS = $(addprefix $(BIN_DIR)/,$(APPS))

all: dirs \
	 $(TARGETS)

dirs:
	mkdir -p $(DIRS)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/%: app/%.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGETS)

.PHONY: all clean dirs