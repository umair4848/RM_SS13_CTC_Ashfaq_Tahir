#include "my_functional_class.h"
#include <gtest/gtest.h>


TEST(TestSuite, testCase1) {
	MyFunctionalClass f;

	EXPECT_DOUBLE_EQ(1.0, f.computeMyData(1.0));
}


int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
