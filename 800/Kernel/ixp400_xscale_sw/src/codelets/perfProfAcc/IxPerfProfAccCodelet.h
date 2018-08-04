/*
 * @file IxPerfProfAccCodelet.h
 *
 * @date June-18-2003
 *
 * @brief This is the header file for the PerfProf Access Compenent Codelet.
 */

/**
 * @par
 * IXP400 SW Release Crypto version 2.0
 * 
 * -- Intel Copyright Notice --
 * 
 * @par
 * Copyright 2002-2005 Intel Corporation All Rights Reserved.
 * 
 * @par
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or its suppliers or licensors.  Title to the Material remains with
 * Intel Corporation or its suppliers and licensors.
 * 
 * @par
 * The Material is protected by worldwide copyright and trade secret laws
 * and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way except in accordance with the
 * applicable license agreement .
 * 
 * @par
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel, except in accordance with the
 * applicable license agreement.
 * 
 * @par
 * Unless otherwise agreed by Intel in writing, you may not remove or
 * alter this notice or any other notice embedded in Materials by Intel
 * or Intel's suppliers or licensors in any way.
 * 
 * @par
 * For further details, please see the file README.TXT distributed with
 * this software.
 * 
 * @par
 * -- End Intel Copyright Notice --
 */

/**
 * @defgroup Codelets IXP400 Codelets
 *
 * @brief IXP400 PerfProfAcc Codelet
 *
 * @{
 */

/**
 * @ingroup Codelets
 *
 * @defgroup IxPerfProfAccCodelet IXP400 PerfProf Access Codelet
 *           (IxPerfProfAccCodelet) API
 *
 * @brief IXP400 codelet PerfProf Access component API
 *
 * Functionality of the PerfProf Access Codelet
 *
 * The codelet shall demonstrate how the Performance Profiling utility 
 * can be used for profiling purposes.
 * <UL>
 *
 * <LI>The different implementations are demonstrated.
 *   <UL>
 *  <LI>Help - Lists down how the codelet can be used.
 *  <LI>Demo All - A non user configurable demonstration on how to use the APIs
 *  <LI>Bus Pmu North Mode - Profiling of the north bus activities. Enables 
 *                           user to select events that they wish to monitor.
 *  <LI>Bus Pmu South Mode - Profiling of the south bus activities. Enables 
 *                           user to select events that they wish to monitor.
 *  <LI>Bus Pmu Sdram Mode - Profiling of the sdram bus activities. Enables 
 *                           user to select events that they wish to monitor.
 *  <LI>Bus Pmu PMSR Get Mode - Get the last slave or master to access the bus.
 *  <LI>Xscale PMU Event Sampling - Event Sampling of Xscale PMU. Enables 
 *                                  user to select event and sampling rate 
 *                                  that they wish to sample.
 *  <LI>Xscale PMU Time Sampling - Time Sampling of Xscale PMU. Enables user 
 *                                 to select clock count mode and number of events
 *                                     and rate they wish to sample.
 *  <LI>Xscale PMU Event Counting - Event counting of Xscale PMU. Enables 
 *                                  users to select events that they wish to
 *                                  count or monitor.
 *  <LI>Xcycle Measurement - Measurement of cycle idle time. i.e when the cycles
 *                           are not being used to process anything.
 *   </UL>
 * </UL>
 *
 * <UL>
 * <b> User Guide </b><br>
 * Users will be able to start the codelet by calling ixPerfProfAccCodeletMain
 * and passing in up to 10 parameters. The parameters are represented in the
 * following order:
 * <pre>
 * <i>
 * <b> Help Mode </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_HELP
 *     Set the rest of the parameters to 0. 
 * <b> All functionalities mode </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_ALL 
 *     Set the rest of the parameters to 0.
 * <b> Bus PMU north mode </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_NORTH_MODE
 *     param1 - Select proper PEC1 value from main header file.
 *     param2 - Select proper PEC2 value from main header file.
 *     param3 - Select proper PEC3 value from main header file.
 *     param4 - Select proper PEC4 value from main header file.
 *     param5 - Select proper PEC5 value from main header file.
 *     param6 - Select proper PEC6 value from main header file.
 *     param7 - Select proper PEC7 value from main header file.
 *     Set the rest of the parameters to 0.
 * <b> Bus PMU south mode </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_SOUTH_MODE
 *     param1 - Select proper PEC1 value from main header file.
 *     param2 - Select proper PEC2 value from main header file.
 *     param3 - Select proper PEC3 value from main header file.
 *     param4 - Select proper PEC4 value from main header file.
 *     param5 - Select proper PEC5 value from main header file.
 *     param6 - Select proper PEC6 value from main header file.
 *     param7 - Select proper PEC7 value from main header file.
 *     Set the rest of the parameters to 0.
 * <b> Bus PMU sdram mode </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_SDRAM_MODE
 *     param1 - Select proper PEC1 value from main header file.
 *     param2 - Select proper PEC2 value from main header file.
 *     param3 - Select proper PEC3 value from main header file.
 *     param4 - Select proper PEC4 value from main header file.
 *     param5 - Select proper PEC5 value from main header file.
 *     param6 - Select proper PEC6 value from main header file.
 *     param7 - Select proper PEC7 value from main header file.
 *     Set the rest of the parameters to 0.
 * <b> Bus PMU PMSR Get </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_PMSR_GET
 *     Set the rest of the parameters to 0.
 * <b> XScale PMU Event Sampling </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_DEMO_XSCALE_PMU_EVENT_SAMPLING
 *     param1 - Number of events
 *     param2 - Event 1
 *     param3 - Sampling rate of Event 1
 *     param4 - Event 2 
 *     param5 - Sampling rate of Event 2
 *     param6 - Event 3
 *     param7 - Sampling rate of Event 3
 *     param8 - Event 4
 *     param9 - Sampling rate of Event 4
 * <b> XScale PMU Time Sampling </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_DEMO_XSCALE_PMU_TIME_SAMPLING
 *     param1 - Sampling rate.
 *     param2 - Clock count divider.
 *     Set the rest of the parameters to 0.
 * <b> XScale PMU Event Counting </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_DEMO_XSCALE_PMU_EVENT_COUNTING
 *     param1 - Clock count divider.
 *     param2 - Number of events.
 *     param3 - Event 1.
 *     param4 - Event 2.
 *     param5 - Event 3.
 *     param6 - Event 4.
 *     Set the rest of the parameters to 0.
 * <b> Xcycle Measurement </b>
 *     Mode - Select IX_PERFPROF_ACC_CODELET_MODE_XCYCLE
 *     param1 - Number of runs required.
 *     Set the rest of the parameters to 0.
 * </i>
 * </pre>
 * </UL>
 *
 * <b> VxWorks User Guide </b><br>
 * ixPerfProfAccCodeletMain() function is used as a single point of execution for
 * PerfProfAcc Codelet. It allows user to enter selection for different type
 * of supported operations as described above.
 * 
 * <pre>
 *  <i> Usage :
 *      >ixPerfProfAccCodeletMain (mode, param1, param2, param3, param4, param5, param6, 
 *                            param7, param8, param9)
 *      Where mode and params are described above.
 *  </i>
 * </pre>
 *
 * <b> Linux User Guide </b><br>
 * ixPerfProfAccCodeletMain() function is used as a single point of execution for
 * PerfProfAcc Codelet. It allows user to enter selection for different type
 * of supported operations as described above.
 *
 * <pre>
 *  <i> Usage :
 *      >insmod ixp400_codelets_perfProfAcc.o \
 *       mode=<Mode> \
 *       param1=<param1> \
 *       param2=<param2> \
 *       param3=<param3> \
 *       param4=<param4> \
 *       param5=<param5> \
 *       param6=<param6> \
 *       param7=<param7> \
 *       param8=<param8> \
 *       param9=<param9> \
 *       
 *      Where Parameter X are as described above.
 *  </i>
 * </pre>
 *
 * @{
 */

#ifndef IxPerfProfAccCodelet_H
#define IxPerfProfAccCodelet_H
#endif

#include "IxPerfProfAcc.h"
#include "IxOsal.h"


/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PSS_MASK
 *
 * @brief Masks out PSS portion of the PMSR register.
 */
#define    PSS_MASK                  0x3f000

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PMSR
 *
 * @brief Defines value for expansion bus as the slave on ARBS that was previously 
 *        accessing the AHBS.
 */
#define    EXPANSION_BUS             0x1000

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PMSR
 *
 * @brief Defines value for sdram controller as the slave on ARBS that was previously
 *        accessing the AHBS.
 */
#define    SDRAM_CONTROLLER          0x2000         

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PMSR
 *
 * @brief Defines value for pci as the slave on ARBS that was previously
 *        accessing the AHBS.
 */
#define    PCI                       0x4000

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PMSR
 *
 * @brief Defines value for queue manager as the slave on ARBS that was previously
 *        accessing the AHBS.
 */
#define    QUEUE_MANAGER             0x8000

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @def PMSR
 *
 * @brief Defines value for ahb-apb as the slave on ARBS that was previously
 *        accessing the AHBS.
 */
#define    AHB_APB_BRIDGE            0x10000

/**
 * @ingroup IxPerfProfAccCodelet
 * 
 * @enum IxPerfProfAccCodeletMode
 *
 * @brief Contains selection of mode to be used when calling the main API. 
 */
typedef enum
{
        IX_PERFPROF_ACC_CODELET_MODE_HELP = 0, /**< Select help mode*/
        IX_PERFPROF_ACC_CODELET_MODE_ALL, /**< Select all mode*/
        IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_NORTH_MODE, /**< Select north bus pmu mode*/
        IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_SOUTH_MODE, /**< Select south bus pmu mode*/
        IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_SDRAM_MODE, /**< Select sdram mode*/
        IX_PERFPROF_ACC_CODELET_MODE_XSCALE_PMU_EVENT_SAMPLING, /**< Select xscale pmu event sampling mode*/
        IX_PERFPROF_ACC_CODELET_MODE_XSCALE_PMU_TIME_SAMPLING, /**< Select xscale pmu time sampling mode*/
        IX_PERFPROF_ACC_CODELET_MODE_XSCALE_PMU_EVENT_COUNTING, /**< Select xscale pmu event counting mode*/
        IX_PERFPROF_ACC_CODELET_MODE_XCYCLE, /**< Select xcycle mode*/
        IX_PERFPROF_ACC_CODELET_MODE_BUS_PMU_PMSR_GET /**< Select bus pmu pmsr get mode*/
} IxPerfProfAccCodeletMode;

/**
 * @ingroup IxPerfProfAccCodelet
 *
 * @fn ixPerfProfAccCodeletMain (IxPerfProfAccCodeletMode mode, 
 *                               UINT32 param1, 
 *                               UINT32 param2, 
 *                               UINT32 param3, 
 *                               UINT32 param4,
 *                               UINT32 param5, 
 *                               UINT32 param6, 
 *                               UINT32 param7,
 *                               UINT32 param8, 
 *                               UINT32 param9)
 *
 * @brief To initialize and run the selected functions with the choice of 
 *        settings by the user.
 *
 * @param "IxPerfProfAccCodeletMode [in] mode" - The mode required.
 * @param "UINT32 [in] param1" - The first parameter as per the mode chosen.
 * @param "UINT32 [in] param2" - The second parameter as per the mode chosen.
 * @param "UINT32 [in] param3" - The third parameter as per the mode chosen.
 * @param "UINT32 [in] param4" - The fourth parameter as per the mode chosen.
 * @param "UINT32 [in] param5" - The fifth parameter as per the mode chosen.
 * @param "UINT32 [in] param6" - The sixth parameter as per the mode chosen.
 * @param "UINT32 [in] param7" - The seventh parameter as per the mode chosen.
 * @param "UINT32 [in] param8" - The eighth parameter as per the mode chosen.
 * @param "UINT32 [in] param9" - The ninth parameter as per the mode chosen.
 *
 * @return IxPerfProfAccStatus
 *         @li IX_PERFPROF_ACC_STATUS_SUCCESS 
 *         @li IX_PERFPROF_ACC_STATUS_FAIL
 *
 */
PUBLIC void
ixPerfProfAccCodeletMain (IxPerfProfAccCodeletMode mode, 
                          UINT32 param1, 
                          UINT32 param2,
                          UINT32 param3, 
                          UINT32 param4, 
                          UINT32 param5, 
                          UINT32 param6,
                          UINT32 param7, 
                          UINT32 param8, 
                          UINT32 param9);
