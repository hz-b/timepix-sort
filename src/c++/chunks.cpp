#include "timepix_sort_py.h"
#include <timepix_sort/data_model_chunks.h>

namespace tpp = timepix::python;
namespace tpd = timepix::data_model;
namespace py = pybind11;

void tpp::chunks_init(py::module &m)
{

    py::class_<tpd::ChunkView> chunk_view    (m, "ChunkView");
    chunk_view
	.def("n_events", &tpd::ChunkView::n_events)
	.def("chip_nr", &tpd::ChunkView::chip_nr)
	.def("offset", &tpd::ChunkView::offset)
	;

    py::class_<tpd::ChunkCollection> chunk_collection    (m, "ChunkCollection");
    chunk_collection
	.def("size", &tpd::ChunkCollection::size)
	.def("__getitem__", &tpd::ChunkCollection::at)
	.def("__len__", &tpd::ChunkCollection::size)
	;

}
