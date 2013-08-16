/*
 * Copyright (c) 2013 Pavel Shramov <shramov@mexmat.net>
 *
 * ring is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <boost/python.hpp>

#include "ring.h"

namespace py = boost::python;

inline py::object pass_through(py::object const& o) { return o; }

class RingIter;

class Ring : public ringbuffer_t {
public:
	Ring(size_t size) { ring_init(this, size, 0); }
	Ring(void* ptr) { ring_init(this, 0, ptr); }

	ring_size_t next_size() { return ring_next_size(this); }
	const void* next() { return ring_next(this); }

	int write(char *buf, size_t size) { return ring_write(this, buf, size); }
	int shift() { return ring_shift(this); }
	size_t available() const { return ring_available(this); }

	py::object read_buffer() const
	{
		const void *data;
		size_t size;

		if (ring_read(this, &data, &size))
			return py::object();
		py::handle<> h(PyBuffer_FromMemory ((void *) data, size));
		return py::object(h);
	}

	RingIter __iter__() const;
};

class RingIter : public ringiter_t {
	const Ring &_ring;
public:
	RingIter(const Ring &ring) : _ring(ring) { ring_iter_init(&ring, this); }
	int shift() {
		int r = ring_iter_shift(this);
		if (r == EINVAL)
			throw std::out_of_range("Iteratos is out of date");
		return r;
	}
	py::object read_buffer() const
	{
		const void *data;
		size_t size;

		int r = ring_iter_read(this, &data, &size);
		if (r) {
			if (r == EAGAIN) return py::object();
			throw std::out_of_range("Iteratos is out of date");
		}
		py::handle<> h(PyBuffer_FromMemory ((void *) data, size));
		return py::object(h);
	}

	py::object next()
	{
		py::object buf = read_buffer();
		if (buf.ptr() == py::object().ptr()) {
			PyErr_SetString(PyExc_StopIteration, "No more data.");
			boost::python::throw_error_already_set();
		}
		shift();
		return buf;
	}
};

inline RingIter Ring::__iter__() const { return RingIter(*this); }

BOOST_PYTHON_MODULE(ring) {
	using namespace boost::python;

	scope().attr("__doc__") = "Simple ringbuffer implementation\n";

	class_<Ring>("Ring", init<size_t>())
		.def("next", &Ring::read_buffer)
		.def("next_size", &Ring::next_size)
		.def("read", &Ring::read_buffer)
		.def("available", &Ring::available)
		.def("shift", &Ring::shift)
		.def("write", &Ring::write)
		.def("__iter__", &Ring::__iter__);
	class_<RingIter>("RingIter", init<const Ring &>())
		.def("shift", &RingIter::shift)
		.def("read", &RingIter::read_buffer)
		.def("__iter__", &pass_through)
		.def("next", &RingIter::next);
}
