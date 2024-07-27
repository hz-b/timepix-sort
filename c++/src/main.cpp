#include <chrono>
#include <iostream>
#include <stdint.h>
#include <memory>
//#include <parallel/algorithm>
#include <algorithm>

#include <timepix_sort/read.h>
#include <timepix_sort/process.h>

namespace dm = timepix::data_model;

void usage(const char * progname)
{
    std::cout << "Usage " << progname << " file_to_read "   << std::endl;
    exit(1);
}

static void swap(
    timepix::data_model::EventProxyForTimeOfArrivalSort& a,
    timepix::data_model::EventProxyForTimeOfArrivalSort& b
    )
{
}

int main(int argc, char *argv[])
{

    if(argc != 2){
	usage(argv[0]);
    }

    const auto buffer = timepix::sort::detail::read_raw(std::string(argv[1]));
    const auto timestamp_start{std::chrono::steady_clock::now()};
    auto r = timepix::sort::read_chunks(buffer);
    const auto timestamp_read{std::chrono::steady_clock::now()};

    int rising_edge = 0x6E;
    auto events = timepix::sort::process(r, rising_edge, 6);
    const auto timestamp_process{std::chrono::steady_clock::now()};
    std::cout << "Got " << events.size() << " timestamps\n";
    const auto timestamp_sort_start{std::chrono::steady_clock::now()};

    // const auto support_sort = std::vector
    std::vector<dm::EventProxyForTimeOfArrivalSort> pev;
    pev.reserve(events.size());
    std::transform(events.begin(), events.end(), std::back_inserter(pev),
		   [](const auto& ev){ return dm::EventProxyForTimeOfArrivalSort(ev); }
	);

    //std::sort(pev.begin(), pev.end());
    //__gnu_parallel::sort(pev.begin(), pev.end());
    //parallel::sort(timestamps.begin(), timestamps.end());
    const auto timestamp_sort_end{std::chrono::steady_clock::now()};

    const std::chrono::duration<double>
	for_reading    { timestamp_read     - timestamp_start      } ,
	for_processing { timestamp_process  - timestamp_read       } ,
	for_sorting    { timestamp_sort_end - timestamp_sort_start } ;

    std::cout << "Processing in main "
	      << "\n\t reading     " << for_reading.count()
	      << "\n\t processing  " << for_processing.count()
	      << "\n\t sorting     " << for_sorting.count()
	      << std::endl;

#if 0
    const auto sorted = timestamps;
    std::cout << sorted.size()
	      << ": " << sorted[0]
	      << " -- "  << sorted[sorted.size() / 2]
	      <<" -- " << sorted[sorted.size() - 1]
	      << "." << std::endl;
#endif

}
