#include "tailored-glossy.h"
	

PROCESS(glossy_test, "Glossy test");
AUTOSTART_PROCESSES(&glossy_test);
PROCESS_THREAD(glossy_test, ev, data)
{
	PROCESS_BEGIN();
	
	process_start(&tailored_glossy_process, NULL);
	
	PROCESS_END();
}


/** @} */
/** @} */
/** @} */
