TARGET = Processor/build/processor.exe
CPPFLAGS = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -lSDL2
#CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla -lSDL2

.PHONY: all
all: $(TARGET)

$(TARGET): Processor/bin/processor_main.o Processor/bin/processor.o Stack/bin/stack.o Stack/bin/stack_logs.o Stack/bin/verification.o Common/bin/file_reading.o
	g++ Processor/bin/processor_main.o Processor/bin/processor.o Stack/bin/stack.o Stack/bin/stack_logs.o Stack/bin/verification.o Common/bin/file_reading.o -o $(TARGET) $(CPPFLAGS)

Processor/bin/processor_main.o: Processor/processor_main.cpp Processor/processor.hpp Common/file_reading.hpp Common/commands.hpp
	g++ -c Processor/processor_main.cpp -o Processor/bin/processor_main.o $(CPPFLAGS)

Processor/bin/processor.o: Processor/processor.cpp Processor/processor.hpp Common/commands.hpp
	g++ -c Processor/processor.cpp -o Processor/bin/processor.o $(CPPFLAGS)

Stack/bin/stack.o: Stack/stack.cpp Stack/stack.h
	g++ -c Stack/stack.cpp -o Stack/bin/stack.o $(CPPFLAGS)

Stack/bin/stack_logs.o: Stack/stack_logs.cpp Stack/stack.h Stack/stack_logs.h
	g++ -c Stack/stack_logs.cpp -o Stack/bin/stack_logs.o $(CPPFLAGS)

Stack/bin/verification.o: Stack/stack_verification.cpp Stack/stack.h Stack/stack_verification.h
	g++ -c Stack/stack_verification.cpp -o Stack/bin/verification.o $(CPPFLAGS)

Common/bin/file_reading.o: Common/file_reading.cpp Common/file_reading.hpp
	g++ -c Common/file_reading.cpp -o Common/bin/file_reading.o $(CPPFLAGS)
