#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/functional.h>
#include <functional>
#include <iostream>
#include <sstream>

#include "src/Wrapper_py/Module/Module.hpp"

std::string create_helper(const std::string& in_out, const std::string& type)
{
	std::stringstream message;
	message << "Create a new " << in_out << " socket for 'n_elt' elements of type '";
	message << type << "'.";
	return message.str();
};

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Module
::Wrapper_Module(py::handle scope)
: Wrapper_py(),
  py::class_<Module, Module_Publicist>(scope, "Module")
{
}

void Wrapper_Module
::definitions()
{
	this->def(py::init<>());
	this->def("create_task", [](Module_Publicist& self, const std::string &name)->Task&
	{
		return self.create_task(name, -1);
	}, "Create a new Task.", "name"_a, py::return_value_policy::reference);

	this->def("create_socket_in",    [](Module_Publicist& self, Task& task, const std::string& name, const size_t n_elts, py::object type)-> int
	{
		const std::string type_str (type.attr("__name__").cast<std::string>());
		int s = -1;
		if      (type_str == "int8"   ) s = self.create_socket_in<int8_t >(task, name, n_elts);
		else if (type_str == "int16"  ) s = self.create_socket_in<int16_t>(task, name, n_elts);
		else if (type_str == "int32"  ) s = self.create_socket_in<int32_t>(task, name, n_elts);
		else if (type_str == "int64"  ) s = self.create_socket_in<int64_t>(task, name, n_elts);
		else if (type_str == "float32") s = self.create_socket_in<float  >(task, name, n_elts);
		else if (type_str == "float64") s = self.create_socket_in<double >(task, name, n_elts);
		return s;
	}, "task"_a, "name"_a, "n_elts"_a, "type"_a);

	this->def("create_socket_out",    [](Module_Publicist& self, Task& task, const std::string& name, const size_t n_elts, py::object type)-> int
	{
		const std::string type_str (type.attr("__name__").cast<std::string>());
		int s = -1;
		if      (type_str == "int8"   ) s = self.create_socket_out<int8_t >(task, name, n_elts);
		else if (type_str == "int16"  ) s = self.create_socket_out<int16_t>(task, name, n_elts);
		else if (type_str == "int32"  ) s = self.create_socket_out<int32_t>(task, name, n_elts);
		else if (type_str == "int64"  ) s = self.create_socket_out<int64_t>(task, name, n_elts);
		else if (type_str == "float32") s = self.create_socket_out<float  >(task, name, n_elts);
		else if (type_str == "float64") s = self.create_socket_out<double >(task, name, n_elts);
		return s;
	}, "task"_a, "name"_a, "n_elts"_a, "type"_a);

	this->def("create_codelet", [](Module_Publicist& self, Task& task, std::function<int(Module &m, Task &t, const size_t frame_id)> codelet){
		self.create_codelet(task,[codelet](Module &m, Task &t, const size_t f)->int
		{
			py::gil_scoped_acquire acquire;
			return codelet(m,t,f);
		});
	}, "task"_a, "codelet"_a);

	this->def_property_readonly("tasks", [](Module& self) -> std::vector<std::shared_ptr<Task>> { return self.tasks; });
	this->def_property("n_frames", &Module::get_n_frames, &Module::set_n_frames);
};