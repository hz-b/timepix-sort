#include "timepix_sort_py.h"
#include "lookup_table.h"
#include <pybind11/numpy.h>
#include <timepix_sort/events.h>
#include <iostream>

namespace py = pybind11;
namespace dm = timepix::data_model;
namespace tpp = timepix::python;
namespace tps = timepix::sort;


static void
data_to_volume(const tps::PixelEventsDiffTime& pd, const py::array_t<uint64_t>& lut, py::array_t<uint16_t> volume)
{
    auto lut_r = lut.unchecked<2>();
    if (lut_r.shape(1) != 2)
	throw std::runtime_error("Lut last dimension must be 2");

    py::buffer_info buf = lut.request();
    lookup::unbounded_lookup1d lutt;
    // fill lookup table
    for(py::ssize_t i= 0; i < lut_r.shape(1); ++i){
	lutt.insert(lut_r(i, 0), lut_r(i, 1));
    }

    auto r = volume.mutable_unchecked<3>();
    // fill the buffer
    for(const auto& ev: pd){
	const auto pos = tps::map_pixel_and_chip_nr_to_global_pixel(ev.pos(), ev.chip_nr());
	if (pos.x() < 0){
	    throw std::range_error("x < 0");
	}
	if (pos.x() >= r.shape(0) - 1){
	    std::cout << "x pos" << pos.x();
	    throw std::range_error("x >= r.shape(0) - 1");
	}
	if (pos.y() < 0){
	    throw std::range_error("y < 0");
	}
	if (pos.y() >= r.shape(1) - 1){
	    throw std::range_error("y >= r.shape(1) -1");
		}
	const auto tmp = lutt.linear_interp(ev.time_of_arrival());
	const auto ti = int64_t(std::round(tmp));
	if (ti < 0){
	    throw std::range_error("ti <= 0");
	}
	if (ti >= r.shape(2) - 1){
	    throw std::range_error("ti >= r.shape(2) - 1");
	}
	r(pos.x(), pos.y(), ti) += ev.time_over_threshold();
    }
}

static auto
data_to_points(const tps::PixelEventsDiffTime& pd)
{
    std::vector<py::ssize_t> dims = {pd.size(), 3};
    py::array_t<uint64_t> res(dims);
    auto r = res.mutable_unchecked<2>();

    assert(r.shape(0) == pd.size());
    assert(r.shape(1) == 3);
    for(py::ssize_t i=0; i < r.shape(0); ++i){
	const dm::PixelEvent& ev = pd[i];
	const auto pos = tps::map_pixel_and_chip_nr_to_global_pixel(ev.pos(), ev.chip_nr());
	const auto ta = ev.time_of_arrival();
	r(i, 0) = pos.x();
	r(i, 1) = pos.y();
	r(i, 2) = ta;
    }

    return res;
}


void tpp::volume_init(pybind11::module &m)
{

    m.def("data_to_volume", data_to_volume);
    m.def("data_to_points", data_to_points);
}
