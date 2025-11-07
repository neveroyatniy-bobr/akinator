MY_PROGRAM = program.exe

CXX = g++

SRC_PREF = source/
OBJ_PREF = build/
INCLUDE = include

SRC = $(wildcard $(SRC_PREF)*.cpp) 
OBJ = $(patsubst $(SRC_PREF)%.cpp, $(OBJ_PREF)%.o, $(SRC))


DEBUG_FLAGS = -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
RELEASE_FLAGS = -O2 -D NDEBUG

BUILD_TYPE ?= DEBUG
ifeq ($(BUILD_TYPE), RELEASE)
	FLAGS = $(RELEASE_FLAGS)
else
	FLAGS = $(DEBUG_FLAGS)
endif

FLAGS += -I$(INCLUDE)

RED = "\e[31m"
RESET = "\e[0m"

MD = mkdir
RM = rm

.PHONY : all build_and_run build run doxygen commit_warning create_build_dir clean

all: build

build_and_run: build run

ifeq ($(BUILD_TYPE), RELEASE)
build: create_build_dir $(MY_PROGRAM) commit_warning
	@echo "RELEASE_BUILD\n"
else
build: create_build_dir $(MY_PROGRAM) commit_warning
	@echo "DEBUG_BUILD\n"
endif

$(MY_PROGRAM): $(OBJ) $(OBJ_PREF)main.o
	@$(CXX) $^ -o $(MY_PROGRAM) $(FLAGS)

$(OBJ_PREF)main.o: main.cpp
	@$(CXX) -c main.cpp -o $(OBJ_PREF)main.o $(FLAGS)

$(OBJ_PREF)%.o: $(SRC_PREF)%.cpp
	@$(CXX) -c $< -o $@ $(FLAGS)

run:
	@./$(MY_PROGRAM)

create_build_dir:
	@$(MD) -p $(OBJ_PREF)

clean:
	@$(RM) -rf $(OBJ_PREF)

doxygen:
	@doxygen docs/Doxyfile 1>/dev/null

commit_warning:
	@echo $(RED) "\n!!! COMMIT GITHUB !!!\n" $(RESET)

