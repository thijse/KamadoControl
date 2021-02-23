#pragma once
#include "arduino.h"

class AutoResetEvent
{
public:
	AutoResetEvent(const AutoResetEvent& other)            = delete;
	AutoResetEvent& operator=(const AutoResetEvent& other) = delete;

	/// <summary>
	/// start blocked (waiting for signal)
	/// </summary>
	AutoResetEvent() 
	{
		_binary = xSemaphoreCreateBinary();
		xSemaphoreGive(_binary);                
	}
	
	/// <summary>
	/// start blocked or signalled. 
	/// </summary>
	/// <param name="set">If true, first Wait will directly continue</param>
	AutoResetEvent(bool set) 
	{
		_binary = xSemaphoreCreateBinary();
		xSemaphoreGive(_binary);
		if (set) { Set(); }
	}
	
	~AutoResetEvent() 
	{
		vSemaphoreDelete(_binary);
	}

	/// <summary>
	/// Sets signal, will unblock Wait function once
	/// </summary>
	void Set()
	{
		// give binary mutex setting it to 1 so that it will not block in wait function. 
		if (uxSemaphoreGetCount(_binary) == 0) {xSemaphoreGive(_binary); }
	}
	
	/// <summary>
	/// Wait function. Blocks until signal 
	/// </summary>
	/// <returns></returns>
	void Wait()
	{		
		xSemaphoreTake(_binary, (TickType_t)portMAX_DELAY);
	}

private:
	AutoResetEvent(const AutoResetEvent& other);				// = delete;
	AutoResetEvent& operator=(const AutoResetEvent& other);	// = delete;
	
	SemaphoreHandle_t _mutex  = NULL;	
	SemaphoreHandle_t _binary = NULL;
};

