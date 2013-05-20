#!/usr/bin/env python
import unittest
import hbrs_example_ros_package.my_functional_class

class TestMyFunctionalClass(unittest.TestCase):

	def setUp(self):
		self.f = hbrs_example_ros_package.my_functional_class.MyFunctionalClass()

	def test_add_one_and_one(self):
		self.assertEquals(2, self.f.add_two_numbers(1, 1))


if __name__ == '__main__':
	unittest.main()
