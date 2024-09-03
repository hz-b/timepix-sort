#include "timepix_sort_py.h"
#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <stdint.h>

namespace tpp = timepix::python;
namespace tps = timepix::sort;
namespace tpd = timepix::data_model;
namespace py = pybind11;

void tpp::read_init(py::module &m)
{
    /**
     * todo: check if buffer is good enough ...
     *       review if it is worth to provide an unmannaged interface
     *       to the collection.
     */
    m.def("read_chunks",
	  [](py::array_t<uint64_t,  py::array::c_style | py::array::forcecast>& data){
	      const uint64_t *ptr  = (const uint64_t *)(data.data());
	      auto t_vec = std::make_shared<const std::vector<uint64_t>>(ptr, ptr + data.size());
	      return tps::read_chunks(t_vec);
	  });

    // m.def("read_chunks", &tps::read_chunks);

    m.def("process",
	  [](const tpd::ChunkCollection& collection, const int select_trigger_mode, const int tot_min) {
	      return tpd::EventCollection(std::move(tps::process(collection, select_trigger_mode, tot_min)));
	  });

}
