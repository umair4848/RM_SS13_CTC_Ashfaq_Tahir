rostopic pub -r 10 /hbrs_manipulation/arm_cart_control/cartesian_velocity_command geometry_msgs/TwistStamped '{header: {frame_id: /base_link}, twist: {linear: {z: -0.05}}}'
