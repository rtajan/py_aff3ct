#include <pybind11/stl.h>
#include "src/Wrapper_py/Module/Task.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Task
::Wrapper_Task(py::handle scope)
: Wrapper_py(),
  py::class_<Task, std::shared_ptr<Task>, Task_Publicist>(scope, "Task")
{
}

void Wrapper_Task
::definitions()
{
	this->def_property_readonly("name",    &aff3ct::module::Task::get_name);
	this->def_property_readonly("sockets", [](aff3ct::module::Task& t) -> std::vector<std::shared_ptr<aff3ct::module::Socket>>
	{
		return t.sockets;
	});
	this->def("exec",
		[](Task &self, const int frame_id, const bool managed_memory){ self.exec(frame_id, managed_memory);},
		"frame_id"_a = -1, "managed_memory"_a = true);
	this->def_property_readonly("tasks", [](aff3ct::module::Task_Publicist& self) { return self.codelet; });
};