#include <string>
#include <sstream>
#include <algorithm>
#include <Python.h>
#include "Py_Module.hpp"
#include<pybind11/pybind11.h>
#include<pybind11/numpy.h>

namespace aff3ct
{
namespace module
{


template <typename B, typename R, typename Q>
Socket& Py_Module<B,R,Q>
::operator[](const std::string tsk_sck)
{
	size_t pos = tsk_sck.find("::", 0);
	std::string tsk_name = tsk_sck.substr (0,pos);
	std::string sck_name = tsk_sck.substr (pos+2,tsk_sck.size());
	if ((int)pos < 0)
	{
		std::stringstream message;
		message << "Invalid socket name, it should be of the form task::socket.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	int t = -1;
	for (auto i = 0; i < this->tasks.size(); i++)
	{
		if (this->tasks[i]->get_name() == tsk_name)
		{
			t = i;
			break;
		}
	}

	if(t < 0)
	{
		std::stringstream message;
		message << "Task '" << tsk_name << "' not found.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	int s = -1;
	for (auto i = 0; i < this->tasks[t]->sockets.size(); i++)
	{
		if (this->tasks[t]->sockets[i]->get_name() == sck_name)
		{
			s = i;
			break;
		}
	}
	if(s < 0)
	{
		std::stringstream message;
		message << "Socket '" << sck_name << "' not found for task '" << tsk_name <<"'.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}
	return Module::operator[]((size_t)t)[(size_t)s];
}

template <typename B, typename R, typename Q>
Task& Py_Module<B,R,Q>
::operator()(const std::string tsk_name)
{
	int t = -1;
	for (auto i = 0; i < this->tasks.size(); i++)
	{
		if (this->tasks[i]->get_name() == tsk_name)
		{
			t = i;
			break;
		}
	}

	if(t < 0)
	{
		std::stringstream message;
		message << "Task '" << tsk_name << "' not found.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}
	return Module::operator[]((size_t)t);
}

template <typename B, typename R, typename Q>
Py_Module<B,R,Q>
::Py_Module(const py::object &py_mod, const int n_frames)
: Module(n_frames), py_mod(py_mod), task_map(), sck_in(), sck_out()
{
	this->set_name      (py_mod.attr("name").cast<std::string>());
	this->set_short_name(py_mod.attr("short_name").cast<std::string>());

	py::list task_list = py_mod.attr("task_list");


	for (size_t t = 0; t < task_list.size(); t++)
	{
		auto task = task_list[t];

		std::string task_name(task.attr("name").cast<std::string>());
		this->task_map[task_name] = task;

		auto &p = this->create_task(task_name);
		sck_in.push_back (std::vector<int>());
		sck_out.push_back(std::vector<int>());

		py::list socket_in_list  = task.attr("socket_in_list");
		for (auto sck : socket_in_list)
		{
			std::string sck_name  = sck.attr("name").cast<std::string>();
			int n_elmts           = sck.attr("n_elmts").cast<int>();
			std::string data_type = sck.attr("data_type").cast<std::string>();

			size_t s_in = -1;
			if (data_type == "B")
				s_in = this->template create_socket_in<B >(p, sck_name, n_elmts);
			else if (data_type == "R")
				s_in = this->template create_socket_in<R>(p, sck_name, n_elmts);
			else if (data_type == "Q")
				s_in = this->template create_socket_in<Q>(p, sck_name, n_elmts);
			else
			{
				std::stringstream message;
				message << "Unsupported data type.";
				throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
			}
			sck_in[t].push_back(s_in);
		}

		py::list socket_out_list  = task.attr("socket_out_list");
		for (auto sck : socket_out_list)
		{
			std::string sck_name = sck.attr("name").cast<std::string>();
			int n_elmts          = sck.attr("n_elmts").cast<int>();
			std::string data_type = sck.attr("data_type").cast<std::string>();

			size_t s_out = -1;
			if (data_type == "B")
				s_out = this->template create_socket_out<B >(p, sck_name, n_elmts);
			else if (data_type == "Q")
				s_out = this->template create_socket_out<Q>(p, sck_name, n_elmts);
			else if (data_type == "R")
				s_out = this->template create_socket_out<R>(p, sck_name, n_elmts);
			else
			{
				std::stringstream message;
				message << "Unsupported data type.";
				throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
			}
			sck_out[t].push_back(s_out);
		}

		auto p_in  = sck_in[t];
		auto p_out = sck_out[t];

		this->create_codelet(p, [p_in, p_out](Module &m, Task &t) -> int
		{
			py::gil_scoped_acquire acquire;
			//auto t_load = std::chrono::steady_clock::now(); // Uncomment to monitor load
			auto args = py::tuple(p_in.size());
			for (size_t i = 0; i<p_in.size(); i++)
			{
				auto s = t[p_in[i]];
				auto T = s.get_datatype();
				size_t elt_nbr  = s.get_n_elmts(); // / m.get_n_frames()

				if      (T == typeid(int8_t )) args[i] = Py_Module<>::sck2py<int8_t >(s.get_dataptr(), elt_nbr);
				else if (T == typeid(int16_t)) args[i] = Py_Module<>::sck2py<int16_t>(s.get_dataptr(), elt_nbr);
				else if (T == typeid(int32_t)) args[i] = Py_Module<>::sck2py<int32_t>(s.get_dataptr(), elt_nbr);
				else if (T == typeid(int64_t)) args[i] = Py_Module<>::sck2py<int64_t>(s.get_dataptr(), elt_nbr);
				else if (T == typeid(float  )) args[i] = Py_Module<>::sck2py<float  >(s.get_dataptr(), elt_nbr);
				else if (T == typeid(double )) args[i] = Py_Module<>::sck2py<double >(s.get_dataptr(), elt_nbr);
			}
			//auto d_load = std::chrono::steady_clock::now() - t_load;

			//auto t_decod = std::chrono::steady_clock::now(); // Uncomment to monitor processing
			py::object py_mod = static_cast<module::Py_Module<B,Q,R>&>(m).py_mod;
			py::list result = py_mod.attr(t.get_name().c_str())(*args);
			//auto d_decod = std::chrono::steady_clock::now() - t_decod;

			//auto t_store = std::chrono::steady_clock::now(); // Uncomment to monitor store
			for (size_t i = 0; i<p_out.size(); i++)
			{
				auto s = t[p_out[i]];
				auto T = s.get_datatype();
				size_t byte_nbr  = s.get_databytes(); // / m.get_n_frames()

				py::array res = py::array::ensure(result[i]);

				const int8_t* py_ptr = static_cast<const int8_t*>(res.data());
				int8_t*        s_ptr = static_cast<      int8_t*>(s.get_dataptr());
				std::copy(py_ptr, py_ptr + byte_nbr, s_ptr);
			}
			//auto d_store = std::chrono::steady_clock::now() - t_store;

			//t.update_timer(0,   d_load);
			//t.update_timer(1,   d_decod);
			//t.update_timer(2,   d_store);

			return 0;
		});
	this->register_timer(p, "load");
	this->register_timer(p, "decode");
	this->register_timer(p, "store");
	}
}

template <>
template <typename T>
py::array_t<T> Py_Module<>
::sck2py(void* void_data_ptr, size_t data_len)
{
	T* data_ptr = static_cast<T*>(void_data_ptr);
	py::array_t<T> py_array(data_len, data_ptr); // there is a copy here
	return(py_array);
}


template <typename B, typename R, typename Q>
Py_Module<B,R,Q>* Py_Module<B,R,Q>
::clone() const
{
	auto m = new Py_Module<B,R,Q>(*this);
	m->deep_copy(*this);
	return m;
}

}
}