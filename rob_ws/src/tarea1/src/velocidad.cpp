#include <tarea1/velocidad.hpp>
#include <cmath>

const double PI = 3.141592;

//Constructor
MM::MM(): Node("velocidad")
{
    pub_twist = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
    

    tiempo = 0;

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


void MM::manejador(){

    if(tiempo < 20){
        cmd.linear.x = 0.1;
    }else{
        cmd.linear.x = 0;
    }

    pub_twist -> publish(cmd);

    tiempo += 0.2;

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
        node->manejador();  //sen twist to robot and update pose estimation
        loop_rate.sleep();  // sleep till next step time
    }

    rclcpp::shutdown();
    return 0;
}