#include <chrono>
#include <thread>
#include <iostream>

#include "Server.h"
#include "TaskConsumerThread.h"

using namespace rte;

int main(int argc, char* argv[])
{
	Server s;
	s.connect("localhost", 0x1234);

	TaskConsumerThread<int> th;
	th.start([](int* i)
	{
		std::cout << "start " << *i << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "end " << *i << std::endl;
	});

	int t[3] = { 1, 1, 1 };
	for (int i = 0; i < 3; ++i)
	{
		std::cout << "add 1" << std::endl;
		th.addTask(&t[i]);
	}

	int tt[2] = { 2, 2 };
	for (int i = 0; i < 2; ++i)
	{
		std::cout << "add 2" << std::endl;
		th.addTask(&tt[i]);
	}

	th.stop();
	th.join();

	int ttt[2] = { 3 };
	for (int i = 0; i < 1; ++i)
	{
		std::cout << "add 3" << std::endl;
		th.addTask(&ttt[i]);
	}

	return 0;
}
