RM_SS13_CTC_Ashfaq_Tahir
========================

<pre>

"Cartesian Trajectory Controller" for KUKA youBot manipulator having five degrees of freedom.

Using this algorithm, the youBot manipulator can move its end-effector in a straight line,
with a given velocity in Cartesian space.

Details:

A new node named "CTC" has been added, with path "hbrs_manipulation/hbrs_arm_cart_control/ros/nodes/CTC.cpp".
The "CTC" node:
1. takes the "Cartesian coordinates (target poses)" and the "current tool pose" as inputs,
2. calculates the "cartesian velocity" depending on the current and the target pose and
3. delivers the calculated "cartesian velocity" as the output, which is then published to the "arm_cart_control" node.

-------------------------------------------------------------------------------------------

Group Members:			Muhammad Umair Tahir,
						Nasir Ashfaq

Instructer:				Prof. Dr. Erwin Prassler

Teaching Assistant:		Matthias Fueller

Course:					Robot Manipulation

Term:					Summer Semester 2013

Institute:				Bonn-Rhein-Sieg University of Applied Sciences
</pre>
