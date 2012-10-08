#!/usr/bin/env python

PACKAGE = 'hbrs_marker_detection'
NODE = 'tf_goal'

import roslib
roslib.load_manifest(PACKAGE)
import rospy

import numpy as np
from tf.transformations import euler_from_quaternion, quaternion_from_euler
from tf.msg import tfMessage
from tf import TransformListener

from geometry_msgs.msg import TransformStamped


def msg_to_quaternion(msg):
    return [msg.x, msg.y, msg.z, msg.w]


def transform_to_list(tf):
    a = euler_from_quaternion(msg_to_quaternion(tf.rotation))
    t = tf.translation
    return [t.x, t.y, t.z, a[0], a[1], a[2]]


class TfFilter:
    def __init__(self, source_frame_id, target_frame_id, buffer_size):
        self.tf = TransformListener(True, rospy.Duration(5))
        self.source = source_frame_id
        self.target = target_frame_id
        self.buffer = np.zeros((buffer_size, 1))
        self.buffer_ptr = 0
        self.buffer_size = buffer_size
        self.tf_sub = rospy.Subscriber('tf', tfMessage, self.tf_cb)
        self.tf_pub = rospy.Publisher('tf', tfMessage)

    def tf_cb(self, msg):
        for t in msg.transforms:
            if t.child_frame_id == self.target:
                p, q = self.tf.lookupTransform(self.source, self.target,
                                               t.header.stamp)
                [roll, pitch, yaw] = euler_from_quaternion(q)
                print 'Relative angles:', roll, pitch, yaw
                self.last_position = p
                self.buffer[self.buffer_ptr] = yaw
                self.buffer_ptr = (self.buffer_ptr + 1) % self.buffer_size
                self.publish_filtered_tf(t.header)

    def publish_filtered_tf(self, header):
        m = np.mean(self.buffer)
        q = quaternion_from_euler(0, 0, m)
        ts = TransformStamped()
        ts.header = header
        ts.header.frame_id = self.source
        ts.child_frame_id = 'goal'
        ts.transform.translation.x = self.last_position[0]
        ts.transform.translation.y = self.last_position[1]
        ts.transform.translation.z = self.last_position[2]
        ts.transform.rotation.x = q[0]
        ts.transform.rotation.y = q[1]
        ts.transform.rotation.z = q[2]
        ts.transform.rotation.w = q[3]
        msg = tfMessage()
        msg.transforms.append(ts)
        self.tf_pub.publish(msg)


if __name__ == '__main__':
    rospy.init_node(NODE)
    source_frame_id = rospy.get_param('~source_frame_id')
    target_frame_id = rospy.get_param('~target_frame_id')
    filter_window_length = rospy.get_param('~filter_window_length', 30)
    tf_filer = TfFilter(source_frame_id, target_frame_id, filter_window_length)
    rospy.loginfo('Started [%s] node.' % NODE)
    rospy.spin()
