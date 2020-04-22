/************************************************************************************
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation. Title to the
 * Material remains with Intel Corporation.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery of
 * the Materials, either expressly, by implication, inducement, estoppel or otherwise.
 ************************************************************************************/

#include "../include/Common_ut.hpp"
#include <string>
#include <vector>


using namespace std;

void Common_ut::SetUp()
{
	// Setup code
}

void Common_ut::TearDown()
{
	// TearDown code
}

TEST_F(Common_ut, readEnvVariable_ReadSuccess)
{

	std::string envVar = "";
	bool bRetVal = CCommon::getInstance().readEnvVariable("ReadRequest", envVar);

	EXPECT_EQ("MQTT_Export_RdReq", envVar);

}

TEST_F(Common_ut, readEnvVariable_ReadUnSuccess)
{

	std::string envVar = "";
	bool bRetVal = CCommon::getInstance().readEnvVariable("NotDefined", envVar);

	EXPECT_EQ(false, bRetVal);

}

TEST_F(Common_ut, readCommonEnvVariables_ReadSuccess)
{
	//Setting environment variable for testing purpose
	setenv("ReadRequest", "ReadRequest_UT", 0);
	setenv("WriteRequest", "WriteRequest_UT", 0);
	setenv("APP_VERSION", "APP_VERSION", 0);

	bool bRetVal = CCommon::getInstance().readCommonEnvVariables();

	EXPECT_EQ(true, bRetVal);

	// Un-set environment variable which was set for testing
	unsetenv("ReadRequest");
	unsetenv("WriteRequest");
	unsetenv("APP_VERSION");
}

// env variables are not set
TEST_F(Common_ut, readCommonEnvVariables_Invalid)
{

	bool bRetVal = CCommon::getInstance().readCommonEnvVariables();

	EXPECT_EQ(false, bRetVal);

}

// DevMode = FALSE
TEST_F(Common_ut, readCommonEnvVariables_DevModeFalse)
{
	//Setting environment variable for testing purpose
	setenv("ReadRequest", "ReadRequest_UT", 0);
	setenv("WriteRequest", "WriteRequest_UT", 0);
	setenv("APP_VERSION", "APP_VERSION", 0);
	setenv("DEV_MODE", "false", 1);

	bool bRetVal = CCommon::getInstance().readCommonEnvVariables();

	EXPECT_EQ(true, bRetVal);

	// Un-set environment variable which was set for testing
	unsetenv("ReadRequest");
	unsetenv("WriteRequest");
	unsetenv("APP_VERSION");
	setenv("DEV_MODE", "true", 1);
}

// DevMode = other than true and false
TEST_F(Common_ut, readCommonEnvVariables_DevModeOther)
{
	setenv("ReadRequest", "ReadRequest_UT", 0);
	setenv("WriteRequest", "WriteRequest_UT", 0);
	setenv("APP_VERSION", "APP_VERSION", 0);
	setenv("DEV_MODE", "other", 1);

	bool bRetVal = CCommon::getInstance().readCommonEnvVariables();

	EXPECT_EQ(true, bRetVal);

	unsetenv("ReadRequest");
	unsetenv("WriteRequest");
	unsetenv("APP_VERSION");
	setenv("DEV_MODE", "true", 1);
}

TEST_F(Common_ut, setStrReadRequest_SetstrCorrect)
{

	CCommon::getInstance().setStrReadRequest("SetStrForUT");

	string str_temp = CCommon::getInstance().getStrReadRequest();

	EXPECT_EQ("SetStrForUT", str_temp);

}

TEST_F(Common_ut, addTimestampsToMsg_InvMsg)
{

	std::string strTsReceivedFromMQTT;
	std::string message_Inv = "InvMsg";
	std::string tsMsgRcvdFromMQTT = "RecMsg";
	bool RetVal = true;


	CCommon::getInstance().getCurrentTimestampsInString(strTsReceivedFromMQTT);


	RetVal = CCommon::getInstance().addTimestampsToMsg(message_Inv, tsMsgRcvdFromMQTT, strTsReceivedFromMQTT);

	EXPECT_EQ(false, RetVal);

}