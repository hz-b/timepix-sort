#include "timepix_sort_py.h"
#include <timepix_sort/data_model.h>
#include <timepix_sort/process.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <iostream>
#include <sstream>

namespace tpp = timepix::python;
namespace tpd = timepix::data_model;
namespace tps = timepix::sort;
namespace py = pybind11;

static const py::buffer_info sorted_indices_of_collection(std::vector<size_t>& vec)
{
    return py::buffer_info(
            vec.data(),                               /* Pointer to buffer */
            sizeof(size_t),                           /* Size of one scalar */
            py::format_descriptor<size_t>::format(),  /* Python struct-style format descriptor */
            1,                                        /* Number of dimensions */
            {vec.size() },                            /* Buffer dimensions */
            { sizeof(size_t) }                        /* Strides (in bytes) for each index */
        );
}

void tpp::event_init(py::module &m)
{

    py::class_<tpd::PixelPos>   pixel_pos  (m, "PixelPos");
    pixel_pos
	.def_property_readonly("x", &tpd::PixelPos::x)
	.def_property_readonly("y", &tpd::PixelPos::y)
	;


    py::class_<tpd::TimeOfFlightEvent> time_of_flight_ev(m, "TimeOfFlightEvent");
    time_of_flight_ev
	// todo: convert it to np.datetime64(val, 'fs')
	.def_property_readonly("time_of_arrival", &tpd::TimeOfFlightEvent::time_of_arrival)
	.def("__repr__", [](const tpd::TimeOfFlightEvent& ev){
	    std::stringstream strm;
	    ev.show(strm);
	    return strm.str();
	})
	;

    py::class_<tpd::PixelEvent> pixel_event(m, "PixelEvent");
    pixel_event
    	.def_property_readonly("time_of_arrival",     &tpd::PixelEvent::time_of_arrival)
    	.def_property_readonly("time_over_threshold", &tpd::PixelEvent::time_over_threshold)
  	.def_property_readonly("pos",                 &tpd::PixelEvent::pos)
  	.def_property_readonly("x",                   &tpd::PixelEvent::x)
  	.def_property_readonly("y",                   &tpd::PixelEvent::y)
	.def("__repr__", [](const tpd::PixelEvent& ev){
	    std::stringstream strm;
	    ev.show(strm);
	    return strm.str();
	})
	;

    py::class_<tpd::Event> event(m, "Event");

    /* todo: simplify the index handling code */
    struct EventCollectionSortedIndices
    {
	std::vector<size_t> m_sorted_indices;

	EventCollectionSortedIndices(std::vector<size_t>&& sorted_indices)
	    : m_sorted_indices (std::move(sorted_indices))
	    {}

	const auto size() { return this->m_sorted_indices.size(); }
    };

    struct PixelEventsDiffTime
    {
	std::vector<tpd::PixelEvent> m_pixel_events;
	bool m_is_sorted = false;

	PixelEventsDiffTime(std::vector<tpd::PixelEvent>&& pixel_events, bool is_sorted = false)
	    : m_pixel_events(std::move(pixel_events))
	    , m_is_sorted(is_sorted)
	    {}

	/*
	PixelEventsDiffTime(const std::vector<tpd::PixelEvent>& pixel_events)
	    : m_pixel_events(pixel_events)
	    {}
	*/
	const auto size() const { return this->m_pixel_events.size(); }
	const auto& at(const size_t i) const { return this->m_pixel_events.at(i); }
	void sort() {
	    tps::sort_pixel_events(this->m_pixel_events);
	    this->m_is_sorted = true;
	}
	const bool is_sorted() const { return this->m_is_sorted; }
	py::array_t<uint64_t> time_of_arrival() const {
	    py::array_t<uint64_t> ta(this->size());
	    auto r = ta.mutable_unchecked<1>();
	    for(size_t i=0; i<this->size(); ++i){
		r(i) = this->m_pixel_events[i].time_of_arrival();
	    }
	    return ta;
	}
    };

    py::class_<EventCollectionSortedIndices> event_collection_sorted_indices(m, " EventCollectionSortedIndices", py::buffer_protocol());
    event_collection_sorted_indices
	.def("size", &EventCollectionSortedIndices::size)
	.def_buffer([](EventCollectionSortedIndices& si){
	    return sorted_indices_of_collection(si.m_sorted_indices);
	})
	;

    py::class_<PixelEventsDiffTime> pixel_events_diff_time(m, "PixelEventsDiffTime");
    pixel_events_diff_time
	.def("__len__", &PixelEventsDiffTime::size)
	.def("__getitem__", [](const PixelEventsDiffTime& pd, const size_t i){
	    return pd.at(i);
	})
	.def("sort", &PixelEventsDiffTime::sort)
	.def("time_of_arrival", &PixelEventsDiffTime::time_of_arrival)
	.def_property_readonly("is_sorted", &PixelEventsDiffTime::is_sorted)
	;

    py::class_<tpd::EventCollection> event_collection(m, "EventCollection");
    event_collection
	.def("__len__", &tpd::EventCollection::size)
	.def("__getitem__", [](const tpd::EventCollection& col, const size_t i){
	    return col.at(i).get_event();
	})
	.def("sorted_indices", [](const tpd::EventCollection& col){
	    // return timepix::sort::sort_indices(col);
	    // should be able to return std::vector as an np array and not a list
	    return EventCollectionSortedIndices(timepix::sort::sort_indices(col));
	})
	.def("pixel_events_with_difference_time",
	     // don't make the indices array a std:vector. pybind seems to cnvert it into
	     // a list
	     [](const tpd::EventCollection& col, py::array_t<uint64_t> indices_a){
		 // todo: make it optional if pixel events should be sorted
		 py::buffer_info buf = indices_a.request();
		 uint64_t * p = static_cast<uint64_t *>(buf.ptr);
		 const size_t n_elms = indices_a.size();
		 const std::vector<size_t> indices(p, p + n_elms);
		 return PixelEventsDiffTime(std::move(tps::calculate_diff_time(col, indices)));
		 //return PixelEventsDiffTime(std::move());
	     },
	     "produce a vector of pixel events with the time difference from the"
	     " last trigger calculated using sorted indices to access events in"
	     " the collection in a sorted manner",
	     py::arg("sorted_indices")
	    );
}
