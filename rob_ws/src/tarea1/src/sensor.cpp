#include <tarea1/sensor.hpp>
#include <cmath>

using std::placeholders::_1;

//Constructor
MM::MM(): Node("posicion")
{
    pub_twist = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel",10);
    sub_sensor = this-> create_subscription<geometry_msgs::msg::Vector3>(
        "/sensors_vector", 10, std::bind(&MM::sensor_callback, this, _1));


    cmd.linear.x =0;
    cmd.linear.y =0;
    cmd.linear.z =0;
    cmd.angular.x = 0;
    cmd.angular.y = 0;
    cmd.angular.z = 0;

    obstaculo_cerca = false;

}

//Destructor
MM::~MM()
{

}

void MM::sensor_callback(const geometry_msgs::msg::Vector3::SharedPtr msg)
{
    double d1 = msg->x; //FRONTAL
    double d2 = msg->y; //DERECHO
    double d3 = msg->z; // IZQUIERDO

    if(d1<7 ||d2<7||d3<7){
        cmd.linear.x = 0;
        cmd.angular.z=0;
        obstaculo_cerca = true;
    }else if (obstaculo_cerca == true){
        cmd.linear.x = 0;
        cmd.angular.z=0;
    }else{
        if(d1<30 && d2 <d3){
            cmd.linear.x = 0;
            cmd.angular.z = 1;
        }else if (d1<30 && d3<d2){
            cmd.linear.x = 0;
            cmd.angular.z = -1;
        }else{
            cmd.linear.x = 0.1;
            cmd.angular.z=0;
        }
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