#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	class Timer
	{
	public:
		Timer(void);
		~Timer(void);
		
		void reset();
		float time() const;

		static void sleep(uint32 ms);

	private:
		uint64 freq;
		uint64 t;
	};


	inline int64 CYCLE_COUNT()
	{
#if defined(_M_IX86)
		__asm rdtsc;
#else
		return 1; //TODO
#endif
	}

	struct CycleCounter
	{
		INLINE CycleCounter()
			: m_cycles(0)
			, m_started(false)
		{
		}

		INLINE void start()
		{
			if(m_started==false)
			{
				m_started = true;
				m_cycles -= CYCLE_COUNT();
			}
		}
		
		INLINE void stop()
		{			
			if( m_started )
			{
				m_cycles += CYCLE_COUNT();
				m_started = false;
			}
		}

		INLINE void reset()
		{
			m_cycles = 0;
			m_started = false;
		}

		INLINE int64 cycles()
		{
			if( m_started == true )
			{
				stop();

				int64 res = m_cycles;

				start();

				return res;
			}
			else
			{
				return m_cycles;
			}
		}
		
		int64	m_cycles;
		bool	m_started;
	};

	struct ScopeCycles
	{
		INLINE ScopeCycles(CycleCounter& _ctr)
			: ctr(_ctr)
		{
			ctr.start();
		}
		INLINE ~ScopeCycles()
		{
			ctr.stop();
		}
		
		CycleCounter& ctr;

	private:
		ScopeCycles& operator=(const ScopeCycles& rhs);
	};

	/// @}

}


