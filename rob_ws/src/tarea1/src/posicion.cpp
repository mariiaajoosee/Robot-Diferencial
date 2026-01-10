#include <tarea1/posicion.hpp>
#include <cmath>

using std::placeholders::_1;

//Constructor
MM::MM(): Node("posicion")
{
    pub_twist = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
    sub_pose = this-> create_subscription<geometry_msgs::msg::Pose>(
        "/robot_pose", 10, std::bind(&MM::pose_callback, this, _1));


    cmd.linear.x =0;
    cmd.linear.y =0;
    cmd.linear.z =0;
    cmd.angular.x = 0;
    cmd.angular.y = 0;
    cmd.angular.z = 0;

}

//Destructor
MM::~MM()
{

}

void MM::pose_callback(const geometry_msgs::msg::Pose::SharedPtr msg)
{
    double pos_x = msg->position.x;

    RCLCPP_INFO(this->get_logger(), "Pose position.x = %.3f", pos_x);

    if(abs(pos_x - 2) < 0.25){
        cmd.linear.x = 0;
    }else{
        //cmd.linear.x = 0.2;
        cmd.linear.x = 0.25;
    }

    pub_twist -> publish(cmd);

}

int main (int argc, char * argv[])
{
    //init ROS2 node
    rclcpp::init( argc, argv);

    // Create object from our MotionModel class
    auto node = std::make_shared<MM>();


    // Running at 5Hz
    rclcpp::Rate loop_rate(5.0);
    RCLCPP_INFO(node->get_logger(), "Starting Main Loop...");
    
    while (rclcpp::ok())
    {
        rclcpp::spin_some(node);  // attend subscriptions and srv request
        loop_rate.sleep();  // sleep till next step time
    }

    rclcpp::shutdown();
    return 0;
}