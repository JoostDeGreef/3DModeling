#include "gtest.h"
using namespace testing;

#include "Common.h"
using namespace Common;

#include "Geometry.h"
using namespace Geometry;

class EntityTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

CREATE_ENTITY_STRUCT(TestStruct, 
     ((int, ID, INTEGER PRIMARY KEY))
     ((bool, flag, BOOLEAN)));

TEST_F(EntityTest, Construction)
{
    TestStruct s0;
    TestStruct s1(42,true);

    EXPECT_EQ(42, s1.m_ID);
    EXPECT_TRUE(s1.m_flag);
    SUCCEED() << s1.sqlCreate;
    SUCCEED() << s1.sqlInsert;
    SUCCEED() << s1.sqlQuery;
}

