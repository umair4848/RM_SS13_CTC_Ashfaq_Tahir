#!/usr/bin/env python
import roslib; roslib.load_manifest('hbrs_example_ros_package')

import sys
import rosunit

pkg_dir = roslib.packages.get_pkg_dir('hbrs_example_ros_package')
sys.path.append(pkg_dir + '/common/scripts')
import hbrs_example_ros_package.test.my_functional_class_test


if __name__ == '__main__':
	rosunit.unitrun('hbrs_example_ros_package', 'test_my_functional_class',
			hbrs_example_ros_package.test.my_functional_class_test.TestMyFunctionalClass)
