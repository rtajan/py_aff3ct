#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <aff3ct.hpp>

#include "Module/Py_Module/py_module.hpp"

#include <pybind11/embed.h>
#include "numpy/arrayobject.h"
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"

namespace py = pybind11;

using namespace aff3ct;
using namespace aff3ct::module;

int main(int argc, char** argv)
{
	py::scoped_interpreter guard{}; // start the interpreter and keep it alive
	py::module sys = py::module::import("sys");
	// sys.attr("path").attr("append")("/Users/rtajan/Documents/Codes/mpwa/examples/tasks/src/");

	// py::gil_scoped_acquire acquire;  for multi thread
	// py::gil_scoped_acquire release;  for multi thread
	py::object py_module = py::module::import("py_module").attr("My_Module")(10);
	std::unique_ptr<Py_Module<     >> adder    (new Py_Module<     >(py_module));
	std::unique_ptr<Py_Module<float>> printer1 (new Py_Module<float>(py_module));
	std::unique_ptr<Py_Module<int  >> printer2 (new Py_Module<int  >(py_module));
	std::unique_ptr<Source_random< >> source   (new Source_random< >(10       ));

	adder->tasks[1]->sockets[0]->bind((*source)[src::sck::generate::U_K ]);
	printer2->tasks[0]->sockets[0]->bind(*(adder->tasks[1]->sockets[1]));
	printer1->tasks[0]->sockets[0]->bind(*(adder->tasks[1]->sockets[2]));

	(*source)[src::tsk::generate].exec();
	adder->tasks[1]->exec();
	printer1->tasks[0]->exec();
	printer2->tasks[0]->exec();

	return 0;

}
