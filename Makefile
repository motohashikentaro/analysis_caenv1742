CXX = crg++

CXXFLAGS = 

SRC = \
	src/rootfile_analyzer.cpp \
	src/hit_selection.cpp \
	src/feature_analyzer.cpp \
	src/hit_analyzer.cpp \
	src/analysis_process.cpp \
	src/hit_correlation.cpp \
	src/correla_ana.cpp \
	src/correla_peak.cpp \
	src/correla_multiplicity.cpp

BIN_DIR = bin
BUILD_DIR = build
RESULT_DIR = result

DIRS = $(BIN_DIR) $(BUILD_DIR) $(RESULT_DIR)

OBJ = \
	  $(BUILD_DIR)/rootfile_analyzer.o \
	  $(BUILD_DIR)/hit_selection.o \
	  $(BUILD_DIR)/feature_analyzer.o \
	  $(BUILD_DIR)/hit_analyzer.o \
	  $(BUILD_DIR)/analysis_process.o \
	  $(BUILD_DIR)/hit_correlation.o \
	  $(BUILD_DIR)/correla_ana.o \
	  $(BUILD_DIR)/correla_peak.o \
	  $(BUILD_DIR)/correla_multiplicity.o

APPS = \
	   feature_extraction \
	   feature_summary \
	   hit_extraction \
	   hit_summary \
	   waveform \
	   feature_summary_bycanvas \
	   make_corre_file \
	   correla_ana \
	   correla_peak \
	   hit_summary_bycanvas \
	   correla_multiplicity

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