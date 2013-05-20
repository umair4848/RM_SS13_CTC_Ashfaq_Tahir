#!/usr/bin/env python
import roslib; roslib.load_manifest('hbrs_example_ros_package')
import sys
import rospy
import std_msgs.msg

pkg_dir = roslib.packages.get_pkg_dir('hbrs_example_ros_package')
sys.path.append(pkg_dir + '/common/scripts')
import my_functional_class

class MyNode:

	def __init__(self):
		self.f = my_functional_class.MyFunctionalClass()
		
		'''
		rospy.loginfo('Waiting for service "dummy"')
		rospy.wait_for_service('dummy')
		rospy.loginfo('Found service "dummy"')
		'''
		
		self.publisher = rospy.Publisher('out', std_msgs.msg.Int32)
		rospy.Subscriber('in', std_msgs.msg.Int32, self.add_callback)

	def add_callback(self, msg):
		res = std_msgs.msg.Int32()
		res.data = self.f.add_two_numbers(msg.data, msg.data)
		self.publisher.publish(res)


if __name__ == '__main__':
	rospy.init_node('hbrs_ros_node_example')

	node = MyNode()

	while (not rospy.is_shutdown()):
		rospy.sleep(1.0)
