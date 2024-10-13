all:
	g++ -fpermissive -std=c++17 *.cpp -oschedule
clean:
	rm *~ schedule
run:
	g++ -fpermissive -std=c++17 *.cpp -oschedule && ./schedule
