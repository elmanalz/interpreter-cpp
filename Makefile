# ---------------------------------------------------------------------------
# Build settings
# ---------------------------------------------------------------------------
CXX     	:= g++
CXXFLAGS	:= -std=c++17 -Wall -Wextra -pedantic -g -MMD -MP

# MinGW appends .exe to output binaries; Linux/macOS do not.
# OS is set to Windows_NT on Windows (MSYS2 inherits it).
ifeq ($(OS),Windows_NT)
    EXT	:=	.exe
else
    EXT	:=
endif

# ---------------------------------------------------------------------------
# What gets built
# ---------------------------------------------------------------------------
# Source files shared by BOTH executables (everything except the two mains).
COMMON_SRC := lexer.cpp ast.cpp parser.cpp
COMMON_OBJ := $(COMMON_SRC:.cpp=.o)

APP	:= calc$(EXT)
TESTS 	:= tests$(EXT)

all: $(APP) $(TESTS)

# Each executable = its own main.o plus the shared objects.
$(APP): main.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTS): tests.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Pattern rule: how to turn any .cpp into a .o.
# -c means "compile only, do not link".
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# Convenience targets
# ---------------------------------------------------------------------------
run: $(TESTS)
	./$(TESTS)

run-app: $(APP)
	./$(APP)

clean:
	rm -f *.o *.d $(APP) $(TESTS)

.PHONY: all run run-app clean

# ---------------------------------------------------------------------------
# Header dependency tracking
# ---------------------------------------------------------------------------
# -MMD -MP (in CXXFLAGS) makes the compiler emit a .d file next to each .o
# listing every header that .cpp included. Including those files here teaches
# make which objects to rebuild when a header changes.
# The leading '-' means "don't complain if they don't exist yet".
-include $(COMMON_OBJ:.o=.d) main.d tests.d
