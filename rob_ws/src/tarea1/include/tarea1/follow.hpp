#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/pose.hpp"

#include <vector>

struct Vector2 {
    float x;
    float y;
};

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
    std::vector<std::array<float,2>> posiciones = {
        {1.0, 1.0},
        {1.5, 1.0},
        {1.5, 2.0},
    };

    int i = 0;   // índice del waypoint actual

    float distancia = 0;
    float dif = 0;

    //Current pose estimation
    geometry_msgs::msg::Twist cmd;
    geometry_msgs::msg::Pose pose_final;

    double L;
    double R;


};