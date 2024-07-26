#ifndef TIMEPIX_DATA_MODEL_CHUNKS_H
#define TIMEPIX_DATA_MODEL_CHUNKS_H

#include <stdint.h>
#include <memory>
#include <vector>
#include "mmappable_vector.h"

namespace timepix::data_model {

    struct ChunkAddress{
	uint64_t offset;
	uint32_t n_events;
	char chip_nr;

	inline ChunkAddress(uint64_t offset_, char chip_nr, uint32_t n_events_)
	    : offset(offset_)
	    , n_events(n_events_)
	    , chip_nr(chip_nr)
	    {}
    };


    class ChunkView;


    class ChunkCollection{
    private:
	const mmap_allocator_namespace::mmappable_vector<uint64_t>& m_buffer;
	// std::shared_ptr<const std::vector<uint64_t>> m_buffer;
	const std::vector<ChunkAddress> m_chunk_addresses;

	inline auto buffer() const { return this->m_buffer; }
	friend class ChunkView;

    public:
    ChunkCollection(
	const mmap_allocator_namespace::mmappable_vector<uint64_t>& buffer,
	// std::shared_ptr<const std::vector<uint64_t>> buffer,
	const std::vector<ChunkAddress> chunk_addresses
	)
	    : m_buffer(buffer)
	    , m_chunk_addresses(chunk_addresses)
	{}
	inline ChunkView get(size_t index) const ;
	inline size_t size() const { return this->m_chunk_addresses.size(); }
	/*
	*/
    };


    class ChunkView{
    private:
	const mmap_allocator_namespace::mmappable_vector<uint64_t>& m_buffer;
	//std::shared_ptr<const std::vector<uint64_t>> m_buffer;
	const ChunkAddress m_address;

    public:
    ChunkView(
	const mmap_allocator_namespace::mmappable_vector<uint64_t>& buffer,
	//const std::shared_ptr<const std::vector<uint64_t>> buffer,
	const ChunkAddress address)
	: m_buffer(buffer)
	, m_address(address)
	{}

	inline auto offset()    const { return this->m_address.offset;   }
	inline auto n_events()  const { return this->m_address.n_events; }
	inline auto chip_nrs()  const { return this->m_address.chip_nr;  }

	inline auto events()   const {
	    const auto start = this->offset();
	    const auto  end = start + this->n_events();
	    // return std::vector<int64_t>(this->m_buffer->begin() + start, this->m_buffer->begin() + end);
	    std::vector<uint64_t> r(this->n_events());
	    for(size_t i=0;  i<this->n_events(); ++i){
		r[i] = this->m_buffer[i];
	    }
	    return r;
	    // return std::vector<uint64_t>(this->m_buffer.begin() + start * sizeof(uint64_t), this->m_buffer.begin() + end  * sizeof(uint64_t));
	}
    };

    inline ChunkView ChunkCollection::get(size_t index) const {
	return ChunkView(this->m_buffer, this->m_chunk_addresses.at(index));
    }


} // namespace timepi::datamodel
#endif // TIMEPIX_DATA_MODEL_CHUNKS_H
