#include <iostream>
#include <stdint.h>
#include <memory>
#include "mmappable_vector.h"
#include <timepix_sort/read.h>
#include <timepix_sort/process.h>
#include <iostream>

void usage(const char * progname)
{
    std::cout << "Usage " << progname << " file_to_read file_size"   << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{

    if(argc != 3){
	usage(argv[0]);
    }
    size_t size = std::atol(argv[2]);

    using namespace mmap_allocator_namespace;
    mmappable_vector<uint64_t> buffer;

    buffer.mmap_file(argv[1], READ_ONLY, 0, size);
    auto r = timepix::sort::read_chunks(buffer);

    int rising_edge = 0x6E;
    auto timestamps = timepix::sort::process(r, rising_edge, 6);

    std::cout << "Got " << timestamps.size() << " timestamps\n";

}
