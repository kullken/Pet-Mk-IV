#ifndef PET_LOCALISATION_KALMAN_NODE_H
#define PET_LOCALISATION_KALMAN_NODE_H

#include <memory>
#include <string>
#include <vector>
#include <queue>

#include <ros/ros.h>

#include <tf2_ros/transform_broadcaster.h>

#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <pet_mk_iv_msgs/DistanceMeasurement.h>
#include <sensor_msgs/Imu.h>

#include "kalman_filter.h"
#include "measurement.h"
#include "imu_measurement.h"
#include "sonar_measurement.h"

namespace pet
{

class KalmanNode
{
private:
    using MeasurementPtr = std::shared_ptr<const Measurement>;

    // Lower/earlier time stamp has higher priority.
    struct MeasurementPriority
    {
        bool operator()(const MeasurementPtr& lhs, const MeasurementPtr& rhs) const {
            return lhs->stamp() > rhs->stamp();
        }
    };

public:
    KalmanNode(ros::NodeHandle& nh, ros::NodeHandle& nh_private);

    void start();

private:
    void initialise_kalman_filter();
    ros::Duration get_queue_latency(const ros::Time& now) const;

    void timer_cb(const ros::TimerEvent& e);
    void process_imu_measurement(const ImuMeasurement& measurement);
    void process_sonar_measurement(const SonarMeasurement& measurement);

    void imu_cb(const sensor_msgs::Imu& msg);
    void sonar_cb(const pet_mk_iv_msgs::DistanceMeasurement& msg);

    void publish_tf(const ros::Time& stamp);
    void publish_pose(const ros::Time& stamp);
    void publish_velocity(const ros::Time& stamp);

private:
    ros::NodeHandle& m_nh;
    ros::NodeHandle& m_nh_private;

    ros::Subscriber m_imu_sub;
    ros::Subscriber m_sonar_sub;

    ros::Publisher m_pose_pub;
    ros::Publisher m_velocity_pub;

    tf2_ros::TransformBroadcaster m_tf_broadcaster;

    geometry_msgs::TransformStamped m_tf_msg;
    geometry_msgs::PoseStamped m_pose_msg;
    geometry_msgs::Vector3Stamped m_vel_msg;

    const std::string m_base_frame;
    const std::string m_map_frame;

    ros::Timer m_timer;

    KalmanFilter m_kalman_filter;

    std::priority_queue<MeasurementPtr, std::vector<MeasurementPtr>, MeasurementPriority> m_queue;

    ros::Time m_previous_imu_time;
    ros::Time m_previous_sonar_time;
    double m_previous_sonar_distance;

private:
    // Minimum amount of time a measurement will wait in the queue before processing.
    static const ros::Duration kQueueMinLatency;
    // Maximum amount of time a measurement should wait in the queue before processing.
    static const ros::Duration kQueueMaxLatency;

    // Desired maximum duration between two consecutive imu neasurements.
    static const ros::Duration kImuMaxDuration;
    // Maximum duration between two consecutive sonar measurements for which we still use the measurement.
    static const ros::Duration kSonarMaxDuration;
};

} // namespace pet

#endif // PET_LOCALISATION_KALMAN_NODE_H