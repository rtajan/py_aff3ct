#ifndef PY_MODULE_HPP_
#define PY_MODULE_HPP_

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

#include "Module/Task.hpp"
#include "Module/Socket.hpp"
#include "Module/Module.hpp"

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace aff3ct
{
namespace module
{
class Py_Module : public Module
{
protected:
	pybind11::object py_mod;
	std::vector<pybind11::object> py_codelets;
	std::vector<std::vector<int>> sck_in;
	std::vector<std::vector<int>> sck_out;

public:
	inline Socket& operator[](const std::string &tsk_sck );
	inline Task&   operator()(const std::string &tsk_name);

	Py_Module(const pybind11::object &Py_Module, const int n_frames=1);

	virtual ~Py_Module() = default;

	Py_Module* clone() const;

private:
	template <typename T>
	static pybind11::array_t<T> sck2py(void* data_ptr, size_t data_len);
};
}
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "Py_Module.hxx"
#endif

#endif /* PY_MODULE_HPP_ */
