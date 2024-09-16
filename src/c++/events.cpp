#include "timepix_sort_py.h"
#include <timepix_sort/data_model.h>
#include <timepix_sort/process.h>
#include <timepix_sort/events.h>
#include <timepix_sort/hardware/tpx3.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
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


template<class C, class UnaryOp>
static auto fill_pyarray(const C& cls, UnaryOp op)
{
    const size_t n = cls.size();
    ///> @todo a size save acst
    const py::ssize_t pyn = n;
    auto sample = op(cls.at(0));
    py::array_t<decltype(sample)> ta(pyn);
    py::buffer_info buf = ta.request();
    decltype(sample) * ptr = static_cast<decltype(sample)*>(buf.ptr);

    for(size_t i=0; i<n; ++i){
	ptr[i] = op(cls[i]);
    }
    return ta;
}

#if 0
static const py::buffer_info time_of_arrival_of_pixel_events(std::vector<tpd::PixelEvent>& vec)
{
    return py::buffer_info(
            vec.data(),                                /* Pointer to buffer */
            sizeof(uint64_t),                          /* Size of one scalar */
            py::format_descriptor<uint64_t>::format(), /* Python struct-style format descriptor */
            1,                                         /* Number of dimensions */
            {vec.size() },                             /* Buffer dimensions */
            { sizeof(tpd::PixelEvent) }                /* Strides (in bytes) for each index */
        );
}



class PixelEventsViewer : public tps::PixelEventsDiffTime
{

public:
    PixelEventsViewer(tps::PixelEventsDiffTime &pd)
	: tps::PixelEventsDiffTime(pd)
	{}
    // should be considered nearly a burgler
    auto& events() {
	return this->m_pixel_events;
    }
};
#endif


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

    py::class_<tpd::EventStatistics> event_statistics(m, "EventStatistic");
    event_statistics
	.def("__repr__", [](const tpd::EventStatistics& ev){
	    std::stringstream strm;
	    ev.show(strm);
	    return strm.str();
	})
	.def_property_readonly( "n_events"                   , &tpd::EventStatistics::n_events                  )
	.def_property_readonly( "n_pixels"                   , &tpd::EventStatistics::n_pixels                  )
	.def_property_readonly( "n_timestamps"               , &tpd::EventStatistics::n_timestamps              )
	.def_property_readonly( "n_timestamps_with_trigger"  , &tpd::EventStatistics::n_timestamps_with_trigger )
	.def_property_readonly( "n_control_indications"      , &tpd::EventStatistics::n_control_indications     )
	.def_property_readonly( "n_global_time"              , &tpd::EventStatistics::n_global_time             )
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


    py::class_<EventCollectionSortedIndices> event_collection_sorted_indices(m, " EventCollectionSortedIndices", py::buffer_protocol());
    event_collection_sorted_indices
	.def("size", &EventCollectionSortedIndices::size)
	.def_buffer([](EventCollectionSortedIndices& si){
	    return sorted_indices_of_collection(si.m_sorted_indices);
	})
	;
#if 0
    py::class_<PixelEventsViewer> pixel_events_time_arrival_buffer(m, "_PixelEventsTimeOfArrival");
    pixel_events_time_arrival_buffer
	.def_buffer([](PixelEventsViewer& viewer){
	    return time_of_arrival_of_pixel_events(viewer.events());
	})
	;
#endif

    py::class_<tps::PixelEventsDiffTime> pixel_events_diff_time(m, "PixelEventsDiffTime");
    pixel_events_diff_time
	.def("__len__", &tps::PixelEventsDiffTime::size)
	.def("__getitem__", [](const tps::PixelEventsDiffTime& pd, const size_t i){
	    return pd.at(i);
	})
	///> @todo release GIL for this block ?
	.def("sort", &tps::PixelEventsDiffTime::sort)
	.def_property_readonly("is_sorted", &tps::PixelEventsDiffTime::is_sorted)
	//> @todo  release GIL for this block ?
	//>        or returning a buffer object with
	.def("time_of_arrival", [](const tps::PixelEventsDiffTime& pd) {
	    return fill_pyarray<>(pd, [](const tpd::PixelEvent& ev){ return ev.time_of_arrival(); });
	})
	.def("time_over_threshold", [](const tps::PixelEventsDiffTime& pd) {
	    return fill_pyarray<>(pd, [](const tpd::PixelEvent& ev){ return ev.time_over_threshold(); });
	})
#if 0
	///> todo: add keep alive?
	.def("time_of_arrival_as_buffer", [](tps::PixelEventsDiffTime& pd){
	    return PixelEventsViewer(pd);
	})
#endif
	;

    py::class_<tpd::EventCollection> event_collection(m, "EventCollection");
    event_collection
	.def("__len__", &tpd::EventCollection::size)
	.def("__getitem__", [](const tpd::EventCollection& col, const size_t i){
	    return col.at(i).get_event();
	})
	/**
	 * return timepix::sort::sort_indices(col);
	 * should be able to return std::vector as an np array and not a list
	 * currently returning special type to wrap access to indices as a
	 * buffer
	 */
	.def("time_of_arrival", [](const tpd::EventCollection &col) {
	    return fill_pyarray<>(col, [](const tpd::Event& ev){ return ev.time_of_arrival(); });
	})
	.def("sorted_indices",
	     [](const tpd::EventCollection& col, const uint64_t modulo){
		 ///> todo: add keep alive, I guess not required here
		 /// as the object keeps its own buffer
		 return EventCollectionSortedIndices(timepix::sort::sort_indices(col, modulo));
	     },
	     "Return an object containing the sorted indices as"
	     "\nan buffer object. Please apply numpy.array on them"
	     "\nreturned object to use them in python"
	     "\n"
	     "\nonly events will be taken into account whose"
	     "\nthreshold is above the spcified value."
	     "\n"
	     "\nThis solution was chosen as it provides"
	     "\nsignificant flexibility without too much overhead",
	     py::arg("modulo") = timepix::hardware::tpx3::pixel_max_time_fs
	    )

	/**
	 * @warning don't make the indices array a std:vector.
	 *          pybind seems to convert it into a list
	 */
	.def("pixel_events_with_difference_time",
	     [](const tpd::EventCollection& col, const py::array_t<uint64_t> indices_a, const uint64_t modulo){
		 ///> @todo make it optional if pixel events should be sorted
		 py::buffer_info buf = indices_a.request();
		 ///> @todo Can I release GIL for this block?
		 {
		     uint64_t * p = static_cast<uint64_t *>(buf.ptr);
		     const size_t n_elms = indices_a.size();
		     const std::vector<size_t> indices(p, p + n_elms);
		     return tps::PixelEventsDiffTime(std::move(tps::calculate_diff_time(col, indices, modulo)));
		 }
		 //return PixelEventsDiffTime(std::move());
	     },
	     "produce a vector of pixel events with the time difference from the"
	     " last trigger calculated using sorted indices to access events in"
	     " the collection in a sorted manner",
	     py::arg("sorted_indices"),
	     py::arg("modulo") = timepix::hardware::tpx3::pixel_max_time_fs
	    );
}
