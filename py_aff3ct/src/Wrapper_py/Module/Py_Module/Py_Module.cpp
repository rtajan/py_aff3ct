#include <pybind11/stl.h>
#include <aff3ct.hpp>
#include <string>
#include <sstream>

#include "src/Wrapper_py/Module/Py_Module/Py_Module.hpp"

namespace py = pybind11;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Py_Module
::Wrapper_Py_Module(py::handle scope)
: Wrapper_py(),
  py::class_<Py_Module, Module, Py_Module_Trampoline>(scope, "Py_Module")
{
}

void Wrapper_Py_Module
::definitions()
{
	this->def(py::init<>());
	this->def(py::init<const Py_Module_Trampoline&>());
	this->def("clone",          &Py_Module::clone);
	this->def("__copy__",       &Py_Module::__copy__);
	this->def("__deepcopy__",   &Py_Module::__deepcopy__);
	this->def("__str__",        &Py_Module::to_string);
	this->def("__getitem__",  [](Py_Module& m, const std::string& s) { return &m[s];}, py::return_value_policy::reference);
	this->def("__call__",     [](Py_Module& m, const std::string& s) { return &m(s);}, py::return_value_policy::reference);

	this->def_property("n_frames", &Py_Module::get_n_frames, &Py_Module::set_n_frames);
	this->def_property("n_frames_per_wave", &Py_Module::get_n_frames_per_wave, &Py_Module::set_n_frames_per_wave);

};