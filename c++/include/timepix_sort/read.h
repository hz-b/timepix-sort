#ifndef TIMEPIX_READ_H
#define TIMEPIX_READ_H

#include <timepix_sort/data_model_chunks.h>
#include "mmappable_vector.h"

namespace timepix::sort {

    timepix::data_model::ChunkCollection
    read_chunks(const mmap_allocator_namespace::mmappable_vector<uint64_t>& buffer);

}// namespace timepix::sort
#endif // TIMEPIX_READ_H
