#ifndef TIMEPIX_DATA_MODEL_H
#define TIMEPIX_DATA_MODEL_H

#include <stdint.h>
#include <vector>
#include <variant>


namespace timepix::data_model {

    class PixelPos{
    private:
	const int16_t m_x, m_y;

    public:
	PixelPos(const int16_t x, const int16_t y)
	    : m_x(x)
	    , m_y(y)
	{}
	inline auto x() const { return this->m_x; }
	inline auto y() const { return this->m_x; }
    };


    template<class D>
    struct HasTimeOfArrival
    {
	inline const int64_t time_of_arrival() const {
	    return static_cast<const D*>(this)->time_of_arrival_impl();
	}

    };


    class TimeOfFlightEvent : public HasTimeOfArrival<TimeOfFlightEvent>{
    private:
	const int64_t m_time_of_arrival;

    public:
	TimeOfFlightEvent(int64_t time_of_arrival)
	    : m_time_of_arrival(time_of_arrival)
	    {}

	inline const int64_t time_of_arrival_impl() const {
	    return this->m_time_of_arrival;
	}
    };


    class PixelEvent : public HasTimeOfArrival<PixelEvent>{
    private:
	const int64_t m_time_of_arrival;
	const int64_t m_time_over_threshold;
	const PixelPos m_pos;

    public:
	inline PixelEvent(const PixelPos pos, const int64_t time_of_arrival, const int64_t time_over_threshold)
	    : m_time_of_arrival(time_of_arrival)
	    , m_time_over_threshold(time_over_threshold)
	    , m_pos(pos)
	    {}

	inline auto time_over_tresholdl()  { return this-m_time_over_threshold; }
	inline auto x()                    { return this->m_pos.x();            }
	inline auto y()                    { return this->m_pos.y();            }

	inline const int64_t time_of_arrival_impl() const {
	    return this->m_time_of_arrival;
	}

    };

    namespace {
	// helper type for the visitor #4
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	// explicit deduction guide (not needed as of C++20)
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
    }

    typedef struct std::variant<TimeOfFlightEvent,PixelEvent> a_event;
    class Event
    {
	a_event m_event;
    public:
	inline Event(a_event&& event)
	    : m_event(std::move(event))
	    {}

	inline Event(TimeOfFlightEvent&& event)
	    : m_event(std::move(event))
	    {}

	inline Event(PixelEvent&& event)
	    : m_event(std::move(event))
	    {}


	inline uint64_t time_of_arrival () const {
	    uint64_t t;
	    std::visit(overloaded{
		    [&t] (const PixelEvent& ev){
			t = ev.time_of_arrival();
		    },
		    [&t] (const auto& ev){
			t = ev.time_of_arrival();
		    },
		}, this->m_event);
	    return t;
	}
    };

    class EventCollection
    {
	const std::vector<Event> events;

    };


};

#endif
