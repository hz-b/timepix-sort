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
	  [](const tpd::ChunkCollection& collection, const int select_trigger_mode, const uint64_t time_over_threshold) {
	      auto tmp = tps::process(collection, select_trigger_mode, time_over_threshold);
	      auto ev = tpd::EventCollection(std::move(std::get<0>(tmp)));
	      return std::make_pair(ev, std::get<1>(tmp));
	  },
	  "extract events from the chunks",
	  py::arg("collection"), py::arg("selected_trigger_mode"), py::arg("time_over_threshold")
	);

}
