#!/bin/bash

# get the name of ROSRELEASE, GITHUBUSER and REPOSITORY from JOB_NAME
RELEASE=$1
GITHUBUSER=$2
REPOSITORY=$3

echo ""
echo "-------------------------------------------------------"
echo "==> RELEASE =" $RELEASE
echo "==> GITHUBUSER =" $GITHUBUSER
echo "==> REPOSITORY =" $REPOSITORY
echo "-------------------------------------------------------"
echo ""


# installing ROS release
sudo apt-get autoclean
sudo apt-get update
sudo apt-get install python-setuptools -y
sudo easy_install -U rosinstall vcstools
sudo apt-get install ros-$RELEASE-* -y
sudo apt-get remove ros-fuerte-moveit-core  ros-fuerte-moveit-msgs -y
sudo apt-get autoremove -y

# setup ROS environment
. /opt/ros/$RELEASE/setup.bash

# execute repository.rosinstall of each repository
rosinstall $WORKSPACE/../ext_pkgs $WORKSPACE/repository.rosinstall --delete-changed-uris --rosdep-yes

# define amount of ros prozesses during build for multi-prozessor machines
#COUNT=$(cat /proc/cpuinfo | grep 'processor' | wc -l)
#COUNT=$(echo "$COUNT*2" | bc)
export ROS_PARALLEL_JOBS=-j1

# add whole directory of the job the the ROS package path
export ROS_PACKAGE_PATH=$WORKSPACE/..:$ROS_PACKAGE_PATH

echo ""
echo "-------------------------------------------------------"
echo "==> RELEASE =" $RELEASE
echo "==> WORKSPACE =" $WORKSPACE
echo "==> ROS_ROOT =" $ROS_ROOT
echo "==> ROS_PACKAGE_PATH =" $ROS_PACKAGE_PATH
echo "-------------------------------------------------------"
echo ""

# installing dependencies and building
export USE_NORMAL_SUDO=1    # for youbot_oodl
cd $WORKSPACE
rosdep install * -y
rosmake -r * --skip-blacklist --profile --status-rate=1

# check if building is succesfull, otherwise don't perform test and exit
if [ $? != "0" ]; then
	echo "rosmake failed, skipping tests"
	exit 1
fi

# do rostests
echo ""
echo "--------------------------------------------------------------------------------"
echo "Rostest for $REPOSITORY"

mkdir -p $WORKSPACE/../test_results # create test_results directory

echo "--------------------------------------------------------------------------------"
echo ""
