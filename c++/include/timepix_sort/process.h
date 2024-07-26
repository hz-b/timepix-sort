#ifndef TIMEPIX_PROCESS_H
#define TIMEPIX_PROCESS_H

#include <timepix_sort/data_model_chunks.h>
#include "mmappable_vector.h"

namespace timepix::sort {
    std::vector<uint64_t>  process(const timepix::data_model::ChunkCollection&,
				   int trigger_mode, int tot_min
	);
}// namespace timepix::sort
#endif // TIMEPIX_PROCESS_H
