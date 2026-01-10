#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/pose.hpp"

class MM : public rclcpp::Node
{
    public:
    MM(); //Constructor
    ~MM(); //Destructor

    //Métodos
    void manejador ( );

    private:
    //Publishers
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_twist;

    //Variables
    double tiempo;
    double radio ;

    //Current pose estimation
    geometry_msgs::msg::Twist cmd;


};