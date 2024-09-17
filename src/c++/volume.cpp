#include "timepix_sort_py.h"
#include "lookup_table.h"
#include <pybind11/numpy.h>
#include <timepix_sort/events.h>
#include <iostream>
#include <boost/numeric/conversion/cast.hpp>

namespace py = pybind11;
namespace dm = timepix::data_model;
namespace tpp = timepix::python;
namespace tps = timepix::sort;


template<typename array_base_t>
static void
data_to_volume(const tps::PixelEventsDiffTime& pd, const py::array_t<uint64_t>& lut, py::array& volume)
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

    auto r = volume.mutable_unchecked<array_base_t, 3>();
    // fill the buffer
    size_t event_nr = -1;
    for(const auto& ev: pd){
	event_nr++;
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
	const int64_t tot = ev.time_over_threshold();
	const int64_t prev_tot = r(pos.x(), pos.y(), ti);
	const int64_t tot_s = tot + prev_tot;

	try {
	    r(pos.x(), pos.y(), ti) = boost::numeric_cast<array_base_t>(tot_s);
	}
	catch(boost::numeric::negative_overflow& e) {
	    std::stringstream strm;
	    strm << "event nr " << event_nr << ": tot = " << tot
		 << " old value =  " << prev_tot
		 << " total " << tot_s
		 << ": " << e.what();
	    throw std::overflow_error(strm.str());
	}
	catch(boost::numeric::positive_overflow& e) {
	    std::stringstream strm;
	    strm << "event nr " << event_nr << ": tot = " << tot
		 << " old value =  " << prev_tot
		 << " total " << tot_s
		 << ": " << e.what();
	    throw std::overflow_error(strm.str());
	}
    }
}

static auto
data_to_points(const tps::PixelEventsDiffTime& pd)
{
    std::vector<py::ssize_t> dims = {boost::numeric_cast<py::ssize_t>(pd.size()), 3};
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


static void
data_to_volume_gateway(const tps::PixelEventsDiffTime& pd, const py::array_t<uint64_t>& lut, py::array& volume)
{

    const auto dtype = volume.dtype();
    const auto dtype_num = dtype.num();
    if(dtype_num == py::dtype::of<uint8_t>().num()){
	data_to_volume<uint8_t>(pd, lut, volume);
    } else if (dtype_num == py::dtype::of<int8_t>().num()){
	data_to_volume<int8_t>(pd, lut, volume);
    } else if(dtype_num == py::dtype::of<uint16_t>().num()){
	data_to_volume<uint16_t>(pd, lut, volume);
    } else if (dtype_num == py::dtype::of<int16_t>().num()){
	data_to_volume<int16_t>(pd, lut, volume);
    } else if(dtype_num == py::dtype::of<uint32_t>().num()){
	data_to_volume<uint32_t>(pd, lut, volume);
    } else if (dtype_num == py::dtype::of<int32_t>().num()){
	data_to_volume<int32_t>(pd, lut, volume);
    } else if(dtype_num ==  py::dtype::of<uint64_t>().num()){
	data_to_volume<uint64_t>(pd, lut, volume);
    } else if (dtype_num == py::dtype::of<int64_t>().num()){
	data_to_volume<int64_t>(pd, lut, volume);
    } else {
	std::stringstream strm;
	strm <<  "data to gateway dtype: " << dtype.char_()
	     << ", dtype num " << dtype.num()
	     << "not handled!";
	throw std::runtime_error(strm.str());
    }
}


void tpp::volume_init(pybind11::module &m)
{

    m.def("data_to_volume", &data_to_volume_gateway);
    m.def("data_to_points", data_to_points);
}
