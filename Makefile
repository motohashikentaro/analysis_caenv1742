CXX = crg++

SRC = \
	../src/rootfile_analyzer.cpp \
	../src/analysis_process.cpp

BIN_DIR = ../bin

all: $(BIN_DIR)/multi \
     $(BIN_DIR)/hitmap \
     $(BIN_DIR)/avgw \
     $(BIN_DIR)/min_adc_distro

$(BIN_DIR)/multi: ../app/multiplicity.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

$(BIN_DIR)/hitmap: ../app/hitmap.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

$(BIN_DIR)/avgw: ../app/avg_waveform.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

$(BIN_DIR)/min_adc_distro: ../app/min_adc_distro.cpp $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

clean:
	rm -f $(BIN_DIR)/multi \
	      $(BIN_DIR)/hitmap \
	      $(BIN_DIR)/avgw \
	      $(BIN_DIR)/min_adc_distro

.PHONY: all clean