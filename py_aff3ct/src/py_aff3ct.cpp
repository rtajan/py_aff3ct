#include <pybind11/pybind11.h>
#include <aff3ct.hpp>
#include <memory>

#include "Wrapper_py/Wrapper_py.hpp"
#include "Wrapper_py/Module/Module.hpp"
#include "Wrapper_py/Module/Socket.hpp"
#include "Wrapper_py/Module/Task.hpp"
#include "Wrapper_py/Module/Py_Module/Py_Module.hpp"

PYBIND11_MODULE(py_aff3ct, m){
	// Split in two following https://pybind11.readthedocs.io/en/stable/advanced/misc.html#avoiding-c-types-in-docstrings
	// for enhancing python doc
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_socket   (new aff3ct::wrapper::Wrapper_Socket   (m));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_task     (new aff3ct::wrapper::Wrapper_Task     (m));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_module   (new aff3ct::wrapper::Wrapper_Module   (m));
	std::unique_ptr<aff3ct::wrapper::Wrapper_py> wrapper_py_module(new aff3ct::wrapper::Wrapper_Py_Module(m));

	//wrapper_socket   .definitions();
	//wrapper_task     .definitions();
	//wrapper_module   .definitions();
	//wrapper_py_module.definitions();

	std::vector<aff3ct::wrapper::Wrapper_py*> wrappers = {wrapper_socket.get(), wrapper_task.get(), wrapper_module.get(), wrapper_py_module.get()};
	for (size_t i = 0; i < wrappers.size(); i++)
		wrappers[i]->definitions();
}