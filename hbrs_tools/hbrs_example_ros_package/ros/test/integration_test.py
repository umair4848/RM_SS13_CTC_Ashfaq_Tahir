#!/usr/bin/env python
import roslib; roslib.load_manifest('hbrs_example_ros_package')

import sys
import unittest
import rospy
import rostest
import std_msgs.msg
import std_srvs.srv

class TestMyFunctionalNode(unittest.TestCase):

	def setUp(self):
		self.result = 0
		self.wait_for_result = True
		self.publisher = rospy.Publisher("in", std_msgs.msg.Int32, latch = True)
		self.subscriber = rospy.Subscriber("out", std_msgs.msg.Int32, self.result_callback)

	def tearDown(self):
		self.subscriber.unregister()
		self.publisher.unregister()

	def test_node_send_one(self):
		self.publisher.publish(1)
		while (self.wait_for_result): pass
		self.assertEquals(2, self.result.data)
		rospy.logerr("An error occurred")
		self.assertEquals(2, 2)

	def test_node_send_two(self):
		self.publisher.publish(2)
		while (self.wait_for_result): pass
		self.assertEquals(4, self.result.data)

	def result_callback(self, msg):
		self.result = msg
		self.wait_for_result = False


if __name__ == '__main__':
	rospy.init_node('integration_test')
	rostest.rosrun('hbrs_example_ros_package', 'integration_test', TestMyFunctionalNode)
