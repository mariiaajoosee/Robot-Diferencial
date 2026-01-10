#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "geometry_msgs/msg/pose.hpp"

class MM : public rclcpp::Node
{
    public:
    MM(); //Constructor
    ~MM(); //Destructor

    //Métodos
    

    private:
    //Publishers
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist;
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr sub_joy;

    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg);

    //Variables
    double scale_linear_; 
    double scale_angular_;

    //Current pose estimation
    geometry_msgs::msg::Twist cmd;


};