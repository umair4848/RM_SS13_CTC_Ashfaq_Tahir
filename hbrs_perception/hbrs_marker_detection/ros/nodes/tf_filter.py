#!/usr/bin/env python

PACKAGE = 'hbrs_marker_detection'
NODE = 'tf_filter'

import roslib
roslib.load_manifest(PACKAGE)
import rospy

import numpy as np
from tf.transformations import euler_from_quaternion, quaternion_from_euler
from tf.msg import tfMessage

from geometry_msgs.msg import TransformStamped


def msg_to_quaternion(msg):
    return [msg.x, msg.y, msg.z, msg.w]


def transform_to_list(tf):
    a = euler_from_quaternion(msg_to_quaternion(tf.rotation))
    t = tf.translation
    return [t.x, t.y, t.z, a[0], a[1], a[2]]


class TfFilter:
    def __init__(self, frame_id, buffer_size):
        self.frame_id = frame_id
        self.buffer = np.zeros((buffer_size, 6))
        self.buffer_ptr = 0
        self.buffer_size = buffer_size
        self.tf_sub = rospy.Subscriber('tf', tfMessage, self.tf_cb)
        self.tf_pub = rospy.Publisher('tf', tfMessage)
        self.buffer_filled = False

    def tf_cb(self, msg):
        for t in msg.transforms:
            if t.child_frame_id == self.frame_id:
                self.buffer[self.buffer_ptr] = transform_to_list(t.transform)
                self.buffer_ptr = (self.buffer_ptr + 1) % self.buffer_size
                if self.buffer_filled:
                    self.publish_filtered_tf(t.header)
                else:
                    self.buffer_filled = self.buffer_ptr == 0

    def publish_filtered_tf(self, header):
        m = np.median(self.buffer, axis=0)
        q = quaternion_from_euler(m[3], m[4], m[5])
        ts = TransformStamped()
        ts.header = header
        ts.child_frame_id = self.frame_id + '_filtered'
        ts.transform.translation.x = m[0]
        ts.transform.translation.y = m[1]
        ts.transform.translation.z = m[2]
        ts.transform.rotation.x = q[0]
        ts.transform.rotation.y = q[1]
        ts.transform.rotation.z = q[2]
        ts.transform.rotation.w = q[3]
        msg = tfMessage()
        msg.transforms.append(ts)
        self.tf_pub.publish(msg)


if __name__ == '__main__':
    rospy.init_node(NODE)
    target_frame_id = rospy.get_param('~target_frame_id')
    filter_window = rospy.get_param('~filter_window', 30)
    tf_filer = TfFilter(target_frame_id, filter_window)
    rospy.loginfo('Started [%s] node.' % NODE)
    rospy.spin()
