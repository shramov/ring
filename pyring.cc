#include <boost/python.hpp>

#include "ring.h"

namespace py = boost::python;

class Ring : public ringbuffer_t {
public:
	Ring(size_t size) { ring_init(this, size, 0); }
	Ring(void* ptr) { ring_init(this, 0, ptr); }

	ring_size_t next_size() { return ring_next_size(this); }
	void* next() { return ring_next(this); }

	int write(char *buf, size_t size) { return ring_write(this, buf, size); }
	void shift() { return ring_shift(this); }

	py::object next_buffer()
	{
		ring_size_t size = next_size();
		if (size < 0)
			return py::object();
		py::handle<> h(PyBuffer_FromMemory (next(), size));
		return py::object(h);
	}
};

BOOST_PYTHON_MODULE(ring) {
	using namespace boost::python;

	scope().attr("__doc__") = "Simple ringbuffer implementation\n";

	class_<Ring>("Ring", init<size_t>())
		.def("next", &Ring::next_buffer)
		.def("next_size", &Ring::next_size)
		.def("shift", &Ring::shift)
		.def("write", &Ring::write);
}
