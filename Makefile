##################################################
# ソースファイルの格納先
SRCDIR  = ./src

##################################################
# コンパイラの指定
CC := g++ -std=gnu++14

##################################################
# マクロ設定
# ------------------------------
# -- エネルギー計算を重心からの有限半径内でカットオフする
# MACROS += -DENABLE_CUTOFF_RADIUS

##################################################
# コンパイルオプションの指定
CFLAGS += -O3
CFLAGS += -ffast-math
CFLAGS += -funroll-loops
CFLAGS += -march=native
CFLAGS += -mtune=native
CFLAGS += -mavx
# CFLAGS += -Wall
# CFLAGS += -pg
# CFLAGS += -fopenmp

##################################################
# 実行ファイル名
PROGRAM = nbody.out

##################################################
# コンパイル対象のcppファイル検索
CPPOBJS += $(patsubst $(SRCDIR)/%.cpp, %.o, $(wildcard $(SRCDIR)/*.cpp))

##################################################
# Recipes
.PHONY: clean all cleanlog cleandir

# ------------------------------
# -- Make object files
%.o:	$(SRCDIR)/%.cpp
	@echo "Bulding $< ..."
	@$(CC) -c $< $(CFLAGS) $(MACROS)
	@echo "[$< OK]"

# ------------------------------
# -- Link object files and make execution file
all:	$(CPPOBJS)
	@echo "Linking object files..."
	$(CC) $(CFLAGS) $(MACROS) $(CPPOBJS) -o $(PROGRAM)
	@echo "Link Success! [$(PROGRAM)]"

# ------------------------------
# -- Delete all execution/object files
clean:
	-rm *.out *.o

# ------------------------------
# -- Delete all log files
cleanlog:
	-rm *.log

# ------------------------------
# -- Delete result files
cleandir:
	rm -rf result
