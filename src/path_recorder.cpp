// path_recorder: subscribes to the target pose, collects the last N poses
// into a nav_msgs/Path (visualizable in RViz2) and publishes the
// total travelled distance as std_msgs/Float32.

#include <cmath>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/path.hpp"
#include "std_msgs/msg/float32.hpp"

using std::placeholders::_1;

class PathRecorder : public rclcpp::Node
{
public:
  PathRecorder()
  : Node("path_recorder")
  {
    max_poses_ = declare_parameter<int>("max_poses", 500);

    pose_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>(
      "target_pose", 10, std::bind(&PathRecorder::poseCallback, this, _1));
    path_pub_ = create_publisher<nav_msgs::msg::Path>("trajectory", 10);
    dist_pub_ = create_publisher<std_msgs::msg::Float32>("travelled_distance", 10);

    RCLCPP_INFO(get_logger(), "Path recorder started, keeping the last %d poses", max_poses_);
  }

private:
  void poseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg)
  {
    // accumulate the distance between the previous and the current position
    if (!path_.poses.empty()) {
      const auto & last = path_.poses.back().pose.position;
      distance_ += std::hypot(msg->pose.position.x - last.x, msg->pose.position.y - last.y);
    }

    path_.header = msg->header;
    path_.poses.push_back(*msg);
    if (static_cast<int>(path_.poses.size()) > max_poses_) {
      path_.poses.erase(path_.poses.begin());
    }
    path_pub_->publish(path_);

    std_msgs::msg::Float32 dist_msg;
    dist_msg.data = static_cast<float>(distance_);
    dist_pub_->publish(dist_msg);

    RCLCPP_INFO_THROTTLE(
      get_logger(), *get_clock(), 2000,
      "pose: (%.2f, %.2f) | path size: %zu | travelled distance: %.2f m",
      msg->pose.position.x, msg->pose.position.y, path_.poses.size(), distance_);
  }

  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_sub_;
  rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr dist_pub_;
  nav_msgs::msg::Path path_;
  double distance_ = 0.0;
  int max_poses_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PathRecorder>());
  rclcpp::shutdown();
  return 0;
}
