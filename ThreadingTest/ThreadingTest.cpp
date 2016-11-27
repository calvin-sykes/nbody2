// ThreadingTest.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <future>
#include <chrono>

// Constructing thread with reference to function

void threadFunc()
{
	while (rand() > 10); // delay
	std::cout << "Hi from thread!\n";
}

void ex1()
{
	std::thread th(&threadFunc);
	std::cout << "Hi from main!\n";
	th.join();
}

// Constructing thread with lambda function

void ex2()
{
	std::thread th([]()
	{
		while (rand() > 10); // delay
		std::cout << "Hi from thread!\n";
	});
	std::cout << "Hi from main!\n";
	th.join();
}

// Capturing variables into lambda function

void ex3()
{
	std::vector<std::thread> workers;
	for (int i = 0; i < 10; i++)
	{
		workers.push_back(std::thread([i]()
		{
			std::cout << "Hi from thread!" << i << "\n";
		}));
	}
	std::cout << "Hi from main!\n";
	std::for_each(workers.begin(), workers.end(), [](std::thread &th)
	{
		th.join();
	});
}

// GOTCHA! - references can expire if they are accessed after their target goes
// out of scope
// ==> Sometimes running this will print garbage
// additionally, threads are all accessing i while main loop is still incrementing it
// ==> race condition, may get duplicate values of i

void threadFunctionArg(int& i)
{
	std::cout << "Hi from worker " << i << "!\n";
}

void test(std::vector<std::thread>& workers)
{
	for (int i = 0; i < 10; i++)
	{
		auto th = std::thread(&threadFunctionArg, std::ref(i));
		workers.push_back(std::move(th));
	}
}

void ex4()
{
	std::vector<std::thread> workers;
	test(workers);
	std::cout << "Hi from main!\n";
	std::for_each(workers.begin(), workers.end(), [](std::thread &th)
	{
		th.join();
	});
}

struct List
{
	struct Node
	{
		int _x;
		Node* _next;

		Node(int y) : _x(y), _next(nullptr) {}
	};

	Node* _head;

	List() : _head(nullptr) {}

	void insert(int x)
	{
		auto node = new Node(x);
		node->_next = _head;
		_head = node;
	}

	int count() const;
};

int List::count() const
{
	int n = 0;
	auto cur = _head;
	while (cur != nullptr)
	{
		++n;
		cur = cur->_next;
	}
	return n;
}

void threadFunctionList(List& list)
{
	for (int i = 0; i < 100; i++)
		list.insert(i);
}

void ex5()
{
	List list;
	std::vector<std::thread> workers;
	for (int i = 0; i < 10; i++)
	{
		workers.push_back(std::thread(&threadFunctionList, std::ref(list)));
	}
	std::for_each(workers.begin(), workers.end(), [](std::thread &th)
	{
		th.join();
	});
	int total = list.count();
}

// Returning values from threads
// (promises and futures)

void threadFuncFuture(std::promise<std::string>&& prms)
{
	try
	{
		std::string str("Hello from future!\n");
		throw(std::exception("Exception from future!\n"));
		prms.set_value(str);
	}
	catch (...)
	{
		prms.set_exception(std::current_exception());
	}
}

void ex6()
{
	std::promise<std::string> prms;
	std::future<std::string> ftr = prms.get_future();
	std::thread th(&threadFuncFuture, std::move(prms));
	std::cout << "Hello from main!\n";
	try
	{
		std::string str = ftr.get();
		std::cout << str << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	th.join();
}

// std::async avoids boilerplate of promises + futures

std::string asyncFunc()
{
	std::string str("Hello from future!\n");
	throw(std::exception("Exception from async!\n"));
	return str;
}

void ex7()
{
	auto ftr = std::async(&asyncFunc);
	std::cout << "Hello from main!\n";
	try
	{
		std::string str = ftr.get();
		std::cout << str << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
}

// tasks that return void have termination enforced by future destructor

void asyncFuncVoid()
{
	std::cout << "Starting task!\n";
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Ending task!\n";
}

void ex8()
{
	auto ftr = std::async(&asyncFuncVoid);
	std::cout << "Exiting main!\n";
}

int main()
{
	ex8();
	return 0;
}