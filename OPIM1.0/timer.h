#pragma once


class Timer
{
private:
	LARGE_INTEGER __StartTime, __LastTime, __EndTime, __TimeFreq;
	const char* __processName;
public:
	Timer()
	{
		QueryPerformanceFrequency(&__TimeFreq);
		QueryPerformanceCounter(&__StartTime);
		__LastTime = __StartTime, __EndTime = __StartTime;
		__processName = "Unnamed";
	}

	explicit Timer(const char* processName)
	{
		QueryPerformanceFrequency(&__TimeFreq);
		QueryPerformanceCounter(&__StartTime);
		__LastTime = __StartTime, __EndTime = __StartTime;
		__processName = processName;
	}

	/// Refresh time
	void refresh_time()
	{
		QueryPerformanceCounter(&__StartTime);
		__LastTime = __StartTime, __EndTime = __StartTime;
	}

	/// Record current time
	void record_current_time()
	{
		__LastTime = __EndTime;
		QueryPerformanceCounter(&__EndTime);
	};
	/// Get the operation time from the last recorded moment
	double get_operation_time()
	{
		this->record_current_time();
		return (double)(__EndTime.QuadPart - __LastTime.QuadPart) / __TimeFreq.QuadPart;
	}

	/// Print the operation time from the last recorded moment
	void log_operation_time()
	{
		const double duration = this->get_operation_time();
		std::cout << "->Time used (sec): " << duration << std::endl;
	}

	/// Print the operation time from the last recorded moment with a given name
	void log_operation_time(const char* operationName)
	{
		const double duration = this->get_operation_time();
		std::cout << "->Time used (sec) for operation [" << operationName << "]: " << duration << std::endl;
	}

	/// Get the total time from the beginning
	double get_total_time()
	{
		this->record_current_time();
		return (double)(__EndTime.QuadPart - __StartTime.QuadPart) / __TimeFreq.QuadPart;
	}

	/// Print the total time from the beginning
	void log_total_time()
	{
		const double duration = this->get_total_time();
		std::cout << "--->Time used (sec) for process [" << __processName << "]: " << duration << std::endl;
	}

	/// Print the total time from the beginning to the last recorded moment
	void log_sub_total_time() const
	{
		const double duration = (double)(__EndTime.QuadPart - __StartTime.QuadPart) / __TimeFreq.QuadPart;
		std::cout << "--->Time used (sec) for process [" << __processName << "]: " << duration << std::endl;
	}
};

using TTimer = Timer;
using PTimer = std::shared_ptr<Timer>;
