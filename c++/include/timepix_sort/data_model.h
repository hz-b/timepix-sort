#ifndef TIMEPIX_DATA_MODEL_H
#define TIMEPIX_DATA_MODEL_H

#include <stdint.h>
#include <vector>

namespace timepix::data_model {

    class PixelPos{
    private:
	int16_t m_x, m_y;

    public:
	PixelPos(int16_t x, int16_t y)
	    : m_x(x)
	    , m_y(y)
	{}
	inline auto x() { return this->m_x; }
	inline auto y() { return this->m_x; }
    };

    template<class D>
    struct HasTimeOfArrival
    {
	int64_t time_of_arrival(){
	    return static_cast<D*>(this)->time_of_arrival_impl();
	}
    };

    class TimeOfFlightEvent : public HasTimeOfArrival<TimeOfFlightEvent>{
    private:
	int64_t m_time_of_arrival;
    protected:
	int64_t time_of_arrival_impl(){
	    return this->m_time_of_arrival;
	}
    };

    class PixelEvent : public HasTimeOfArrival<PixelEvent>{
    private:
	int64_t m_time_of_arrival, m_time_over_threshold;
	PixelPos m_pos;
    protected:
	int64_t time_of_arrival_impl(){
	    return this->m_time_of_arrival;
	}

    public:
	inline auto time_over_treshold_impl()  { return this-m_time_over_threshold; }
	inline auto x()                        { return this->m_pos.x();            }
	inline auto y()                        { return this->m_pos.y();            }
    };



};

#endif
