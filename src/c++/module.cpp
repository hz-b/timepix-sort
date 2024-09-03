#include <pybind11/pybind11.h>
#include "timepix_sort_py.h"


namespace py = pybind11;
namespace tpp = timepix::python;


PYBIND11_MODULE(_timepix_sort, m) {
    m.doc() = "timepix_sort: c++ wrapper";
    tpp::event_init(m);
    tpp::chunks_init(m);
    tpp::read_init(m);
}
