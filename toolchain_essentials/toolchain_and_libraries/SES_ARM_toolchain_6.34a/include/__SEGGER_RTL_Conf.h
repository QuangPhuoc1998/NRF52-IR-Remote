/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
*                           www.segger.com                           *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef __SEGGER_RTL_CONF_H
#define __SEGGER_RTL_CONF_H

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#if defined(__ARM_ARCH_ISA_ARM) || defined(__ARM_ARCH_ISA_THUMB)
  //
  // GNU C doesn't define __ARM_ACLE but does set the feature-test macros,
  // so use the ISA macros, one of which must be defined.
  //
  #include "__SEGGER_RTL_Arm_Conf.h"
#elif defined(__riscv)
  #include "__SEGGER_RTL_RISCV_Conf.h"
#elif defined(__XC16__)
  #include "__SEGGER_RTL_XC16_Conf.h"
#elif defined(_MSC_VER)
  #include "__SEGGER_RTL_MSVC_Conf.h"
#else
  #error Cannot determine configuration from standard definitions!
#endif

#endif

/*************************** End of file ****************************/
