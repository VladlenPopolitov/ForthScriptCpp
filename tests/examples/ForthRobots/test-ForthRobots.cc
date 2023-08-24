
#include <gtest/gtest.h>
#include <string>
#include <cmath>
#include <iostream>
#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include  <ForthScriptCpp.h>
#include <crobots.h>

TEST(BasicFunctions,Test1OK)
{
  EXPECT_TRUE(true);
}

TEST(MotionCPP,lcos)
{
  long degrees=0;
  const auto MAXSIN=SCALE;
  for(degrees=-361;degrees<=361;++degrees){  
    if(std::abs(lcos(degrees) -  static_cast<long>(std::round(std::cos(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)))>1){
        std::cout <<" degrees" << degrees << " lcos "<<lcos(degrees) << "cos "<< static_cast<long>(round(std::cos(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)) << std::endl;
    }
    EXPECT_FALSE(std::abs(lcos(degrees) -  static_cast<long>(std::round(std::cos(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)))>1);
  }
  
}

TEST(MotionCPP,lsin)
{
  long degrees=0;
  const long MAXSIN=100000;
  for(degrees=-361;degrees<=361;++degrees){  
    if(std::abs(lsin(degrees) -  static_cast<long>(std::round(std::sin(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)))>1){
        std::cout <<" degrees" << degrees << " lsin "<<lsin(degrees) << "sin "<< static_cast<long>(round(std::sin(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)) << std::endl;
    }
    EXPECT_FALSE(std::abs(lsin(degrees) -  static_cast<long>(std::round(std::sin(static_cast<double>(degrees)/(RAD_DEG))*MAXSIN)))>1);
  }
  
}

TEST(MotionCPP,a){


}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

