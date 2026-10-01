#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "task3_action_service/action/go_to_charger.hpp"

using namespace std::chrono_literals;

class ChargingActionClient : public rclcpp::Node
{
public:
    using GoToCharger =
        task3_action_service::action::GoToCharger;

    using GoalHandleGoToCharger =
        rclcpp_action::ClientGoalHandle<GoToCharger>;

    ChargingActionClient()
        : Node("charging_action_client")
    {
        client_ =
            rclcpp_action::create_client<GoToCharger>(
                this,
                "go_to_charger");

        RCLCPP_INFO(
            this->get_logger(),
            "Charging Action Client started.");
    }

    void send_goal()
    {
        RCLCPP_INFO(
            this->get_logger(),
            "Waiting for action server...");

        if (!client_->wait_for_action_server(5s))
        {
            RCLCPP_ERROR(
                this->get_logger(),
                "Action server not available.");

            return;
        }

        RCLCPP_INFO(
            this->get_logger(),
            "Action server available.");

        GoToCharger::Goal goal;

        RCLCPP_INFO(
            this->get_logger(),
            "Sending GoToCharger goal...");

        rclcpp_action::Client<GoToCharger>::SendGoalOptions
            send_goal_options;

        send_goal_options.feedback_callback =
            std::bind(
                &ChargingActionClient::feedback_callback,
                this,
                std::placeholders::_1,
                std::placeholders::_2);

        send_goal_options.result_callback =
            std::bind(
                &ChargingActionClient::result_callback,
                this,
                std::placeholders::_1);

        client_->async_send_goal(
            goal,
            send_goal_options);
    }

private:


    void feedback_callback(
        GoalHandleGoToCharger::SharedPtr goal_handle,
        const std::shared_ptr<const GoToCharger::Feedback>
            feedback)
    {
        (void)goal_handle;

        RCLCPP_INFO(
            this->get_logger(),
            "[feedback] Distance remaining: %.1f m",
            feedback->distance_remaining);
    }


    void result_callback(
        const GoalHandleGoToCharger::WrappedResult & result)
    {
        switch (result.code)
        {
            case rclcpp_action::ResultCode::SUCCEEDED:
            {
                RCLCPP_INFO(
                    this->get_logger(),
                    "✔ Result: %s",
                    result.result->message.c_str());

                break;
            }

            case rclcpp_action::ResultCode::ABORTED:
            {
                RCLCPP_ERROR(
                    this->get_logger(),
                    "✘ Action aborted.");

                break;
            }

            case rclcpp_action::ResultCode::CANCELED:
            {
                RCLCPP_WARN(
                    this->get_logger(),
                    "⚠ Action canceled.");

                break;
            }

            default:
            {
                RCLCPP_ERROR(
                    this->get_logger(),
                    "Unknown result code.");

                break;
            }
        }

        rclcpp::shutdown();
    }

    rclcpp_action::Client<GoToCharger>::SharedPtr
        client_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<ChargingActionClient>();

    node->send_goal();

    rclcpp::spin(node);

    return 0;
}