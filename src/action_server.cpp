#include <chrono>
#include <cmath>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "task3_action_service/action/go_to_charger.hpp"

using namespace std::chrono_literals;

class ChargingActionServer : public rclcpp::Node
{
public:
    using GoToCharger =
        task3_action_service::action::GoToCharger;

    using GoalHandleGoToCharger =
        rclcpp_action::ServerGoalHandle<GoToCharger>;

    ChargingActionServer()
        : Node("charging_action_server")
    {
        using namespace std::placeholders;

        action_server_ =
            rclcpp_action::create_server<GoToCharger>(
                this,
                "go_to_charger",
                std::bind(
                    &ChargingActionServer::handle_goal,
                    this,
                    _1,
                    _2),
                std::bind(
                    &ChargingActionServer::handle_cancel,
                    this,
                    _1),
                std::bind(
                    &ChargingActionServer::handle_accepted,
                    this,
                    _1));

        RCLCPP_INFO(
            this->get_logger(),
            "Charging Action Server started.");
    }

private:

    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const GoToCharger::Goal> goal)
    {
        (void)uuid;
        (void)goal;

        RCLCPP_INFO(
            this->get_logger(),
            "Received GoToCharger action goal.");

        RCLCPP_INFO(
            this->get_logger(),
            "Accepting goal.");

        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

   

    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleGoToCharger> goal_handle)
    {
        (void)goal_handle;

        RCLCPP_INFO(
            this->get_logger(),
            "Received request to cancel goal.");

        return rclcpp_action::CancelResponse::ACCEPT;
    }


    void handle_accepted(
        const std::shared_ptr<GoalHandleGoToCharger> goal_handle)
    {

        std::thread(
            std::bind(
                &ChargingActionServer::execute,
                this,
                goal_handle))
            .detach();
    }


    void execute(
        const std::shared_ptr<GoalHandleGoToCharger> goal_handle)
    {
        RCLCPP_INFO(
            this->get_logger(),
            "Goal accepted — starting 30-second task.");

        auto feedback =
            std::make_shared<GoToCharger::Feedback>();

        auto result =
            std::make_shared<GoToCharger::Result>();

        float distance_remaining = 30.0f;

        for (int second = 1; second <= 30; ++second)
        {
            if (!rclcpp::ok())
            {
                return;
            }

            if (goal_handle->is_canceling())
            {
                result->success = false;
                result->message =
                    "Charging station task cancelled.";

                goal_handle->canceled(result);

                RCLCPP_INFO(
                    this->get_logger(),
                    "Goal cancelled.");

                return;
            }

            std::this_thread::sleep_for(1s);

            distance_remaining -= 1.0f;

            if (distance_remaining < 0.0f)
            {
                distance_remaining = 0.0f;
            }

            feedback->distance_remaining =
                distance_remaining;

            goal_handle->publish_feedback(feedback);

            RCLCPP_INFO(
                this->get_logger(),
                "[feedback] Distance remaining: %.1f m",
                distance_remaining);
        }

        result->success = true;
        result->message =
            "Arrived at charging station!";

        goal_handle->succeed(result);

        RCLCPP_INFO(
            this->get_logger(),
            "Action completed successfully.");
    }

    rclcpp_action::Server<GoToCharger>::SharedPtr
        action_server_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<ChargingActionServer>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}