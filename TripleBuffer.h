// Triple buffer implementation
// Implementation is not lockless, but lock only required for pointer switching
// Note: if Producer is faster than Consumer, consumer will miss buffers
//       if the write buffer is only partially updated each time it is requested, consider using getClonedWriteBuffer

#pragma once
#include "arduino.h"

template <class T>
class TripleBuffer
{
public:
	TripleBuffer           (const TripleBuffer& other) = delete;			
	TripleBuffer& operator=(const TripleBuffer& other) = delete;

	TripleBuffer() : 
		_readIndex  (-1),
		_writeIndex ( 0),
		_newIndex   (-1),
		_isNewBuffer(false)
	{
		_mutex = xSemaphoreCreateMutex();
	}
	~TripleBuffer() {
		if (xSemaphoreTake(_mutex, (TickType_t)100))
		{
			_readIndex   = -1;
			_writeIndex  = -1;
			_newIndex    = -1;
			_isNewBuffer = false;
			xSemaphoreGive(_mutex);
		}
		vSemaphoreDelete(_mutex);
	}

	/// <summary>
	/// Gets the most recently written buffer
	/// </summary>
	/// <returns>Pointer to buffer of type T</returns>
	T* getReadBuffer()
	{
		if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
		{
			_isNewBuffer = false;
			_readIndex   = _newIndex;
			xSemaphoreGive(_mutex);
		}
		return (_readIndex >= 0) ? &_buffers[_readIndex] : nullptr;
	}

	/// <summary>
	/// Gets the most recently written buffer if a new one is available
	/// </summary>
	/// <returns>Pointer to buffer of type T if a buffer has been updated since last read. If not, returns nullptr</returns>
	T* getNewReadBuffer()
	{
		bool isNewBuffer = false;
		if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
		{
			isNewBuffer  = _isNewBuffer;
			_isNewBuffer = false;
			_readIndex   = _newIndex;
			xSemaphoreGive(_mutex);
		}
		return (_readIndex >= 0 && isNewBuffer) ? &_buffers[_readIndex] : nullptr;
	}

	/// <summary>
	/// Gets a buffer to write in. Release after the values have been set
	/// </summary>
	/// <returns>Pointer to buffer of type T</returns>
	T* getWriteBuffer() {
		return (_writeIndex >= 0) ? &_buffers[_writeIndex] : nullptr;
	}


	/// <summary>
	/// Gets a cloned value buffer to write in. 
	/// The initial values are cloned from the last written buffer. 
	//  This is requires a (time consuming) memcopy to be made, but is useful if buffer is only partially updated
	//  Release after the values have been set
	/// </summary>
	/// <returns>Pointer to buffer of type T</returns>
	T* getClonedWriteBuffer() {
		if (_writeIndex < 0) return nullptr;
		if (_newIndex   =>0) memcpy ( &_newIndex, &_writeIndex, sizeof(_newIndex) ); // no locking needed because we assume the read thread will not update struct			
		return &_buffers[_writeIndex];
	}

	/// <summary>
	/// Release the buffer to write in. Do this always after GetNewReadBuffer.
	/// </summary>
	void releaseWriteBuffer()
	{		
		if (xSemaphoreTake(_mutex, (TickType_t)portMAX_DELAY))
		{
			_newIndex    = _writeIndex;
			_isNewBuffer = true;
			switch (_writeIndex)
			{
			case 0: _writeIndex = (_readIndex == 1) ? 2 : 1; break;
			case 1: _writeIndex = (_readIndex == 2) ? 0 : 2; break;
			case 2: _writeIndex = (_readIndex == 0) ? 1 : 0; break;
			}
			xSemaphoreGive(_mutex);
		}
	}

private:   	
	int8_t            _readIndex;
	int8_t            _writeIndex;
	int8_t            _newIndex;
	bool              _isNewBuffer;
	T                 _buffers[3];	
	SemaphoreHandle_t _mutex = nullptr;	
};

