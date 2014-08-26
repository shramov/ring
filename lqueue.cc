#include "lqueue.h"
#include <iostream>
#include <thread>

template <typename T>
::std::ostream& operator<<(::std::ostream& os, const std::pair<T, bool>& m) {
	if (!m.second)
		return os << "Nothing";
	else
		return os << "Just " << m.first;
}

void push(lqueue<int>* lq, int count)
{
	for (int i = 0; i < count; i++)
		lq->push(i);
}

void pop(lqueue<int>* lq, int count)
{
	while (count) {
		if (lq->pop().second)
			--count;
	}
}

int main()
{
	lqueue<int> lq;

	std::thread t0(push, &lq, 1000);
	std::thread t1(push, &lq, 1000);
	std::thread t2(push, &lq, 1000);
	std::thread c0(pop, &lq, 1000);
	std::thread c1(pop, &lq, 1000);
	std::thread c2(pop, &lq, 1000);
	t0.join();
	t1.join();
	t2.join();
	c0.join();
	c1.join();
	c2.join();
}
