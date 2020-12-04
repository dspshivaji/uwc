/************************************************************************************
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation. Title to the
 * Material remains with Intel Corporation.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery of
 * the Materials, either expressly, by implication, inducement, estoppel or otherwise.
 ************************************************************************************/

/*** KPIAppConfigMgr.hpp handles the configuration of kpi app */

#ifndef INCLUDE_KPIAPPCONFIGMGR_HPP_
#define INCLUDE_KPIAPPCONFIGMGR_HPP_

#include "ControlLoopHandler.hpp"

/** class for kpi app configuration*/
class CKPIAppConfig
{
	uint32_t m_uiExecTimeMin; /** Execution time in minutes*/
	bool m_bIsMQTTModeApp; /** mqtt mode on or not(true or false*/
	bool m_bIsRTModeForPolledPoints; /** RT mode for polled points(true or false) */
	bool m_bIsRTModeForWriteOp; /** RT mode for write operation(true or false) */

	CControlLoopMapper m_oCtrlLoopMap; //object of class CControlLoopMapper

	/** Default constructor*/
	CKPIAppConfig(): m_uiExecTimeMin{0}, m_bIsMQTTModeApp{false}, 
		m_bIsRTModeForPolledPoints{true}, m_bIsRTModeForWriteOp{true}, m_oCtrlLoopMap{}
	{}

	/** delete copy and move constructors and assign operators*/
	CKPIAppConfig(const CKPIAppConfig&) = delete;	 			// Copy construct
	CKPIAppConfig& operator=(const CKPIAppConfig&) = delete;	// Copy assign

public:
	static CKPIAppConfig& getInstance()
	{
		static CKPIAppConfig _self;
		return _self;
	}
	bool parseYMLFile(const std::string &a_sFileName);
	uint32_t getExecutionTime() {return m_uiExecTimeMin;}
	bool isMQTTModeOn() {return m_bIsMQTTModeApp;}
	bool isRTModeForPolledPoints() {return m_bIsRTModeForPolledPoints;}
	bool isRTModeForWriteOp() {return m_bIsRTModeForWriteOp;}

	CControlLoopMapper& getControlLoopMapper() {return m_oCtrlLoopMap;}
};

#endif 
