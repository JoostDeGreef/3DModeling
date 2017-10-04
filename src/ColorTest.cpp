#include "CommonTestFunctionality.h"

class ColorTest : public Test 
{
protected:
	virtual void SetUp() 
    {
    }

    virtual void TearDown() 
    {
    }
};

TEST_F(ColorTest, FromToInt) 
{
    Color c0(0.0f, 0.5f, 1.0f, 1.0f);
    unsigned int i0 = c0.GetInt();
    Color c1(i0);
    unsigned int i1 = c1.GetInt();

//    EXPECT_EQ(c0, c1);
    EXPECT_EQ(i0, i1);
}

