CXX := g++
CXXFLAGS := -Iinclude
SRC_DIR := src
BUILD_DIR := build

TARGET1 := oss
TARGET2 := user_proc
COLOR := color
MSG := msg
ERROR := error
CLOCK := clock

CLOCK_SRC := $(wildcard $(SRC_DIR)/$(CLOCK)/*.cpp)
MSG_SRC := $(wildcard $(SRC_DIR)/$(MSG)/*.cpp)
COLOR_SRC := $(wildcard $(SRC_DIR)/$(COLOR)/*.cpp)
ERROR_SRC := $(wildcard $(SRC_DIR)/$(ERROR)/*.cpp)
TARGET1_SRC := $(wildcard $(SRC_DIR)/$(TARGET1)/*.cpp)
TARGET2_SRC := $(wildcard $(SRC_DIR)/$(TARGET2)/*.cpp)

CLOCK_OBJ := $(patsubst $(SRC_DIR)/$(CLOCK)/*.cpp, $(BUILD_DIR)/$(CLOCK)/%.o, $(CLOCK_SRC))
MSG_OBJ := $(patsubst $(SRC_DIR)/$(MSG)/*.cpp, $(BUILD_DIR)/$(MSG)/%.o, $(MSG_SRC))
COLOR_OBJ := $(patsubst $(SRC_DIR)/$(COLOR)/*.cpp, $(BUILD_DIR)/$(COLOR)/%.o, $(COLOR_SRC))
ERROR_OBJ := $(patsubst $(SRC_DIR)/$(ERROR)/*.cpp, $(BUILD_DIR)/$(ERROR)/%.o, $(ERROR_SRC))
TARGET1_OBJ := $(patsubst $(SRC_DIR)/$(TARGET1)/*.cpp, $(BUILD_DIR)/$(TARGET1)/%.o, $(TARGET1_SRC))
TARGET2_OBJ := $(patsubst $(SRC_DIR)/$(TARGET2)/*.cpp, $(BUILD_DIR)/$(TARGET2)/%.o, $(TARGET2_SRC))

COMMON_OBJS := $(MSG_OBJ) $(CLOCK_OBJ) $(ERROR_OBJ) $(COLOR_OBJ)

all:	$(TARGET1) $(TARGET2)

$(TARGET1):	$(TARGET1_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TARGET2):	$(TARGET2_OBJ) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/$(TARGET1)/%.o:	$(SRC_DIR)/$(TARGET1)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
$(BUILD_DIR)/$(TARGET2)/%.o:	$(SRC_DIR/$(TARGET2)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
$(BUILD_DIR)/$(MSG)/%.o:	$(SRC_DIR)/$(MSG)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
$(BUILD_DIR)/$(COLOR)/%.o:	$(SRC_DIR)/$(COLOR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
$(BUILD_DIR)/$(ERROR)/%.o:	$(SRC_DIR)/$(ERROR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
$(BUILD_DIR)/$(CLOCK)/%.o:	$(SRC_DIR)/$(CLOCK)/%.cpp
	$mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	/bin/rm $(BUILD_DIR) $(TARGET1) $(TARGET)



