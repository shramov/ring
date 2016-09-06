#ifndef __LQUEUE_H__
#define __LQUEUE_H__

/*
 * Copyright (c) 2014 Pavel Shramov <shramov@mexmat.net>
 *
 * lqueue is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <memory>
#include <atomic>

/*
 * XXX: Probably there are some races...
 */
template <typename T>
class lqueue
{
	struct node {
		node * next;
		T value;

		node() : next(0), value() {}
	};

	std::atomic<node *> _head;
	std::atomic<node *> _tail;

public:
	lqueue() : _head(new node), _tail(_head.load()) {}	
	~lqueue()
	{
		for (auto ptr = _head.load(); ptr;)
		{
			std::unique_ptr<node> p(ptr);
			ptr = ptr->next;
		}
	}

	// Passing by value to have fast push(T &&)
	void push(T value)
	{
		auto e = new node;
		do {
			auto p = _tail.load();
			if (!_tail.compare_exchange_weak(p, e))
				continue;
			std::swap(p->value, value);
			//XXX: Write barrier is needed here
			std::atomic_thread_fence(std::memory_order_release);
			p->next = e;
			return;
		} while (1);
	}

	std::pair<T, bool> pop()
	{
		do {
			auto p = _head.load();
			if (!p->next) return std::make_pair(T(), false);
			if (!_head.compare_exchange_weak(p, p->next))
				continue;
			std::unique_ptr<node> ptr(p);
			return std::make_pair(p->value, true);
		} while (1);
	}
};

#endif//__LQUEUE_H__
