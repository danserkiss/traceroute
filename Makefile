all:
	g++ main.cpp -o my_traceroute 
install:
	sudo cp ./my_traceroute /usr/local/bin