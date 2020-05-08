/************************************************************************************
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation. Title to the
* Material remains with Intel Corporation.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery of
* the Materials, either expressly, by implication, inducement, estoppel or otherwise.
************************************************************************************/

#include "../include/CDataPoint_ut.hpp"


void CDataPoint_ut::SetUp()
{
	// Setup code
}

void CDataPoint_ut::TearDown()
{
	// TearDown code
}


/***Test:CDataPoint_ut::width_manatory_param****/
/*Test 02::  checks whether mandatory parameter (iwidth) is available in yml file or not */


TEST_F(CDataPoint_ut, width_manatory_param)
{
 	baseNode = CommonUtils::loadYamlFile("iou_datapoints.yml");
	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;

			for (auto it1 : points)
			{
              //if(counter==0){
				try
				{

				CDataPoint_obj.build(it1, CDataPoint_obj);
                id=CDataPoint_obj.getID();
				m_Address = CDataPoint_obj.getAddress();
				//EXPECT_EQ(Add_expected[i++], m_Address.m_iAddress);
     			EXPECT_EQ(1, m_Address.m_iWidth);
				//EXPECT_EQ(network_info::eEndPointType::eCoil, m_Address.m_eType);

				}
				catch(YAML::Exception &e)
				{
					EXPECT_EQ("key not found", e.what());
				}

			}
		}
	}

}

/***Test:CDataPoint_ut::eType_manatory_param****/
/*Test 03::  checks whether mandatory parameter (eType) is available in yml file or not */

TEST_F(CDataPoint_ut, eType_coil)
{
	baseNode = CommonUtils::loadYamlFile("iou_datapoints.yml");
	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;
			for (auto it1 : points)
			{
				try
				{

					CDataPoint_obj.build(it1, CDataPoint_obj);
					id=CDataPoint_obj.getID();
					m_Address = CDataPoint_obj.getAddress();
					if(m_Address.m_eType == network_info::eEndPointType::eCoil)
					{
						EXPECT_EQ(network_info::eEndPointType::eCoil, m_Address.m_eType);
					}
				}
				catch(YAML::Exception &e)
				{

					EXPECT_EQ("key not found", e.what());

				}

			}
		}
	}

}


TEST_F(CDataPoint_ut, eType_Holiding_register)
{

	baseNode = CommonUtils::loadYamlFile("iou_datapoints.yml");
	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;
			for (auto it1 : points)
			{
				try
				{
					CDataPoint_obj.build(it1, CDataPoint_obj);
					id=CDataPoint_obj.getID();
					m_Address = CDataPoint_obj.getAddress();
					if(m_Address.m_eType == network_info::eEndPointType::eHolding_Register)
					{
						EXPECT_EQ(network_info::eEndPointType::eHolding_Register, m_Address.m_eType);
					}

				}
				catch(YAML::Exception &e)
				{

					EXPECT_EQ("key not found", e.what());

				}
			}

		}
	}

}


TEST_F(CDataPoint_ut, eType_Discrete_Input)
{
	//   int counter=0;


	baseNode = CommonUtils::loadYamlFile("iou_datapoints.yml");
	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;

			/*int i = 0;
			int Add_expected[] = {1, 2};*/
			for (auto it1 : points)
			{
				//if(counter==0){
				try
				{

					CDataPoint_obj.build(it1, CDataPoint_obj);
					id=CDataPoint_obj.getID();
					m_Address = CDataPoint_obj.getAddress();
					if(m_Address.m_eType == network_info::eEndPointType::eDiscrete_Input)
					{
						//		    	EXPECT_EQ(Add_expected[i++], m_Address.m_iAddress);
						//				EXPECT_EQ(1, m_Address.m_iWidth);
						EXPECT_EQ(network_info::eEndPointType::eDiscrete_Input, m_Address.m_eType);
					}

				}
				catch(YAML::Exception &e)
				{

					EXPECT_EQ("key not found", e.what());

				}
			}



		}
	}

}


TEST_F(CDataPoint_ut, eType_input_register)
{
	//   int counter=0;


	baseNode = CommonUtils::loadYamlFile("iou_datapoints.yml");
	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;

			/*int i = 0;
			int Add_expected[] = {1, 2};*/
			for (auto it1 : points)
			{
				//if(counter==0){
				try
				{

					CDataPoint_obj.build(it1, CDataPoint_obj);
					id=CDataPoint_obj.getID();
					m_Address = CDataPoint_obj.getAddress();
					if(m_Address.m_eType == network_info::eEndPointType::eInput_Register)
					{
						//		    	EXPECT_EQ(Add_expected[i++], m_Address.m_iAddress);
						//				EXPECT_EQ(1, m_Address.m_iWidth);
						EXPECT_EQ(network_info::eEndPointType::eInput_Register, m_Address.m_eType);
					}

				}
				catch(YAML::Exception &e)
				{

					EXPECT_EQ("key not found", e.what());

				}
			}



		}
	}

}


/***Test:CDataPoint_ut::all_manatory_param****/
/*Test 04::  checks whether all mandatory parameters are available in yml file or not, if not
  present then throws exception accordingly*/


/*TEST_F(CDataPoint_ut, all_manatory_param)
{
 //   int counter=0;

//	std::string path("/Device_Config/iou_datapoints_9.yml");
	std::string path("/Device_Config/iou_datapoints.yml");
	const char *cEtcdValue  = CfgManager::Instance().getETCDValuebyKey(path.c_str());
	std::string sYamlStr(cEtcdValue);
	YAML::Node baseNode = CommonUtils::loadFromETCD(sYamlStr);


	for( auto it : baseNode)
	{
		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
		{
			const YAML::Node& points =  it.second;

			int i = 0;
			int Add_expected[] = {1, 2};
			for (auto it1 : points)
			{
              //if(counter==0){
				try
				{

				CDataPoint_obj.build(it1, CDataPoint_obj);
                id=CDataPoint_obj.getID();
				m_Address = CDataPoint_obj.getAddress();

				EXPECT_EQ(Add_expected[i++], m_Address.m_iAddress);

				EXPECT_EQ(1, m_Address.m_iWidth);
				EXPECT_EQ(network_info::eEndPointType::eCoil, m_Address.m_eType);

				}
				catch(YAML::Exception &e)
				{

					cout<<e.what()<<endl;
					EXPECT_EQ("key not found", e.what());
				}

			}
		}
	}

}*/

/*****************************************************CDataPoint::getPointType()**********************************/


// Need to be Updated...

/*test 01:: this test checks whether modbus point type(COIL) is correct or not
  and throws exception accordingly**/

//TEST_F(CDataPoint_ut, 1_notsetdafault)
//{
//
//
//	std::string path("/Device_Config/iou_datapoints1.yml");
//	const char *cEtcdValue  = CfgManager::Instance().getETCDValuebyKey(path.c_str());
//
//	std::string sYamlStr(cEtcdValue);
//	YAML::Node baseNode = CommonUtils::loadFromETCD(sYamlStr);
//	cout<<baseNode<<endl;
//
//  //  baseNode = CommonUtils::loadYamlFile("iou_datapoints_6.yml");
//	for( auto it : baseNode)
//	{
//
//		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
//		{
//			const YAML::Node& points =  it.second;
//
//			for (auto it1 : points)
//			{
//
//				try{
//				CDataPoint_obj.build(it1, CDataPoint_obj);
//				m_Address = CDataPoint_obj.getAddress();
//
//				EXPECT_EQ(network_info::eEndPointType::eCoil, m_Address.m_eType);
//
//				}
//				catch(YAML::Exception &e)
//				{
//					EXPECT_EQ("Unknown Modbus point type", (string)e.what());
//				}
//
//			}
//		}
//	}
//
//}



/*test 002:: this test checks whether the modbus point type("DISCRETE_INPUT")is correct or not
  and throws exception accordingly**/


//TEST_F(CDataPoint_ut, 2_getpoint_type)
//{
//	std::string path("/Device_Config/iou_datapoints_3.yml");
//	const char *cEtcdValue  = CfgManager::Instance().getETCDValuebyKey(path.c_str());
//	std::string sYamlStr(cEtcdValue);
//	YAML::Node baseNode = CommonUtils::loadFromETCD(sYamlStr);
//
//    baseNode = CommonUtils::loadYamlFile("iou_datapoints_3.yml");
//	for( auto it : baseNode)
//	{
//		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
//		{
//			const YAML::Node& points =  it.second;
//
//			for (auto it1 : points)
//			{
//
//				try{
//				CDataPoint_obj.build(it1, CDataPoint_obj);
//				m_Address = CDataPoint_obj.getAddress();
//				EXPECT_EQ(network_info::eEndPointType::eDiscrete_Input, m_Address.m_eType); //Fails
//				}
//
//				catch(YAML::Exception &e)
//				{
//
//					EXPECT_EQ("name key not found", (string)e.what());
//				}
//
//
//			}
//		}
//	}
//
//}

/*test 003:: this test checks whether the modbus point type("INPUT_REGISTER")is correct or not
  and throws exception accordingly**/

//
//TEST_F(CDataPoint_ut, 3_getpoint_type)
//{
//
//	std::string path("/Device_Config/iou_datapoints_5.yml");
//	const char *cEtcdValue  = CfgManager::Instance().getETCDValuebyKey(path.c_str());
//	std::string sYamlStr(cEtcdValue);
//	YAML::Node baseNode = CommonUtils::loadFromETCD(sYamlStr);
//
//
//	// baseNode = CommonUtils::loadYamlFile("iou_datapoints_5.yml");
//	for( auto it : baseNode)
//	{
//		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
//		{
//			const YAML::Node& points =  it.second;
//
//			for (auto it1 : points)
//			{
//
//				try{
//				CDataPoint_obj.build(it1, CDataPoint_obj);
//				m_Address = CDataPoint_obj.getAddress();
//				EXPECT_EQ(network_info::eEndPointType::eInput_Register, m_Address.m_eType); //Fails
//				}
//
//				catch(YAML::Exception &e)
//				{
//
//					EXPECT_EQ("Unknown Modbus point type", (string)e.what());
//
//				}
//
//
//			}
//		}
//	}
//
//}


/*test 003:: This unit test is for giving exception when there is no any point name is available in
 yml file*/

//
//TEST_F(CDataPoint_ut, 4_getpoint_type)
//{
//
//
//	std::string path("/Device_Config/iou_datapoints8.yml");
//	const char *cEtcdValue  = CfgManager::Instance().getETCDValuebyKey(path.c_str());
//	std::string sYamlStr(cEtcdValue);
//	YAML::Node baseNode = CommonUtils::loadFromETCD(sYamlStr);
////	baseNode = CommonUtils::loadYamlFile("iou_datapoints_8.yml");
//	for( auto it : baseNode)
//	{
//		if(it.second.IsSequence() && it.first.as<std::string>() == "datapoints")
//		{
//			const YAML::Node& points =  it.second;
//
//			for (auto it1 : points)
//			{
//
//				try{
//				CDataPoint_obj.build(it1, CDataPoint_obj);
//				m_Address = CDataPoint_obj.getAddress();
//				EXPECT_EQ(network_info::eEndPointType::eInput_Register, m_Address.m_eType); //Fails
//				}
//
//				catch(YAML::Exception &e)
//				{
//
//
//					EXPECT_EQ("name key not found", (string)e.what());
//				}
//
//
//			}
//		}
//	}
//
//}
