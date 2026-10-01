#include <chrono>
#include <memory>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "task3_action_service/srv/go_to_charger.hpp"

using namespace std::chrono_literals;

class ChargingServiceServer : public rclcpp::Node
{
public:
    ChargingServiceServer(): Node("charging_service_server")
    {
        service_ = this->create_service<task3_action_service::srv::GoToCharger>("go_to_charger",std::bind(&ChargingServiceServer::handle_request,this,std::placeholders::_1,std::placeholders::_2));

        RCLCPP_INFO(this->get_logger(),"Charging Service Server started.");
    }

private:
    void handle_request(const std::shared_ptr<task3_action_service::srv::GoToCharger::Request> request,std::shared_ptr<task3_action_service::srv::GoToCharger::Response> response)
    {
        (void)request;

        RCLCPP_INFO(this->get_logger(),"Received request: Go to charging station.");

        RCLCPP_INFO(this->get_logger(),"Starting 30-second charging station trip...");

        std::this_thread::sleep_for(30s);

        response->success = true;
        response->message ="Arrived at charging station!";

        RCLCPP_INFO(this->get_logger(),"30-second task completed.");

        RCLCPP_INFO(this->get_logger(),"Sending service response.");
    }

    rclcpp::Service<task3_action_service::srv::GoToCharger>::SharedPtr service_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<ChargingServiceServer>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}