#include <sstream>
#include <algorithm>
#include <exception>

#include "Py_Module.hpp"

namespace py = pybind11;

namespace aff3ct
{
namespace module
{
Socket& Py_Module
::operator[](const std::string &tsk_sck)
{
	size_t pos = tsk_sck.find("::", 0);
	if ((int)pos < 0)
	{
		std::stringstream message;
		message << "Invalid socket name, it should be of the form task::socket.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}
	std::string tsk_name = tsk_sck.substr(0, pos);
	tsk_name.erase(remove(tsk_name.begin(), tsk_name.end(), ' '), tsk_name.end());
	std::string sck_name = tsk_sck.substr(pos+2, tsk_sck.size());
	sck_name.erase(remove(sck_name.begin(), sck_name.end(), ' '), sck_name.end());
	auto &cur_tsk = this->operator()(tsk_name);

	auto it = find_if(cur_tsk.sockets.begin(), cur_tsk.sockets.end(),
	                  [sck_name](std::shared_ptr<Socket> s){return s->get_name()==sck_name;});

	if (it == cur_tsk.sockets.end())
	{
		std::stringstream message;
		message << "Socket '" << sck_name << "' not found for task '" << tsk_name << "'.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	return *it->get();
}

Task& Py_Module
::operator()(const std::string &tsk_name)
{
	auto it = find_if(this->tasks.begin(), this->tasks.end(),
					  [tsk_name](std::shared_ptr<Task> t){return t->get_name()==tsk_name;});

	if (it == this->tasks.end())
	{
		std::stringstream message;
		message << "Task '" << tsk_name << "' not found.";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	return *it->get();
}
}
}