TARGET = Interpretator\build\assembler.exe
CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla

.PHONY: all
all: $(TARGET)

$(TARGET): Interpretator\bin\ass_main.o Interpretator\bin\ass_inout.o Interpretator\bin\assemblr.o Stack\bin\stack.o Stack\bin\stack_logs.o Stack\bin\verification.o
	g++ Interpretator\bin\ass_main.o Interpretator\bin\ass_inout.o Interpretator\bin\assemblr.o Stack\bin\stack.o Stack\bin\stack_logs.o Stack\bin\verification.o -o $(TARGET) $(CPPFLAGS)

Interpretator\bin\ass_main.o: Interpretator\ass_main.cpp
	g++ -c Interpretator\ass_main.cpp -o Interpretator\bin\ass_main.o $(CPPFLAGS)

Interpretator\bin\ass_inout.o: Interpretator\ass_inout.cpp
	g++ -c Interpretator\ass_inout.cpp -o Interpretator\bin\ass_inout.o $(CPPFLAGS)

Interpretator\bin\assemblr.o: Interpretator\assemblr.cpp
	g++ -c Interpretator\assemblr.cpp -o Interpretator\bin\assemblr.o $(CPPFLAGS)

Stack\bin\stack.o: Stack\stack.cpp
	g++ -c Stack\stack.cpp -o Stack\bin\stack.o $(CPPFLAGS)

Stack\bin\stack_logs.o: Stack\stack_logs.cpp
	g++ -c Stack\stack_logs.cpp -o Stack\bin\stack_logs.o $(CPPFLAGS)

Stack\bin\verification.o: Stack\stack_verification.cpp
	g++ -c Stack\stack_verification.cpp -o Stack\bin\verification.o $(CPPFLAGS)