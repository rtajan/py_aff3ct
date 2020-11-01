#include <string>
#include <sstream>
#include <algorithm>

#include "Module/Socket.hpp"

//#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "Py_Module.hpp"


//PYBIND11_EMBEDDED_MODULE(py_socket, m){
//	pybind11::class_<aff3ct::module::Socket>(m, "Socket", pybind11::buffer_protocol())
//	//.def(py::init<py::ssize_t, py::ssize_t>())
//	.def_buffer([](aff3ct::module::Socket &sck) -> pybind11::buffer_info {
//			return pybind11::buffer_info(
//				sck.get_dataptr(),                               /* Pointer to buffer */
//				sck.get_datatype_size(),                          /* Size of one scalar */
//				pybind11::format_descriptor<float>::format(), /* Python struct-style format descriptor */
//				1,                                      /* Number of dimensions */
//				{sck.get_n_elmts()},                 /* Buffer dimensions */
//				{sck.get_datatype_size()}
//			);
//		});
//}

namespace aff3ct
{
namespace module
{

Socket& Py_Module
::operator[](const std::string &tsk_sck)
{
	size_t pos = tsk_sck.find("::", 0);
	std::string tsk_name = tsk_sck.substr(0, pos);
	std::string sck_name = tsk_sck.substr(pos+2, tsk_sck.size());
	if ((int)pos < 0)
	{
		std::stringstream message;
		message << "Invalid socket name, it should be of the form task::socket.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	auto &cur_tsk = this->operator()(tsk_name);

	int s = -1;
	for (size_t i = 0; i < cur_tsk.sockets.size(); i++)
	{
		if (cur_tsk.sockets[i]->get_name() == sck_name)
		{
			s = i;
			break;
		}
	}

	if (s < 0)
	{
		std::stringstream message;
		message << "Socket '" << sck_name << "' not found for task '" << tsk_name << "'.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	return cur_tsk[(size_t)s];
}

Task& Py_Module
::operator()(const std::string &tsk_name)
{
	int t = -1;
	for (size_t i = 0; i < this->tasks.size(); i++)
	{
		if (this->tasks[i]->get_name() == tsk_name)
		{
			t = i;
			break;
		}
	}

	if (t < 0)
	{
		std::stringstream message;
		message << "Task '" << tsk_name << "' not found.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	return Module::operator[]((size_t)t);
}

Py_Module
::Py_Module(const pybind11::object &py_mod, const int n_frames)
: Module(n_frames), py_mod(py_mod), py_codelets(), sck_in(), sck_out()
{
	this->set_name      (py_mod.attr("name"      ).cast<std::string>());
	this->set_short_name(py_mod.attr("short_name").cast<std::string>());

	pybind11::list task_list = py_mod.attr("task_list");
	for (size_t t = 0; t < task_list.size(); t++)
	{
		auto task = task_list[t];

		std::string task_name(task.attr("name").cast<std::string>());
		this->py_codelets.push_back(task.attr("codelet"));

		auto &p = this->create_task(task_name);
		sck_in .push_back(std::vector<int>());
		sck_out.push_back(std::vector<int>());

		pybind11::list socket_in_list  = task.attr("socket_in_list");
		for (auto sck : socket_in_list)
		{
			const std::string sck_name  = sck.attr("name"     ).cast<std::string>();
			const int n_elmts           = sck.attr("n_elmts"  ).cast<int        >();
			const std::string data_type = sck.attr("data_type").cast<std::string>();

			size_t s_in = -1;
			if      (data_type == "int8"  ) s_in = this->template create_socket_in<int8_t >(p, sck_name, n_elmts);
			else if (data_type == "int16" ) s_in = this->template create_socket_in<int16_t>(p, sck_name, n_elmts);
			else if (data_type == "int32" ) s_in = this->template create_socket_in<int32_t>(p, sck_name, n_elmts);
			else if (data_type == "int64" ) s_in = this->template create_socket_in<int64_t>(p, sck_name, n_elmts);
			else if (data_type == "float" ) s_in = this->template create_socket_in<float  >(p, sck_name, n_elmts);
			else if (data_type == "double") s_in = this->template create_socket_in<double >(p, sck_name, n_elmts);
			else
			{
				std::stringstream message;
				message << "Unsupported data type.";
				throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
			}
			sck_in[t].push_back(s_in);
		}

		pybind11::list socket_out_list = task.attr("socket_out_list");
		for (auto sck : socket_out_list)
		{
			const std::string sck_name  = sck.attr("name"     ).cast<std::string>();
			const int n_elmts           = sck.attr("n_elmts"  ).cast<int        >();
			const std::string data_type = sck.attr("data_type").cast<std::string>();

			size_t s_out = -1;
			if      (data_type == "int8"  ) s_out = this->template create_socket_out<int8_t> (p, sck_name, n_elmts);
			else if (data_type == "int16" ) s_out = this->template create_socket_out<int16_t>(p, sck_name, n_elmts);
			else if (data_type == "int32" ) s_out = this->template create_socket_out<int32_t>(p, sck_name, n_elmts);
			else if (data_type == "int64" ) s_out = this->template create_socket_out<int64_t>(p, sck_name, n_elmts);
			else if (data_type == "float" ) s_out = this->template create_socket_out<float>  (p, sck_name, n_elmts);
			else if (data_type == "double") s_out = this->template create_socket_out<double> (p, sck_name, n_elmts);
			else
			{
				std::stringstream message;
				message << "Unsupported data type.";
				throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
			}
			sck_out[t].push_back(s_out);
		}

		this->create_codelet(p, [t](Module &m, Task &tsk) -> int
		{
			module::Py_Module& m_cast = static_cast<module::Py_Module&>(m);
			std::vector<int> &scks_in  = m_cast.sck_in [t];
			std::vector<int> &scks_out = m_cast.sck_out[t];

			pybind11::gil_scoped_acquire acquire;
			//auto d_load = std::chrono::nanoseconds(0);
			//auto d_decod = std::chrono::nanoseconds(0);

			//auto t_load = std::chrono::steady_clock::now(); // Uncomment to monitor load
			auto args = pybind11::tuple(scks_in.size()+scks_out.size());
			for (size_t i = 0; i < scks_in.size(); i++)
				args[i] = &tsk[scks_in[i]];

			for (size_t i = 0; i < scks_out.size(); i++)
				args[scks_in.size() + i] = &tsk[scks_out[i]];

			//d_load += std::chrono::steady_clock::now() - t_load;
			try
			{
				//auto t_decod = std::chrono::steady_clock::now(); // Uncomment to monitor processing
				// execute the Python code
				int return_value = m_cast.py_codelets[t](*args).cast<int>();

				//tsk.update_timer(0,   d_load);
				//tsk.update_timer(1,   d_decod);
				return return_value;
			}
			catch (const std::exception &e)
			{
				throw tools::runtime_error(__FILE__, __LINE__, __func__, e.what());
			}
		});

		this->register_timer(p, "load");
		this->register_timer(p, "decode");
	}
}

template <typename T>
pybind11::array_t<T> Py_Module
::sck2py(void* void_data_ptr, size_t data_len)
{
	T* data_ptr = static_cast<T*>(void_data_ptr);
	pybind11::array_t<T> py_array(data_len, data_ptr); // there is a copy here
	return py_array;
}


Py_Module* Py_Module
::clone() const
{
	pybind11::object py_mod_cpy;
	try
	{
		py_mod_cpy = pybind11::module::import("copy").attr("deepcopy")(this->py_mod);
	}
	catch(const std::exception& e)
	{
		std::cerr << rang::tag::error << "Python module clone fails : " << e.what() << std::endl;
		throw tools::runtime_error(__FILE__, __LINE__, __func__, "Cloning Python module fails.");
	}

	auto m = new Py_Module(py_mod_cpy, this->n_frames);
	m->deep_copy(*this);
	return m;
}

}
}