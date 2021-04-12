/* -*- C++ -*- */
/**************

Derived from:

ClickEncoderStream.h

Jun. 2016
Modified by Christophe Persoz and Rui Azevedo.
Based on keyStream.h developed by Rui Azevado.
and ClickEncoder library by Peter Dannegger.
https://github.com/christophepersoz/encoder

Sept. 2014 Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

quick and dirty keyboard driver
metaprog keyboard driver where N is the number of keys
all keys are expected to be a pin (buttons)
we can have reverse logic (pull-ups) by entering negative pin numbers
ex: -A0 means: pin A0 normally high, low when button pushed (reverse logic)

***/


#pragma once

#include <Arduino.h>
#include "AiEsp32RotaryEncoder.h"

#include <menuDefs.h>

namespace Menu {

	//emulate a stream based on clickEncoder movement returning +/- for every 'sensivity' steps
	//buffer not needer because we have an accumulator
	class RotaryClickEncoderStream :public menuIn {
	public:
		AiEsp32RotaryEncoder &enc; //associated hardware clickEncoder
		int oldPos;

		RotaryClickEncoderStream(AiEsp32RotaryEncoder &enc)
			:enc(enc)
		{
			flush();
		}

		// Return 1 when there is input available
		int available(void) {
			return enc.readEncoderValue() != oldPos || enc.ClickOccured(true);
		}

		// Set old pos. No idea whether this is sensible, but it needs an implementation as it cannot stay virtual...
		size_t write(uint8_t v) {
			oldPos = v;
			return 1;
		}

		// Read and consume
		int read(void)
		{
			return read(false);
		}

		// Read and do not consume
		int peek(void)
		{
			return read(true);
		}

		// Read RotaryClickEncoder 'char' output. A click will sent an enter code. Rotation will send an up or down code. When peek = false, the data is consumed
		int read(bool peek)
		{
			// When click occured, return enter code (highest priority)
			if (enc.ClickOccured(peek))
			{
				return options->navCodes[enterCmd].ch;
			}
			// Get encoder position
			int pos = enc.readEncoderValue();
			// Look for difference with respect to previous value
			int d = pos - oldPos;
			// Decrease: send downCode
			if (d <= -1)
			{
				if (!peek) oldPos--;
				return options->navCodes[downCmd].ch;//menu::downCode;
			}
			// Increase: send upCode
			if (d >= 1)
			{
				if (!peek) oldPos++;
				return options->navCodes[upCmd].ch;//menu::upCode;
			}
			// Nothing to do!
			return -1;
		}



		// Clear input buffer (e.g. reset ClickOccured and assume all rotation commands processed)
		void flush() {
			// Set oldPos to current encoder value
			oldPos = enc.readEncoderValue();
			// Read ClickOccured, by this resetting it...
			enc.ClickOccured();
		}

	
	};
}//namespace Menu

