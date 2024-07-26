#include <iostream>
#include <stdint.h>
#include <memory>
#include "mmappable_vector.h"
#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <iostream>
#include <filesystem>
#include <parallel/algorithm>
//#include <algorithm>
#include <chrono>


void usage(const char * progname)
{
    std::cout << "Usage " << progname << " file_to_read "   << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{

    if(argc != 2){
	usage(argv[0]);
    }


    std::filesystem::path p{argv[1]};
    const auto size = std::filesystem::file_size(p);

    std::cout << "treating file " << p.u8string()
	      << " with size " << size << "\n";
    using namespace mmap_allocator_namespace;
    mmappable_vector<uint64_t> buffer;

    const auto timestamp_start{std::chrono::steady_clock::now()};
    buffer.mmap_file(argv[1], READ_ONLY, 0, size);
    const auto timestamp_mapped{std::chrono::steady_clock::now()};
    auto r = timepix::sort::read_chunks(buffer);
    const auto timestamp_read{std::chrono::steady_clock::now()};

    int rising_edge = 0x6E;
    auto timestamps = timepix::sort::process(r, rising_edge, 6);
    const auto timestamp_process{std::chrono::steady_clock::now()};

    std::cout << "Got " << timestamps.size() << " timestamps\n";
    const auto timestamp_sort_start{std::chrono::steady_clock::now()};
    __gnu_parallel::sort(timestamps.begin(), timestamps.end());
    // std::sort(timestamps.begin(), timestamps.end());
    const auto timestamp_sort_end{std::chrono::steady_clock::now()};

    const std::chrono::duration<double>
	for_mapping    { timestamp_mapped   - timestamp_start      } ,
	for_reading    { timestamp_read     - timestamp_mapped     } ,
	for_processing { timestamp_process  - timestamp_read       } ,
	for_sorting    { timestamp_sort_end - timestamp_sort_start } ;

    std::cout << "Processing required "
	      << "\n\t mapping    " << for_mapping.count()
	      << "\n\t reading    " << for_reading.count()
	      << "\n\t processing " << for_processing.count()
	      << "\n\t sorting    " << for_sorting.count()
	      << std::endl;

}
