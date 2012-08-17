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




do_testing(){
	# sleep to finish running tests
	sleep 10

	# export parameters
	export ROBOT_ENV="$1"
	export ROBOT="$2"

	echo ""
	echo "start testing for $ROBOT in $ROBOT_ENV..."
	rm -rf ~/.ros/test_results # delete old rostest logs
	while read myline
	do
		rostest $myline
	done < $WORKSPACE/all.tests
	rosrun rosunit clean_junit_xml.py # beautify xml files
	mkdir -p $WORKSPACE/test_results
	for i in ~/.ros/test_results/_hudson/*.xml ; do mv "$i" "$WORKSPACE/test_results/$ROBOT-$ROBOT_ENV-`basename $i`" ; done # copy test results and rename with ROBOT

	# sleep to finish running tests
	sleep 10

	echo "...finished testing for $ROBOT in $ROBOT_ENV."
}

# installing ROS release
sudo apt-get autoclean
sudo apt-get update
sudo apt-get install python-setuptools -y
sudo easy_install -U rosinstall vcstools

# setup ROS environment
. /opt/ros/$RELEASE/setup.bash

rosinstall $WORKSPACE/../ext_pkgs $WORKSPACE/repository.rosinstall --delete-changed-uris --rosdep-yes

# define amount of ros prozesses during build for multi-prozessor machines
COUNT=$(cat /proc/cpuinfo | grep 'processor' | wc -l)
COUNT=$(echo "$COUNT*2" | bc)
export ROS_PARALLEL_JOBS=-j$COUNT


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
rosmake -r * --skip-blacklist --profile

# check if building is succesfull, otherwise don't perform test and exit
if [ $? != "0" ]; then
	echo "rosmake failed, skipping tests"
	exit 1
fi

# do rostests
echo ""
echo "--------------------------------------------------------------------------------"
echo "Rostest for $REPOSITORY"

mkdir -p $WORKSPACE/test_results # create test_results directory
rm -rf ~/.ros/test_results # delete old rostest logs

if [ ! -s $WORKSPACE/all.tests ]; then
	echo "all.tests-file not found or empty, creating dummy test result file"
	# create dummy test result file
	touch $WORKSPACE/test_results/dummy_test.xml
	echo '<testsuite errors="0" failures="0" name="dummy_test" tests="1" time="0.01">
	<testcase classname="DummyTest.DummyTest" name="dummy_test" time="0.01">
	</testcase>
	<system-out><![CDATA[]]></system-out>
	<system-err><![CDATA[]]></system-err>
</testsuite>' >> $WORKSPACE/test_results/dummy_test.xml
else
	do_testing ipa-kitchen cob3-1
	do_testing ipa-kitchen cob3-2
	do_testing ipa-kitchen cob3-3
fi
echo "--------------------------------------------------------------------------------"
echo ""
