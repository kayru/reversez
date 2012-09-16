#pragma once

#ifdef RUSH_INSIGHT

	#include <insight.h>
	#define INSIGHT_INITIALIZE	Insight::initialize();
	#define INSIGHT_TERMINATE	Insight::terminate();
	#define INSIGHT_SCOPE(name) Insight::Scope __insight_scope(name);

#else //RUSH_INSIGHT

	#define INSIGHT_INITIALIZE
	#define INSIGHT_TERMINATE
	#define INSIGHT_SCOPE(name)

#endif //RUSH_INSIGHT

