#include <chrono>
#include <memory>
#include <future>

#include "rclcpp/rclcpp.hpp"
#include "task3_action_service/srv/go_to_charger.hpp"

using namespace std::chrono_literals;

class ChargingServiceClient : public rclcpp::Node
{
public:
    ChargingServiceClient() : Node("charging_service_client")
    {
        client_ =this->create_client<task3_action_service::srv::GoToCharger>("go_to_charger");

        RCLCPP_INFO(this->get_logger(),"Charging Service Client started.");
    }

    void send_request()
    {
        RCLCPP_INFO(this->get_logger(),"Waiting for charging service...");

        while (!client_->wait_for_service(1s))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_ERROR(this->get_logger(),"ROS shutdown while waiting for service.");
                return;
            }

            RCLCPP_INFO(this->get_logger(),"Service not available, waiting...");
        }

        auto request =std::make_shared<task3_action_service::srv::GoToCharger::Request>();

        RCLCPP_INFO(this->get_logger(),"Requesting charging station trip via SERVICE...");

        auto future =client_->async_send_request(request);

        RCLCPP_INFO(this->get_logger(),"Waiting for response (5s timeout)...");

        auto status =future.wait_for(5s);

        if (status == std::future_status::ready)
        {
            auto response = future.get();

            if (response->success)
            {
                RCLCPP_INFO(this->get_logger(),"Service completed: %s",response->message.c_str());
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(),"Service failed: %s",response->message.c_str());
            }
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(),"✘ TIMED OUT — service did not respond within 5 seconds!");
        }
    }

private:
    rclcpp::Client<task3_action_service::srv::GoToCharger>::SharedPtr client_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node =std::make_shared<ChargingServiceClient>();

    node->send_request();

    rclcpp::shutdown();

    return 0;
}