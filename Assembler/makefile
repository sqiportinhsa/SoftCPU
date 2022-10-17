TARGET = Assembler\build\assembler.exe
CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla

.PHONY: all
all: $(TARGET)

$(TARGET): Assembler\bin\ass_main.o Assembler\bin\ass.o Stack\bin\stack.o Stack\bin\stack_logs.o Stack\bin\verification.o Common\bin\file_reading.o Common\bin\utils.o
	g++ Assembler\bin\ass_main.o Assembler\bin\ass.o Stack\bin\stack.o Stack\bin\stack_logs.o  Stack\bin\verification.o Common\bin\file_reading.o Common\bin\utils.o -o $(TARGET) $(CPPFLAGS)

Assembler\bin\ass_main.o: Assembler\ass_main.cpp Assembler\ass.h
	g++ -c Assembler\ass_main.cpp -o Assembler\bin\ass_main.o $(CPPFLAGS)

Assembler\bin\ass.o: Assembler\ass.cpp Assembler\ass.h
	g++ -c Assembler\ass.cpp -o Assembler\bin\ass.o $(CPPFLAGS)

Stack\bin\stack.o: Stack\stack.cpp
	g++ -c Stack\stack.cpp -o Stack\bin\stack.o $(CPPFLAGS)

Stack\bin\stack_logs.o: Stack\stack_logs.cpp
	g++ -c Stack\stack_logs.cpp -o Stack\bin\stack_logs.o $(CPPFLAGS)

Stack\bin\verification.o: Stack\stack_verification.cpp
	g++ -c Stack\stack_verification.cpp -o Stack\bin\verification.o $(CPPFLAGS)

Common\bin\file_reading.o: Common\file_reading.cpp Common\file_reading.h
	g++ -c Common\file_reading.cpp -o Common\bin\file_reading.o $(CPPFLAGS)

Common\bin\utils.o: Common\utils.cpp Common\utils.h
	g++ -c Common\utils.cpp -o Common\bin\utils.o $(CPPFLAGS)