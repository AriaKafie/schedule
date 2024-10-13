all:
	g++ -w -fpermissive -std=c++17 *.cpp -oschedule
clean:
	rm *~ schedule
run:
	g++ -w -fpermissive -std=c++17 *.cpp -oschedule && ./schedule
