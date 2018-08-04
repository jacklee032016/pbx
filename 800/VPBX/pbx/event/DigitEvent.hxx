#ifndef __DIGIT_EVENT_HXX__
#define __DIGIT_EVENT_HXX__
/*
 * $Id: DigitEvent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PbxEvent.hxx"

namespace Assist
{

class DigitEvent : public PbxEvent
{
	public:
		DigitEvent();

		/// Create given an associated fifo.
		DigitEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo);

		virtual ~DigitEvent();

		// The name of the extending class.
		virtual const char* const name() const 
		{
			return "DIGIT-MSG";
		};

	protected:

	private:
		DigitEvent(const DigitEvent &);
		const DigitEvent & operator=(const DigitEvent &);

	public:
		char 			myDigit;
};
 
}

#endif

