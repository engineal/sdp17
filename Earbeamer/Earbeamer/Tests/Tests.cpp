// Tests.cpp : Defines the entry point for the console application.
//

#include "gtest/gtest.h"
#include "../arithmetic.h"

TEST(SquareTests, Square)
{
	EXPECT_EQ(0, square(0));
	EXPECT_EQ(-4, square(-2));
}
