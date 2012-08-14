#ifndef TEST_BASE_HPP_
#define TEST_BASE_HPP_

#include <iostream>
#include <pcl/common/time.h>

#define MEASURE_RUNTIME(function, description) \
        { \
          double ts1 = pcl::getTime(); \
          (function); \
          double ts2 = pcl::getTime(); \
          std::cout << (description) << " took " << ts2 - ts1 << " seconds." << std::endl; \
        }

class TestBase
{

public:

  virtual ~TestBase() { }

protected:

  virtual void process() = 0;

};

#endif

