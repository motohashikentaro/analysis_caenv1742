CXX = crg++

SRC = \
	src/rootfile_analyzer.cpp \
	src/analysis_process.cpp

BIN_DIR = bin
BUILD_DIR = build
RESULT_DIR = result

OBJ = \
	  $(BUILD_DIR)/rootfile_analyzer.o \
	  $(BUILD_DIR)/analysis_process.o

DIRS = $(BIN_DIR) $(BUILD_DIR) $(RESULT_DIR)

all: dirs \
	 $(BIN_DIR)/multiplicity \
     $(BIN_DIR)/hitmap \
     $(BIN_DIR)/average_waveform \
     $(BIN_DIR)/min_adc_distro

dirs:
	mkdir -p $(DIRS)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) -c $< -o $@

$(BIN_DIR)/multiplicity: app/multiplicity.cpp $(OBJ)
	$(CXX) $^ -o $@

$(BIN_DIR)/hitmap: app/hitmap.cpp $(OBJ)
	$(CXX) $^ -o $@

$(BIN_DIR)/average_waveform: app/avg_waveform.cpp $(OBJ)
	$(CXX) $^ -o $@

$(BIN_DIR)/min_adc_distro: app/min_adc_distro.cpp $(OBJ)
	$(CXX) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(BIN_DIR)/multiplicity \
	      $(BIN_DIR)/hitmap \
	      $(BIN_DIR)/average_waveform \
	      $(BIN_DIR)/min_adc_distro

.PHONY: all clean dirs