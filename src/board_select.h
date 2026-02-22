#pragma once

#if defined(BOARD_UNO)
  #include "board_uno.h"
#elif defined(BOARD_MEGA2560)
  #include "board_mega2560.h"
#else
  #error "Define BOARD_UNO or BOARD_MEGA2560 in compiler flags"
#endif