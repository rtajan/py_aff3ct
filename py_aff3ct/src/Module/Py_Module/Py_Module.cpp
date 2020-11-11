#include <sstream>
#include <algorithm>
#include <exception>
#include <memory>

#include <pybind11/stl.h>

#include "Py_Module.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
namespace py = pybind11;

Py_Module
::Py_Module()
: Module()
{
	const std::string name = "Py_Module";
	this->set_name(name);
}

py::object Py_Module
::__copy__() const
{
	// Cast it to py::object, this should work if the Module is a Py_Module
	py::object py_module = py::cast(*this);
	// Recover the python type
	py::object      type = py_module.attr("__class__");
	// create a new object without initializing it
	py::object     copy_ = py_module.attr("__new__")(type);
	// clone C++ state using copy constructor of Py_Module (copy Tasks, Sockets ...)
	py::module::import("py_aff3ct").attr("Py_Module").attr("__init__")(copy_, py_module);
	// clone Python state
	try
	{
		copy_.attr("__dict__").attr("update")(py_module.attr("__dict__"));
	}
	catch(const std::exception& e)
	{
	}

	return copy_;
}

py::object Py_Module
::__deepcopy__() const
{
	py::object py_copy = this->__copy__();
	Py_Module& copy = py::cast<Py_Module&>(py_copy);
	copy.deep_copy(*this); // Rebuild C++ state with new Tasks / Sockets
	return py_copy;
}

Py_Module* Py_Module
::clone() const
{
	// https://github.com/pybind/pybind11/issues/1049
	py::object* py_clone = new py::object(this->__deepcopy__());
	// Create new Py_Module from a casted version of py_clone (a cpp_clone)
	auto cpp_clone = new  Py_Module(py::cast<Py_Module&>(*py_clone));
	cpp_clone->child.reset(py_clone);
	return cpp_clone;
}

std::string Py_Module
::to_string() const
{
	py::object self = py::cast(*this);
	std::stringstream message;
	message << "----------------------------- Module -----------------------------" << std::endl;
	message << "- Name         : " << this->get_name()         << "\n";
	message << "- Address      : " <<  std::hex << static_cast<const void*>(this) << "\n";
	message << "- Class        : " << self.attr("__class__").attr("__name__").cast<std::string>() << "\n";
	message << "- Frames number: " << this->get_n_frames()     << "\n";
	message << "- Task number  : " << this->tasks.size()       << "\n";

	if (this->tasks.size() > 0)
		message << "- Tasks        :" << "\n";
	for (size_t i = 0 ; i < this->tasks.size() ; i++)
	{
		message << "\t- Task " << i << "\n";
		std::string name = this->tasks[i]->get_name();
		message << "\t\t- Name          : " << name << "\n";
		message << "\t\t- Address       : " <<  std::hex << static_cast<void*>(this->tasks[i].get()) << "\n";
		message << "\t\t- Module address: " <<  std::hex << static_cast<void*>(&this->tasks[i]->get_module()) << "\n";
		message << "\t\t- Socket number : " << this->tasks[i]->sockets.size() << "\n";
		if (this->tasks[i]->sockets.size() > 0)
			message << "\t\t- Sockets      : " << "\n";
		for (size_t j = 0 ; j < this->tasks[i]->sockets.size() ; j++)
		{
			message << "\t\t\t- Socket " << j << "\n";
			std::string type;
			if (this->tasks[i]->get_socket_type(*this->tasks[i]->sockets[j]) == socket_t::SIN )
				type = "in";
			else if (this->tasks[i]->get_socket_type(*this->tasks[i]->sockets[j]) == socket_t::SOUT )
				type = "out";

			message << "\t\t\t\t- Type              : " << type.c_str() << "\n";
			message << "\t\t\t\t- Name              : " << this->tasks[i]->sockets[j]->get_name() << "\n";
			message << "\t\t\t\t- Elements per frame: " << this->tasks[i]->sockets[j]->get_n_elmts()/this->get_n_frames() << "\n";
			message << "\t\t\t\t- Data type         : " << this->tasks[i]->sockets[j]->get_datatype_string() << "\n";
			message << "\t\t\t\t- Adress            : " << std::hex << static_cast<void*>(this->tasks[i]->sockets[j].get()) << "\n\n";
		}
	}


	return message.str();
}