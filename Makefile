cc = g++

all: serverM serverA serverB serverC client monitor

serverM: serverM.cpp
	$(cc) -o serverM serverM.cpp

serverA: serverA.cpp
	$(cc) -o serverA serverA.cpp

serverB: serverB.cpp
	$(cc) -o serverB serverB.cpp

serverC: serverC.cpp
	$(cc) -o serverC serverC.cpp

client: client.cpp
	$(cc) -o client client.cpp

monitor: monitor.cpp
	$(cc) -o monitor monitor.cpp

clean:
	$(RM) serverM serverA serverB serverC client monitor