# コンパイラ
COMPILER  = gcc
# フラグ
CFLAGS    = -Wall -Wextra
# ライブラリ
LIBS      = -ljpeg
# インクルードパス
INCLUDE	= 
# 生成される実行ファイル
TARGETS   = a.out
# 生成されるバイナリファイルの出力ディレクトリ
TARGETDIR = .

SRCROOT   = .
SRCDIRS   = $(shell find $(SRCROOT) -type d)
SOURCES   = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

OBJROOT   = ./obj
OBJECTS   = $(addprefix $(OBJROOT)/, $(SOURCES:.c=.o)) 
OBJDIRS   = $(addprefix $(OBJROOT)/, $(SRCDIRS)) 
DEPENDS   = $(OBJECTS:.o=.d)

# 依存ファイルを元に実行ファイルを作る
$(TARGETS): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $(TARGETDIR)/$@ $^ $(LDFLAGS)

# 中間バイナリのディレクトリを掘りながら.cを中間ファイル.oに
$(OBJROOT)/%.o: %.c
	@if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

-include $(DEPENDS)

clean:
	rm -rf $(OBJROOT)