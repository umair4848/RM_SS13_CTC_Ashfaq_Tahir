#!/usr/bin/env python

PACKAGE = 'hbrs_marker_detection'
NODE = 'marker_detector'
SERVICE = 'detect_markers'

import roslib
roslib.load_manifest(PACKAGE)
import rospy

from collections import defaultdict
import numpy as np
from tf.transformations import euler_from_quaternion, quaternion_from_euler

from geometry_msgs.msg import Pose, Quaternion
from ar_pose.msg import ARMarkers
from hbrs_msgs.msg import Object
from hbrs_srvs.srv import GetObjects, GetObjectsResponse


def msg_to_quaternion(msg):
    return [msg.x, msg.y, msg.z, msg.w]


def quaternion_to_msg(q):
    msg = Quaternion()
    msg.x = q[0]
    msg.y = q[1]
    msg.z = q[2]
    msg.w = q[3]
    return msg


def average_pose(poses):
    pa = []
    for p in poses:
        a = euler_from_quaternion(msg_to_quaternion(p.orientation))
        pa.append([p.position.x, p.position.y, p.position.z, a[0], a[1], a[2]])
    m = np.median(np.array(pa), axis=0)
    q = quaternion_from_euler(m[3], m[4], m[5])
    pose = Pose()
    pose.orientation = quaternion_to_msg(q)
    pose.position.x = m[0]
    pose.position.y = m[1]
    pose.position.z = m[2]
    return pose


def detect_markers_cb(request):
    rospy.loginfo('Received [%s] request.' % SERVICE)
    markers = defaultdict(list)
    frame_id = '/openni_rgb_optical_frame'

    def markers_cb(msg):
        for m in msg.markers:
            markers[m.id].append(m.pose.pose)

    marker_sub = rospy.Subscriber('ar_pose_markers', ARMarkers, markers_cb)
    accumulate_time = rospy.get_param('~accumulate_time', 2)
    rospy.sleep(accumulate_time)
    marker_sub.unregister()

    response = GetObjectsResponse()
    response.stamp = rospy.Time.now()
    for i, l in markers.iteritems():
        obj = Object()
        obj.name = str(i)
        obj.pose.header.frame_id = frame_id
        obj.pose.pose = average_pose(l)
        response.objects.append(obj)
    return response

if __name__ == '__main__':
    rospy.init_node(NODE)
    detect_markers_srv = rospy.Service(SERVICE, GetObjects, detect_markers_cb)
    rospy.loginfo('Started [%s] service.' % SERVICE)
    rospy.spin()
