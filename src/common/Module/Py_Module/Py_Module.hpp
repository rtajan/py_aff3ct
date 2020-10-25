/*!
 * \file
 * \brief Class module::Modem.
 */
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

namespace py = pybind11;

namespace aff3ct
{
namespace module
{

/*!
 * \class
 *
 * \brief
 *
 * Please use for inheritance (instead of)
 */
template <typename B = int, typename R = float, typename Q = R>
class Py_Module : public Module
{
protected:
	py::object py_mod;
	std::map<std::string, py::object> task_map;
	std::vector<std::vector<int>> sck_in;
	std::vector<std::vector<int>> sck_out;

public:
	/*!
	 * \brief Constructor.
	 */
	Py_Module(const py::object &Py_Module, const int n_frames=1);

	/*!
	 * \brief Destructor.
	 */
	virtual ~Py_Module() = default;

	template <typename T>
	static py::array_t<T> sck2py(void* data_ptr, size_t data_len);
};
}
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "Py_Module.hxx"
#endif

#endif /* PY_MODULE_HPP_ */
