#ifndef TIMER_H
#define TIMER_H

class Timer
{
	public:
		Timer(int i_currentTime, int i_maxTime);
		Timer(const Timer& i_timer);
		~Timer();
		void increment(int i_increment);
		bool doAction(bool i_reset = true);
		void setCurrentTime(int i_currentTime){m_currentTime = i_currentTime;}
	private:

		int m_maxTime;
		int m_currentTime;
};

#endif //TIMER_H
