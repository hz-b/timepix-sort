#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <timepix_sort/detail/process_chunks.h>
#include <iostream>
#include <cassert>

/*
enum TDC1TriggerMode{
    rising_edge = 0x6F,
    falling_edge = 0x6A
};

enum TDC2TriggerMode{
    rising_edge = 0x6E,
    falling_edge = 0x6B
};
*/

enum TDCEventType{
    timestamp = 0x6,
    pixel = 0xb
};

namespace tpxs = timepix::sort;
namespace tpxd = timepix::sort::detail;

// tdc_time_stamp;
// unfold_pixel_event


//void process_one(uint64_t pkg)
std::vector<uint64_t> tpxs::process(
    const timepix::data_model::ChunkCollection& collection,
    int select_trigger_mode,
    int tot_min
    )
{

    uint64_t n_events=0, n_pixels=0, n_timestamps=0;
    std::vector<uint64_t> timestamps;

    for(size_t i=0; i < collection.size(); ++i){
	const auto& view = collection.get(i);
	const auto& events = view.events();
	// Todo: recheck that the header is found !
#if 0
	{
	    assert(events.size() == view.n_events());
	    char chip_nr;
	    int check;
	    std::tie(chip_nr, check) = tpxs::process_header(view.header());
	    assert(check == events.size());

	}
#endif

	for(const uint64_t ev : events){
	    int event_type = (ev >> 60) & 0xf;
	    int trigger_mode = ev >> 56;

	    switch(event_type){
	    case timestamp:
		n_timestamps++;
		if (select_trigger_mode == trigger_mode){
		    timestamps.push_back(tpxd::tdc_time_stamp(ev));
		}
		break;
	    case pixel:
		n_pixels++;
		timestamps.push_back(tpxd::unfold_pixel_event(ev));
		break;
	    }
	    n_events++;
	}
    }
    std::cout << "procssed " << collection.size() << " chunks"
	      << " containing " << n_events << " events"
	      << " containing "<< n_pixels << " pixels"
	      << " timestamps " << n_timestamps
	      << " remaining " << int64_t(n_events) - int64_t(n_pixels + n_timestamps)
	      << std::endl;

    return timestamps;
}
