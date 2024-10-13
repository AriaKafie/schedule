all:
	g++ -w -fpermissive -std=c++17 *.cpp -oschedule
debug:
	g++ -w -fpermissive -std=c++17 -g *.cpp -odebug && gdb ./debug
clean:
	rm *~ schedule debug
run:
	g++ -w -fpermissive -std=c++17 *.cpp -oschedule && ./schedule
