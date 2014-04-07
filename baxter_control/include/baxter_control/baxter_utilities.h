/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, CU Boulder
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of CU Boulder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/**
 * \brief   Helper functions for controlling baxter
 * \author  Dave Coleman
 */

#ifndef BAXTER_CONTROL__BAXTER_UTILITIES_
#define BAXTER_CONTROL__BAXTER_UTILITIES_

// ROS
#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>

// Boost
#include <boost/scoped_ptr.hpp>

// MoveIt!
#include <moveit/move_group_interface/move_group.h>
#include <moveit/kinematic_constraints/utils.h>

// Msgs
#include <std_msgs/Bool.h>
#include <std_msgs/Empty.h>
#include <baxter_core_msgs/AssemblyState.h>
#include <baxter_core_msgs/DigitalIOState.h>
#include <moveit_msgs/MoveGroupGoal.h>
#include <moveit_msgs/MoveGroupAction.h>

// Visualization
#include <moveit_visual_tools/visual_tools.h>

namespace baxter_control
{

static const std::string BAXTER_STATE_TOPIC = "/robot/state";

// Needed?
static const std::string ROBOT_DESCRIPTION="robot_description";
static const std::string PLANNING_GROUP_BOTH_NAME = "both_arms";
static const std::string BASE_LINK = "base"; //"/base";
static const std::string NEUTRAL_POSE_NAME = "both_neutral";


class BaxterUtilities
{
public:

  // ROS Messages
  ros::Publisher pub_baxter_enable_;
  ros::Publisher pub_baxter_reset_;
  ros::Subscriber sub_baxter_state_;
  ros::Subscriber sub_shoulder_left_;
  ros::Subscriber sub_shoulder_right_;

  // Action Servers and Clients
  boost::shared_ptr<actionlib::SimpleActionClient<moveit_msgs::MoveGroupAction> > movegroup_action_;

  // Interface with MoveIt
  boost::scoped_ptr<move_group_interface::MoveGroup> move_group_both_;
  boost::scoped_ptr<move_group_interface::MoveGroup> move_group_;

  // Remember the last baxter state and time
  baxter_core_msgs::AssemblyStateConstPtr baxter_state_;
  ros::Time baxter_state_timestamp_;

  // Cache messages
  std_msgs::Bool enable_msg_;
  std_msgs::Bool disable_msg_;
  std_msgs::Empty empty_msg_;

  std::size_t state_counter_;
  bool disabled_callback_called_;

  // Optional function callback for when baxter is disabled
  typedef boost::function<void ()> DisabledCallback;
  DisabledCallback disabled_callback_; //   f1( boost::bind( &myclass::fun1, this ) );

  
  BaxterUtilities();
  
  /**
   * \brief Allow classes that uses BaxterUtilities to add a hook for when Baxter is disabled
   * \param callback - the function to call when baxter is disabled
   */
  void setDisabledCallback(DisabledCallback callback);

  /**
   * \brief Wait for initial state to be recieved from Baxter
   * \return true if communication is ok
   */
  bool communicationActive();

  /**
   * \brief Check if there is no error, is not stopped, and is enabled
   * \return true if baxter is ready to use
   */
  bool isEnabled(bool verbose = false);

  void stateCallback(const baxter_core_msgs::AssemblyStateConstPtr& msg);

  void leftShoulderCallback(const baxter_core_msgs::DigitalIOStateConstPtr& msg);
  void rightShoulderCallback(const baxter_core_msgs::DigitalIOStateConstPtr& msg);

  bool enableBaxter();

  bool disableBaxter();

  bool resetBaxter();

  bool positionBaxterReady();

  bool positionBaxterNeutral();

  /**
   * \brief Send baxter to a named pose defined in the SRDF
   * \param pose_name - name of pose in SRDF
   * \return true if sucessful in planning and moving there
   */
  bool sendToPose(const std::string &pose_name);

  /**
   * \brief Moves the arm to a specified pose
   * \param pose - desired goal
   * \param group_name - which arm / planning group to use the pose with
   * \return true if sucessful in planning and moving there
   */
  bool sendToPose(const geometry_msgs::PoseStamped& pose, const std::string &group_name,   
    moveit_visual_tools::VisualToolsPtr visual_tools);


};

} //namespace

#endif
