CXX = crg++

SRC = \
	src/rootfile_analyzer.cpp \
	src/analysis_process.cpp \
	src/save_objects.cpp

BIN_DIR = bin
BUILD_DIR = build
RESULT_DIR = result

DIRS = $(BIN_DIR) $(BUILD_DIR) $(RESULT_DIR)

OBJ = \
	  $(BUILD_DIR)/rootfile_analyzer.o \
	  $(BUILD_DIR)/analysis_process.o \
	  $(BUILD_DIR)/save_objects.o

APPS = \
	   multiplicity \
	   hitmap \
	   avg_waveform \
	   min_adc_distro \
	   simple_waveform \
	   separate_waveform \
	   single_waveform

TARGETS = $(addprefix $(BIN_DIR)/,$(APPS))

all: dirs \
	 $(TARGETS)

dirs:
	mkdir -p $(DIRS)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) -c $< -o $@

$(BIN_DIR)/%: app/%.cpp $(OBJ)
	$(CXX) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGETS)

.PHONY: all clean dirs