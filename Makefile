CXX = crg++

CXXFLAGS = 

SRC = \
    src/plot_supporter.cpp \
	src/rootfile_reader.cpp \
	src/rootfile_analyzer.cpp \
	src/feature_extractor.cpp \
	src/feature_reader.cpp \
	src/feature_analyzer.cpp \
	src/hit_extractor.cpp \
	src/selection_conditions.cpp \
	src/hit_reader.cpp \
	src/hit_analyzer.cpp \
	src/matched_hit_analyzer.cpp \
	src/correlation_analyzer.cpp 

BIN_DIR = bin
BUILD_DIR = build
RESULT_DIR = result

DIRS = $(BIN_DIR) $(BUILD_DIR) $(RESULT_DIR)

OBJ = \
	  $(BUILD_DIR)/plot_supporter.o \
	  $(BUILD_DIR)/rootfile_reader.o \
	  $(BUILD_DIR)/rootfile_analyzer.o \
	  $(BUILD_DIR)/feature_extractor.o \
	  $(BUILD_DIR)/feature_reader.o \
	  $(BUILD_DIR)/feature_analyzer.o \
	  $(BUILD_DIR)/hit_extractor.o \
	  $(BUILD_DIR)/selection_conditions.o \
	  $(BUILD_DIR)/hit_reader.o \
	  $(BUILD_DIR)/hit_analyzer.o \
	  $(BUILD_DIR)/matched_hit_analyzer.o \
	  $(BUILD_DIR)/correlation_analyzer.o

APPS = \
	   waveform \
	   waveform_map \
	   rootfile_merger \
	   feature_extractor \
	   feature_summary \
	   feature_summary_per_canvas \
	   hit_extractor \
	   hit_extractor2 \
	   hit_extractor3 \
	   hit_summary \
	   correlation_summary

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