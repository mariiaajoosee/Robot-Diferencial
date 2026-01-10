#include <tarea1/follow.hpp>
#include <cmath>

using std::placeholders::_1;


//Constructor
MM::MM(): Node("posicion")
{
    pub_twist = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel2",10);
    sub_pose = this-> create_subscription<geometry_msgs::msg::Pose>(
        "/robot_pose", 10, std::bind(&MM::pose_callback, this, _1));


    cmd.linear.x =0;
    cmd.linear.y =0;
    cmd.linear.z =0;
    cmd.angular.x = 0;
    cmd.angular.y = 0;
    cmd.angular.z = 0;

    L = 0.23;
    R=0.0325;

}

//Destructor
MM::~MM()
{

}

void MM::pose_callback(const geometry_msgs::msg::Pose::SharedPtr msg)
{
    double pos_x = msg->position.x;
    double pos_y = msg->position.y;
    double orientacion = msg->orientation.z;

    if (i >= posiciones.size()) {
        cmd.linear.x = 0;
        cmd.linear.y = 0;
        pub_twist->publish(cmd);
        return;
    }

    float dx = posiciones[i][0] - pos_x;
    float dy = posiciones[i][1] - pos_y;
    float theta = std::atan2(dy, dx);

    distancia = std::sqrt(dx*dx + dy*dy);
    dif = theta - orientacion;

    RCLCPP_INFO(this->get_logger(), "dx = %.3f, dy = %.3f", dx, dy);
    RCLCPP_INFO(this->get_logger(), "Pose position.x = %.3f", pos_x);
    RCLCPP_INFO(this->get_logger(), "Pose position.y = %.3f", pos_y);

    if (std::abs(distancia) < 0.2) {
        i++;
    } else {
        cmd.linear.x = 0.1 + (L/2)*(0.4*dif);
        cmd.linear.y = 0.1 - (L/2)*(0.4*dif);
    }

    pub_twist->publish(cmd);
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