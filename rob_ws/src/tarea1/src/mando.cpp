#include <tarea1/mando.hpp>
#include <cmath>

using std::placeholders::_1;

//Constructor
MM::MM(): Node("posicion")
{
    pub_twist = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
    sub_joy = this-> create_subscription<sensor_msgs::msg::Joy>(
        "/joy", 10, std::bind(&MM::joy_callback, this, _1));

    
        scale_linear_ = 0.15; 
        scale_angular_ = 1;

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

void MM::joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
{
    if(msg->buttons[6]==1){
        cmd.linear.x = 0;
        cmd.angular.z = 1;
    }else if(msg->buttons[7]==1){
        cmd.linear.x = 0;
        cmd.angular.z = -1;
    }else{
        cmd.linear.x = msg->axes[1] * scale_linear_;
        cmd.angular.z = msg->axes[0] * scale_angular_;
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
    rclcpp::Rate loop_rate(100.0);
    RCLCPP_INFO(node->get_logger(), "Starting Main Loop...");
    
    while (rclcpp::ok())
    {
        rclcpp::spin_some(node);  // attend subscriptions and srv request
        loop_rate.sleep();  // sleep till next step time
    }

    rclcpp::shutdown();
    return 0;
}