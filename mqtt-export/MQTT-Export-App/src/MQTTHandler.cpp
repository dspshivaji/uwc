/************************************************************************************
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation. Title to the
 * Material remains with Intel Corporation.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery of
 * the Materials, either expressly, by implication, inducement, estoppel or otherwise.
 ************************************************************************************/

#include "MQTTHandler.hpp"
#include <vector>
#include "cjson/cJSON.h"
#include <semaphore.h>


sem_t g_semaphoreRespProcess;
std::mutex g_mqttSubMutexLock;
std::mutex g_mqttPublishMutexLock;

// constructor
CMQTTHandler::CMQTTHandler(std::string strPlBusUrl) :
		publisher(strPlBusUrl, CLIENTID), subscriber(strPlBusUrl, SUBSCRIBERID), ConfigState(MQTT_PUBLISHER_CONNECT_STATE), subConfigState(
				MQTT_SUSCRIBER_CONNECT_STATE) {
	try {

		//connect options for async subscriber
		mqtt::message willmsg("MQTTConfiguration", LWT_PAYLOAD, QOS, true);
		mqtt::will_options will(willmsg);
		conopts.set_will(will);
		conopts.set_keep_alive_interval(20);
		conopts.set_clean_session(true);
		conopts.set_automatic_reconnect(1, 10);

		//connect options for sync publisher/client
		syncConnOpts.set_keep_alive_interval(20);
		syncConnOpts.set_clean_session(true);
		syncConnOpts.set_automatic_reconnect(1, 10);

#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
		initSem();
#endif

		publisher.set_callback(syncCallback);
		subscriber.set_callback(callback);

		connectSubscriber();

		CLogger::getInstance().log(DEBUG, LOGDETAILS("MQTT initialized successfully"));


	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
	}
}

// function to get single instance of this class
CMQTTHandler& CMQTTHandler::instance() {

	static bool bPlBusUrl = false;
	const char *mqttBrokerURL = NULL;

	string strPlBusUrl;

	if (!bPlBusUrl) {
		/// get the platform bus env variable
		mqttBrokerURL = std::getenv("MQTT_URL_FOR_EXPORT");
		/// check for null
		if (NULL == mqttBrokerURL) {
			CLogger::getInstance().log(ERROR, LOGDETAILS(":MQTT_URL_FOR_EXPORT Environment variable is not set"));
			std::cout << __func__ << ":" << __LINE__ << " Error : MQTT_URL_FOR_EXPORT Environment variable is not set" <<  std::endl;
			exit(EXIT_FAILURE);
		} else {
			strPlBusUrl.assign(mqttBrokerURL);
			CLogger::getInstance().log(DEBUG, LOGDETAILS(":MQTT_URL_FOR_EXPORT Environment variable is set to : "
					+ strPlBusUrl));
			bPlBusUrl = true;
		}
	}

	static CMQTTHandler handler(mqttBrokerURL);
	return handler;
}

bool CMQTTHandler::connect() {

	bool bFlag = true;
	try {

		publisher.connect(syncConnOpts);
	    std::cout << __func__ << ":" << __LINE__ << " MQTT publisher connected with MQTT broker" << std::endl;

	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;

		bFlag = false;
	}
	return bFlag;
}

#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
bool CMQTTHandler::getMsgFromQ(stMsgData &a_msg) {
	CLogger::getInstance().log(DEBUG, "Pre-publish Q msgs count: " + m_qMsgData.size());

	bool bRet = true;
	try {
		std::lock_guard<std::mutex> lock(m_mutexMsgQ);
		/// Ensure that only on thread can execute at a time
		if (false == m_qMsgData.empty()) {
			a_msg = m_qMsgData.front();
			m_qMsgData.pop();
		} else {
			bRet = false;
		}
	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;

		bRet = false;
	}
	return bRet;
}

bool CMQTTHandler::pushMsgInQ(const stMsgData &a_msg) {

	bool bRet = true;
	try {
		/// Ensure that only on thread can execute at a time
		std::lock_guard<std::mutex> lock(m_mutexMsgQ);
		m_qMsgData.push(a_msg);

		CLogger::getInstance().log(DEBUG, "Total msgs pushed in publish Q : " + m_qMsgData.size());
	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
		bRet = false;
	}
	return bRet;

}

void CMQTTHandler::postPendingMsgsThread() {
	CLogger::getInstance().log(DEBUG, "Starting thread to publish pending data");
	bool bDoRun = false;
	try {
		stMsgData msg;
		do {
				if (false == publisher.is_connected()) {
					bDoRun = false;
					break;
				}
				if (false == getMsgFromQ(msg)) {
					CLogger::getInstance().log(DEBUG, "No msgs to send, Q is empty, stopping thread");
					bDoRun = false;
					break;
				}

				bDoRun = true;
				publish(msg.m_sMsg, msg.m_sTopic, msg.m_iQOS, true);
#ifdef PERFTESTING
				m_uiQReqTried++;
#endif
		} while (true == bDoRun);
	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
#ifdef PERFTESTING
		CMQTTHandler::m_ui32PublishSkipped++;
#endif
	}
	CLogger::getInstance().log(DEBUG, "--Stopping thread to publish pending data");
}

void CMQTTHandler::postPendingMsgs() {
	// Create a new thread to post messages to MQTT
	std::thread { std::bind(&CMQTTHandler::postPendingMsgsThread,
			std::ref(*this)) }.detach();
}
#endif

Mqtt_Config_state_t CMQTTHandler::getMQTTConfigState() {
	return ConfigState;
}

void CMQTTHandler::setMQTTConfigState(Mqtt_Config_state_t tempConfigState) {
	ConfigState = tempConfigState;
}

bool CMQTTHandler::publish(std::string a_sMsg, const char *topic, int qos) {

	static bool bIsFirst = true;
	if (true == bIsFirst) {
		connect();
		bIsFirst = false;
	}

	std::string sTopic(topic);
	try {
		publish(a_sMsg, sTopic, qos);
	} catch (const mqtt::exception &exc) {
#ifdef PERFTESTING
		m_ui32PublishExcep++;
#endif
		CLogger::getInstance().log(FATAL, LOGDETAILS(exc.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << exc.what() << std::endl;
	}
	return false;
}

bool CMQTTHandler::publish(std::string &a_sMsg, std::string &a_sTopic, int &a_iQOS,
		bool a_bFromQ) {

	try {
		std::lock_guard<std::mutex> lock(mqttMutexLock);

		// Check if topic is blank
		if (true == a_sTopic.empty()) {
			if (true == a_sMsg.empty()) {
				CLogger::getInstance().log(ERROR, LOGDETAILS("Blank topic and blank Message"));
				std::cout << __func__ << ":" << __LINE__ << " Error : Blank topic and blank Message" <<  std::endl;
			} else {
				CLogger::getInstance().log(ERROR, LOGDETAILS("Blank topic. Message not posted"));
				std::cout << __func__ << ":" << __LINE__ << " Error : Blank topic. Message not posted" <<  std::endl;
			}
			return false;
		}

#ifdef PERFTESTING
		CMQTTHandler::m_ui32PublishReq++;
#endif

		if(false == publisher.is_connected()) {
			//std::cout << __func__ << ":" << __LINE__ << " Failed to publish msg on MQTT for topic: " << a_sTopic << " , msg: " << a_sMsg << std::endl;

			CLogger::getInstance().log(ERROR, LOGDETAILS("MQTT publisher is not connected with MQTT broker" + std::to_string(a_iQOS)));
			CLogger::getInstance().log(ERROR, LOGDETAILS("Failed to publish msg on MQTT : " + a_sMsg));
#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
			pushMsgInQ(stMsgData(a_sMsg, a_sTopic, a_iQOS));
#endif
#ifdef PERFTESTING
			m_ui32ConnectionLost++;
			m_ui32PublishFailed++;
#endif
			return false;
		}

		publisher.publish(mqtt::message(a_sTopic, a_sMsg, a_iQOS, false));
		CLogger::getInstance().log(DEBUG, LOGDETAILS("Published message on MQTT broker successfully with QOS:"
									+ std::to_string(a_iQOS)));
		return true;

	} catch (const mqtt::exception &exc) {
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << exc.what() << std::endl;
		if (false == a_bFromQ) {
#ifdef PERFTESTING
			m_ui32PublishStrExcep++;
#endif
#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
			pushMsgInQ(stMsgData(a_sMsg, a_sTopic, a_iQOS));
#endif
		} else {
#ifdef PERFTESTING
			m_ui32PublishExcep++;
#endif
		}

		CLogger::getInstance().log(FATAL, LOGDETAILS(exc.what()));
	}
	return false;
}

#ifdef PERFTESTING
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishReq(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishReqErr(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32Published(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishFailed(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32ConnectionLost(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32Connection(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishSkipped(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32SubscribeSkipped(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishExcep(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishReqTimeOut(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32Disconnected(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishStrReq(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishStrReqErr(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32PublishStrExcep(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32DelComplete(0);
std::atomic<uint32_t> CMQTTHandler::m_ui32MessageArrived(0);
std::atomic<uint32_t> CMQTTHandler::m_uiQReqTried(0);
std::atomic<uint32_t> CMQTTHandler::m_uiSubscribeQReqTried(0);

void CMQTTHandler::printCounters()
{
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req rcvd: " + std::to_string(m_ui32PublishReq)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req err: "  + std::to_string(m_ui32PublishReqErr)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req sendmsg excep: "  + std::to_string(m_ui32PublishSkipped)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req publish excep: "  + std::to_string(m_ui32PublishExcep)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req published: "  + std::to_string(m_ui32Published)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req publish failed: "  + std::to_string(m_ui32PublishFailed)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req publish timeout: "  + std::to_string(m_ui32PublishReqTimeOut)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req during no connection: "  + std::to_string(m_ui32Disconnected)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req conn lost: " + std::to_string(m_ui32ConnectionLost)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Req conn done: " + std::to_string(m_ui32Connection)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("*****Str Req: " + std::to_string(m_ui32PublishStrReq)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("*****Str Req err: " + std::to_string(m_ui32PublishStrReqErr)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("*****Str Req excep: " + std::to_string(m_ui32PublishStrExcep)));
#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
	CLogger::getInstance().log(DEBUG, LOGDETAILS("----Pending Q Size: " + std::to_string(instance().m_qMsgData.size())));
#endif
	CLogger::getInstance().log(DEBUG, LOGDETAILS("++++Req posted from Q: " + std::to_string(m_uiQReqTried)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("$$$$Delivery completed: " + std::to_string(m_ui32DelComplete)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscriber tried to publish message:" + std::to_string(m_uiSubscribeQReqTried)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscriber skipped publishing message:" + std::to_string(m_ui32SubscribeSkipped)));
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscriber received messages:" + std::to_string(m_ui32MessageArrived)));
}
#endif

Mqtt_Sub_Config_state_t CMQTTHandler::getMQTTSubConfigState() {
	return subConfigState;
}

void CMQTTHandler::setMQTTSubConfigState(Mqtt_Sub_Config_state_t tempConfigState) {
	subConfigState = tempConfigState;
}

bool CMQTTHandler::initSem()
{
	int ok = sem_init(&g_semaphoreRespProcess, 0, 0 /* Initial value of zero*/);
	if (ok == -1) {
	   CLogger::getInstance().log(ERROR, LOGDETAILS("could not create unnamed semaphore, exiting"));
	   std::cout << __func__ << ":" << __LINE__ << " Error : could not create unnamed semaphore, exiting" <<  std::endl;
	   exit(0);
	}
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Sempaphores initialized successfully"));

	return true;
}

bool CMQTTHandler::subscribeToTopics() {

	//get list of topics from topic mapper
	std::vector<std::string> vMqttEnvTopics;
	vMqttEnvTopics.push_back("mqtt_SubReadTopic");
	vMqttEnvTopics.push_back("mqtt_SubWriteTopic");

	std::vector<std::string> vMqttTopics;

	try
	{
		for (auto &envTopic : vMqttEnvTopics) {
			const char* env_pubWriteTopic = std::getenv(envTopic.c_str());
			if(env_pubWriteTopic == NULL) {
				CLogger::getInstance().log(ERROR, LOGDETAILS(envTopic + " Environment Variable is not set"));
				std::cout << __func__ << ":" << __LINE__ << " Error : " + envTopic + " Environment Variable is not set" <<  std::endl;
				continue;
			}
			vMqttTopics.push_back(env_pubWriteTopic);
		}

		for (auto &topic : vMqttTopics) {
			if(! topic.empty()) {
				CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscribing topic : " + topic));
				subscriber.subscribe(topic, QOS, nullptr, listener);
			}
		}
		std::cout << __func__ << ":" << __LINE__ << "Subscribed topics with MQTT broker" << std::endl;
	}
	catch(exception &ex)
	{
		CLogger::getInstance().log(FATAL, LOGDETAILS(ex.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << ex.what() << std::endl;
		return false;
	}

	CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscribed topics with MQTT broker"));

	return true;
}

bool CMQTTHandler::connectSubscriber() {

	bool bFlag = true;
	try {
		std::lock_guard<std::mutex> lock(g_mqttSubMutexLock);

		subscriber.connect(conopts, nullptr, listener);

		// Wait for 2 seconds to get connected
		/*if (false == conntok->wait_for(2000))
		 {
		 CLogger::getInstance().log(DEBUG, LOGDETAILS("Error::Failed to connect to the platform bus ";
		 bFlag = false;
		 }*/
		std::cout << __func__ << ":" << __LINE__ << " Subscriber connected successfully with MQTT broker" << std::endl;
	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
		bFlag = false;
	}

	CLogger::getInstance().log(DEBUG, LOGDETAILS("Subscriber connected successfully with MQTT broker"));

	return bFlag;
}

bool CMQTTHandler::getSubMsgFromQ(mqtt::const_message_ptr &msg) {

	bool bRet = true;
	try {
		std::lock_guard<std::mutex> lock(m_mutexSubMsgQ);
		/// Ensure that only on thread can execute at a time
		if (false == m_qSubMsgData.empty()) {
			msg = m_qSubMsgData.front();
			m_qSubMsgData.pop();
		} else {
			bRet = false;
		}
	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
		bRet = false;
	}
	return bRet;
}

//this should push message in queue
bool CMQTTHandler::pushSubMsgInQ(mqtt::const_message_ptr msg) {

	bool bRet = true;
	try {
		/// Ensure that only on thread can execute at a time
		std::lock_guard<std::mutex> lock(m_mutexSubMsgQ);
		m_qSubMsgData.push(msg);
		CLogger::getInstance().log(DEBUG, LOGDETAILS("Pushed MQTT message in queue"));

		// Signal response process thread
		sem_post(&g_semaphoreRespProcess);

	} catch (const std::exception &e) {
		CLogger::getInstance().log(FATAL, LOGDETAILS(e.what()));
		std::cout << __func__ << ":" << __LINE__ << " Exception : " << e.what() << std::endl;
		bRet = false;
	}
	return bRet;
}

void CMQTTHandler::cleanup()
{
	sem_destroy(&g_semaphoreRespProcess);
	CLogger::getInstance().log(DEBUG, LOGDETAILS("Destroying CMQTTHandler instance ..."));

	conopts.set_automatic_reconnect(0);

	subscriber.disable_callbacks();

	if(publisher.is_connected())
		publisher.disconnect();

	if(subscriber.is_connected())
		subscriber.disconnect();

#ifdef QUEUE_FAILED_PUBLISH_MESSAGES
	m_qMsgData = {};
#endif

	m_qSubMsgData = {};

	CLogger::getInstance().log(DEBUG, LOGDETAILS("Destroyed CMQTTHandler instance"));
}

CMQTTHandler::~CMQTTHandler()
{
}

/////////////////////////////////////////////
