#include "event.h"

#ifndef _EVENTS_INC
#define _EVENTS_INC

namespace Tilkal
{
	typedef TMyArray<CEvent,256> CEventsBase;

	class CEvents:public CEventsBase
	{
	public:
		int first;
		int New(ETypeEvent type, int64 UID, int64 t)
		{
			return New(type,UID,t,0,0,0);	
		}
		int New(ETypeEvent type, int64 UID, int64 t, int action)
		{
			return New(type,UID,t,action,0,0);
		}
		int New(ETypeEvent type, int64 UID, int64 t, int action, int64 data1)
		{
			return New(type,UID,t,action,data1,0);
		}
		int New(ETypeEvent type, int64 UID, int64 t, int action, int64 data1, int64 data2);
		CEvent & First()
		{
			return t[first];
		}
		void RemoveFirst()
		{
			int f=t[first].next;
			Remove(first);
			first=f;
		}
		CEvents():CEventsBase()
		{
			first=NOTHING;
		}
	};
}

#ifdef _DEFINITIONS
Tilkal::CEvents Event;
#else
extern Tilkal::CEvents Event;
#endif

#endif
