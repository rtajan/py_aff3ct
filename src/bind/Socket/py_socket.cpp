#include <pybind11/pybind11.h>
#include <map>

#include "Module/Socket.hpp"
#include "Module/Module.hpp"

namespace py = pybind11;
using namespace py::literals;

std::map<std::type_index, std::string> type_map = {{typeid(int8_t ), pybind11::format_descriptor<int8_t >::format()},
                                                   {typeid(int16_t), pybind11::format_descriptor<int16_t>::format()},
                                                   {typeid(int32_t), pybind11::format_descriptor<int32_t>::format()},
                                                   {typeid(int64_t), pybind11::format_descriptor<int64_t>::format()},
                                                   {typeid(float  ), pybind11::format_descriptor<float  >::format()},
                                                   {typeid(double ), pybind11::format_descriptor<double >::format()}};


PYBIND11_MODULE(py_socket, m){
	pybind11::class_<aff3ct::module::Socket>(m, "Socket", pybind11::buffer_protocol())
	//.def(py::init<py::ssize_t, py::ssize_t>())
	.def_buffer([](aff3ct::module::Socket &sck) -> pybind11::buffer_info{
			size_t n_row = (size_t)sck.get_task().get_module().get_n_frames();
			size_t n_col = (size_t)sck.get_n_elmts()/n_row;
			return pybind11::buffer_info(
				sck.get_dataptr(),                               /* Pointer to buffer */
				sck.get_datatype_size(),                          /* Size of one scalar */
				type_map[sck.get_datatype()], /* Python struct-style format descriptor */
				2,                                      /* Number of dimensions */
				{n_row, n_col},                 /* Buffer dimensions */
				{(size_t)sck.get_datatype_size()*n_col,
				 (size_t)sck.get_datatype_size()}
			);
		});
}