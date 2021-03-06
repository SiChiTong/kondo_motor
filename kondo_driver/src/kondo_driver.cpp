/**
 * @file kondo_driver.cpp
 * @author Ryosuke Tajima
 * Kondo ICS motor driver
 */
#include <boost/shared_ptr.hpp>
#include <math.h>
#include "ros/ros.h"
#include "hardware_interface/joint_command_interface.h"
#include "hardware_interface/joint_state_interface.h"
#include "hardware_interface/actuator_command_interface.h"
#include "hardware_interface/actuator_state_interface.h"
#include "hardware_interface/robot_hw.h"
#include "kondo_driver/kondo_driver.h"

KondoDriver::KondoDriver(int num, char **actuators)
  : ics(-1)
{
  ros::NodeHandle nh("~");
  // Loopback mode
  nh.param <bool>("loopback", loopback, false);
  if (loopback) {
    ROS_WARN("Initialized as loopback mode. The node does not use hardware and loopback the command to state.");
  }
  // open ICS interface device
  if (!loopback) {
    std::string device;
    nh.param <std::string>("device", device, "/dev/ttyUSB0");
    ics = ics_open(device.c_str());
    if (ics < 0) {
      ROS_ERROR("Cannot open ICS device %s\n", device.c_str());
      exit(0);
    }
  }
  // Load atuators
  for (int i = 0; i < num; i++) {
    boost::shared_ptr < KondoMotor >actuator(new KondoMotor(ics, std::string(actuators[i]), joint_state_interface, position_joint_interface,velocity_joint_interface, loopback));
    actuator_vector.push_back(actuator);
  }
  registerInterface(&joint_state_interface);
  registerInterface(&position_joint_interface);
  registerInterface(&velocity_joint_interface);
}

KondoDriver::~KondoDriver()
{
  if (!loopback) {
    ics_close(ics);
  }
}

/**
 * @brief Update function to call all of the update function of motors
 */
void KondoDriver::update(void)
{
  for (int i = 0; i < actuator_vector.size(); i++) {
    actuator_vector[i]->update();
  }
}

/**
 * @brief Switch the controller one to another
 *
 */
void KondoDriver::doSwitch(const std::list < hardware_interface::ControllerInfo > &start_list,
			   const std::list < hardware_interface::ControllerInfo > &stop_list)
{
  // need to be fixed.
#if 0
  
  std::list < hardware_interface::ControllerInfo >::const_iterator info;

  // set the KondoMotor's controller type as the started controller
  for (info = start_list.begin(); info != start_list.end(); info++) {
    for (int i = 0; i < actuator_vector.size(); i++) {
      for (std::set < std::string >::const_iterator resource_it = info->claimed_resources.begin();
	   resource_it != info->claimed_resources.end(); ++resource_it) {
	if (actuator_vector[i]->joint_name == *resource_it) {
          ROS_INFO("controller of %s changed to %s", actuator_vector[i]->joint_name.c_str(), info->type.c_str());
	  if (info->type == "position_controllers/JointPositionController") {
	    actuator_vector[i]->controller_type = 0;
	  } else if (info->type == "velocity_controllers/JointVelocityController") {
	    actuator_vector[i]->controller_type = 1;
	  } else {
	    ROS_WARN("invalid controller %s for %s", info->type.c_str(), actuator_vector[i]->joint_name.c_str());
	  }
	}
      }
    }
  }
#endif
  
}
