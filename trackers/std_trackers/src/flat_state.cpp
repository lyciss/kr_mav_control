/**
 * Class to hold flat outputs state information for trajectory generation.
 *
 * Written by Justin Thomas <justinthomas@jtwebs.net>
 *
 * In most cases we would want to have consecutive trajectories to smoothly
 * transition from one to another, so we need to keep the desired command
 * continuous. This class stores the last command so as to use it as an initial
 * condition for the next trajectory leading to a smooth desired command.
 * Previously, we used the current odom of the robot to set the initial
 * condition and if the robot had some tracking error, it would lead to a jump
 * in the desired, which we want to avoid.
 */

#include <tf/transform_datatypes.h>
#include <std_trackers/flat_state.h>

FlatState::FlatState() :
  pos_(Eigen::Vector3f::Zero()),
  vel_(Eigen::Vector3f::Zero()),
  acc_(Eigen::Vector3f::Zero()),
  jrk_(Eigen::Vector3f::Zero()),
  snp_(Eigen::Vector3f::Zero()),
  yaw_(0.0f),
  yaw_dot_(0.0f),
  yaw_ddot_(0.0f),
  cmd_valid_(false)
{
}

void FlatState::set_from_cmd(const quadrotor_msgs::PositionCommand::ConstPtr &msg)
{
  if (msg == NULL)
  {
    ROS_WARN("Null PositionCommand recieved. Not setting initial condition.");
    return;
  }

  pos_ = Eigen::Vector3f(msg->position.x, msg->position.y, msg->position.z);
  vel_ = Eigen::Vector3f(msg->velocity.x, msg->velocity.y, msg->velocity.z);
  acc_ = Eigen::Vector3f(msg->acceleration.x, msg->acceleration.y, msg->acceleration.z);
  jrk_ = Eigen::Vector3f(msg->jerk.x, msg->jerk.y, msg->jerk.z);
  snp_ = Eigen::Vector3f(msg->snap.x, msg->snap.y, msg->snap.z);
  yaw_ = msg->yaw;
  yaw_dot_ = msg->yaw_dot;
  yaw_ddot_ = msg->yaw_ddot;

  cmd_valid_ = true;
}

void FlatState::set_from_odom(const nav_msgs::Odometry::ConstPtr &msg)
{
  if(!cmd_valid_)
  {
    pos_ = Eigen::Vector3f(msg->pose.pose.position.x, msg->pose.pose.position.y, msg->pose.pose.position.z);
    vel_ = Eigen::Vector3f(msg->twist.twist.linear.x, msg->twist.twist.linear.y, msg->twist.twist.linear.z);
    acc_ = Eigen::Vector3f::Zero();
    jrk_ = Eigen::Vector3f::Zero();
    snp_ = Eigen::Vector3f::Zero();
    yaw_ = tf::getYaw(msg->pose.pose.orientation);
    yaw_dot_ = msg->twist.twist.angular.z; // TODO: Should double check which frame (body or world) this is in
    yaw_ddot_ = 0.0f;
  }
}

void FlatState::reset()
{
  cmd_valid_ = false;
}