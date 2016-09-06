#include "markerqueue.h"

#include <thread>
#include <chrono>

using namespace std::chrono;

void writer(MarkerQueue<long long> * q, long long idx)
{
	while (true) {
		q->push(idx);
	}
}

int main()
{
	MarkerQueue<long long> v(1024);
	std::vector<std::thread> writers;
	for (int i = 0; i < 4; i++)
		writers.push_back(std::thread(writer, &v, 100 + i));

	const long long limit = 10000000;
	long long counter = 0;
	auto start = system_clock::now();
	while (true) {
		if (v.pop() != 0) {
			if (counter++ == limit) {
				auto now = system_clock::now();
				auto dt = now - start;
				printf("Got 1e7 messages: %.3fs, %lldns/m\n", duration_cast<duration<double> >(dt).count(), dt.count() / limit);
				start = now;
				counter = 0;
			}
		}
	}
}
