// lissajous_generator: publishes a target pose moving along a Lissajous curve
//   x(t) = A * sin(a * t + delta)
//   y(t) = B * sin(b * t)
// The orientation of the pose points in the direction of motion (tangent).

#include <chrono>
#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

class LissajousGenerator : public rclcpp::Node
{
public:
  LissajousGenerator()
  : Node("lissajous_generator")
  {
    amp_x_ = declare_parameter<double>("amplitude_x", 4.0);
    amp_y_ = declare_parameter<double>("amplitude_y", 3.0);
    freq_a_ = declare_parameter<double>("freq_a", 3.0);
    freq_b_ = declare_parameter<double>("freq_b", 2.0);
    phase_ = declare_parameter<double>("phase", M_PI / 2.0);
    speed_ = declare_parameter<double>("speed", 0.25);
    frame_id_ = declare_parameter<std::string>("frame_id", "map");
    const double rate_hz = declare_parameter<double>("rate_hz", 20.0);

    pose_pub_ = create_publisher<geometry_msgs::msg::PoseStamped>("target_pose", 10);
    start_time_ = now();
    timer_ = create_wall_timer(
      std::chrono::milliseconds(static_cast<int>(1000.0 / rate_hz)),
      std::bind(&LissajousGenerator::timerCallback, this));

    RCLCPP_INFO(
      get_logger(), "Lissajous generator started: A=%.1f B=%.1f a=%.1f b=%.1f, %.0f Hz",
      amp_x_, amp_y_, freq_a_, freq_b_, rate_hz);
  }

private:
  void timerCallback()
  {
    const double t = speed_ * (now() - start_time_).seconds();

    // position on the curve
    const double x = amp_x_ * std::sin(freq_a_ * t + phase_);
    const double y = amp_y_ * std::sin(freq_b_ * t);

    // derivative -> heading (yaw) of the moving point
    const double dx = amp_x_ * freq_a_ * std::cos(freq_a_ * t + phase_);
    const double dy = amp_y_ * freq_b_ * std::cos(freq_b_ * t);
    const double yaw = std::atan2(dy, dx);

    geometry_msgs::msg::PoseStamped msg;
    msg.header.stamp = now();
    msg.header.frame_id = frame_id_;
    msg.pose.position.x = x;
    msg.pose.position.y = y;
    msg.pose.position.z = 0.0;
    // quaternion from yaw (rotation around the z axis)
    msg.pose.orientation.z = std::sin(yaw / 2.0);
    msg.pose.orientation.w = std::cos(yaw / 2.0);
    pose_pub_->publish(msg);
  }

  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Time start_time_;
  double amp_x_, amp_y_, freq_a_, freq_b_, phase_, speed_;
  std::string frame_id_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LissajousGenerator>());
  rclcpp::shutdown();
  return 0;
}
