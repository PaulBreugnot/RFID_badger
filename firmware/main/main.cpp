/* Main
*/

#include "nvs_manager.h"
// #include "display.h"

extern "C" {
   void app_main();
   #include "display.h"
}

void app_main()
{
	init_display();
}
