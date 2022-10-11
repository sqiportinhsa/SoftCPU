TARGET = Processor\build\processor.exe
CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla

.PHONY: all
all: $(TARGET)

$(TARGET): Processor\bin\processor_main.o Processor\bin\processor.o Stack\bin\stack.o Stack\bin\stack_logs.o Stack\bin\verification.o Common\bin\file_reading.o Common\bin\utils.o
	g++ Processor\bin\processor_main.o Processor\bin\processor.o Stack\bin\stack.o Stack\bin\stack_logs.o Stack\bin\verification.o Common\bin\file_reading.o Common\bin\utils.o -o $(TARGET) $(CPPFLAGS)

Processor\bin\processor_main.o: Processor\processor_main.cpp Processor\processor.h Common\file_reading.h
	g++ -c Processor\processor_main.cpp -o Processor\bin\processor_main.o $(CPPFLAGS)

Processor\bin\processor.o: Processor\processor.cpp Processor\processor.h
	g++ -c Processor\processor.cpp -o Processor\bin\processor.o $(CPPFLAGS)

Stack\bin\stack.o: Stack\stack.cpp Stack\stack.h
	g++ -c Stack\stack.cpp -o Stack\bin\stack.o $(CPPFLAGS)

Stack\bin\stack_logs.o: Stack\stack_logs.cpp Stack\stack.h Stack\stack_logs.h
	g++ -c Stack\stack_logs.cpp -o Stack\bin\stack_logs.o $(CPPFLAGS)

Stack\bin\verification.o: Stack\stack_verification.cpp Stack\stack.h Stack\stack_verification.h
	g++ -c Stack\stack_verification.cpp -o Stack\bin\verification.o $(CPPFLAGS)

Common\bin\file_reading.o: Common\file_reading.cpp Common\file_reading.h
	g++ -c Common\file_reading.cpp -o Common\bin\file_reading.o $(CPPFLAGS)

Common\bin\utils.o: Common\utils.cpp Common\utils.h
	g++ -c Common\utils.cpp -o Common\bin\utils.o $(CPPFLAGS)