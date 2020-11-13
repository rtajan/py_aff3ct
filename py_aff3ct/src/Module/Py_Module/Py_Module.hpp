#ifndef PY_MODULE_HPP_
#define PY_MODULE_HPP_

#include <string>
#include <aff3ct.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace aff3ct
{
namespace module
{
class Py_Module : public Module
{
private:
	std::shared_ptr<py::object> child;

public:
	inline Socket& operator[](const std::string &tsk_sck );
	inline Task&   operator()(const std::string &tsk_name);

	Py_Module(const bool enable_single_wave = true);
	Py_Module(const Py_Module& );

	virtual ~Py_Module() =default;

	virtual Py_Module* clone() const;
	virtual py::object __copy__() const;
	virtual py::object __deepcopy__() const;
	virtual void set_n_frames_per_wave(const size_t n_frames_per_wave);
	virtual void set_n_frames         (const size_t n_frames         );

	void create_codelet(Task& task, std::function<int(Module &m, Task &t, const size_t frame_id)> codelet);
	std::string to_string() const;
	bool has_child() const;
	py::object get_child() const;

};
}
}

#include "Py_Module.hxx"
#endif /* PY_MODULE_HPP_ */
