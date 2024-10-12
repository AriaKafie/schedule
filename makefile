all:
	g++ *.cpp -oschedule
clean:
	rm *~ schedule
run:
	g++ *.cpp -oschedule && ./schedule
