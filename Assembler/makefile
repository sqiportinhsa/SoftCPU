TARGET = Assembler/build/assembler.exe
CPPFLAGS = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

.PHONY: all
all: $(TARGET)

$(TARGET): Assembler/bin/ass_main.o Assembler/bin/ass.o Stack/bin/stack.o Stack/bin/stack_logs.o Stack/bin/verification.o Common/bin/file_reading.o 
	g++ Assembler/bin/ass_main.o Assembler/bin/ass.o Stack/bin/stack.o Stack/bin/stack_logs.o  Stack/bin/verification.o Common/bin/file_reading.o -o $(TARGET) $(CPPFLAGS)

Assembler/bin/ass_main.o: Assembler/ass_main.cpp Assembler/ass.hpp
	g++ -c Assembler/ass_main.cpp -o Assembler/bin/ass_main.o $(CPPFLAGS)

Assembler/bin/ass.o: Assembler/ass.cpp Assembler/ass.hpp
	g++ -c Assembler/ass.cpp -o Assembler/bin/ass.o $(CPPFLAGS)

Stack/bin/stack.o: Stack/stack.cpp
	g++ -c Stack/stack.cpp -o Stack/bin/stack.o $(CPPFLAGS)

Stack/bin/stack_logs.o: Stack/stack_logs.cpp
	g++ -c Stack/stack_logs.cpp -o Stack/bin/stack_logs.o $(CPPFLAGS)

Stack/bin/verification.o: Stack/stack_verification.cpp
	g++ -c Stack/stack_verification.cpp -o Stack/bin/verification.o $(CPPFLAGS)

Common/bin/file_reading.o: Common/file_reading.cpp Common/file_reading.hpp
	g++ -c Common/file_reading.cpp -o Common/bin/file_reading.o $(CPPFLAGS)
