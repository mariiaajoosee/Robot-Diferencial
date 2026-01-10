#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
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
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr sub_pose;

    void pose_callback(const geometry_msgs::msg::Pose::SharedPtr msg);

    //Variables

    //Current pose estimation
    geometry_msgs::msg::Twist cmd;
    geometry_msgs::msg::Pose pose_final;


};