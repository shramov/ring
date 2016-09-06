#ifndef __MARKER_QUEUE_H__
#define __MARKER_QUEUE_H__

/*
 * Copyright (c) 2016 Pavel Shramov <shramov@mexmat.net>
 *
 * lqueue is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <vector>
#include <atomic>
#include <cstddef>
#include <cerrno>

/**
 * Multiple Input - Single Output queue for simple types.
 * Type must have one designated Zero value (by default 0)
 * that can not be stored and is used as empty cell marker.
 *
 * Useless if std::is_atomic_type<std::atomic<T>> == false
 */
template <typename T = intptr_t, T Zero = 0>
class MarkerQueue {
	std::vector<std::atomic<T> > _ring;

	int _head __attribute__((aligned(64))) = 0;
	std::atomic<int> _tail __attribute__((aligned(64))) = {0};

	int _next(int i)
	{
		return (i + 1) % _ring.size();
	}
public:
	static const T zero = Zero;

	MarkerQueue(size_t size) : _ring(size) {}

	/** Store new value
	 * Value must not be Zero (0 by default)
	 *
	 * @return 0 on success, EAGAIN when there is no space available
	 */
	int push(const T& data)
	{
		do {
			auto t = _tail.load(std::memory_order_consume);
			auto next = _next(t);
			if (next == _head) return EAGAIN;
			T v = zero;
			if (_ring[t].compare_exchange_weak(v, data, std::memory_order_release)) {
				if (_tail.load(std::memory_order_consume) != t) {
					/*
					 * Possible race:
					 *
					 * w0: load tail
					 * w1: load tail, swap value, shift tail
					 * r0: read value, store 0
					 * w0: swap value
					 *
					 */
					_ring[t].store(zero, std::memory_order_release);
					continue;
				}
				_tail.store(next, std::memory_order_release);
				return 0;
			}
		} while (true);
	}

	T pop()
	{
		if (_tail == _head) return 0;
		T r = _ring[_head].load(std::memory_order_consume);
		_ring[_head].store(zero, std::memory_order_release);
		_head = _next(_head);
		return r;
	}

	void clear()
	{
		_head = 0;
		_tail = 0;
		for (auto & i : _ring)
			i = zero;
	}
};

#endif//__MARKER_QUEUE_H__
