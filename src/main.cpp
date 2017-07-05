#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// This thing is need it to run the code in my computer
//#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}


int t = 0;
double set_speed = 30; // desired velocity

int main()
{
  uWS::Hub h;

  PID pid_pos;
  PID pid_speed;
  // TODO: Initialize the pid variable.
  
  pid_pos.Init(0.07, 0.007, 0.7); 
  pid_speed.Init(10, 1, 20.3);

  h.onMessage([&pid_pos, &pid_speed](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          
          
          
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value; 
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          t++; //time counter

          if( t > 10){
            pid_speed.i_error = 0;
            pid_pos.i_error   = 0;
            t = 0;
          }

          // pid for speed
          double speed_cte = speed - set_speed ;
          pid_speed.UpdateError(speed_cte);
          double throttle = pid_speed.Control();
          // I asume the boundaries of the trhottle as I no see any difference above 20
          if (throttle > 20)
            throttle = 20;
          if (throttle < -20)
            throttle = -20;

 
          // For tuning only 
          /*
          if( fabs(cte) > 5){
            std::string reset_msg = "42[\"reset\",{}]";
            ws.send(reset_msg.data(), reset_msg.length(), uWS::OpCode::TEXT);
          }
          */

          // pid for position
          pid_pos.UpdateError(cte);
          steer_value = pid_pos.Control();
          if (steer_value > 1)
            steer_value = 1;
          if (steer_value < -1)
            steer_value = -1;


          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
    else{
      //For tunning only
      /*
      pid_pos.Init(pid_pos.Kp, 0, pid_pos.Kd + 0.01);
      set_speed += 5;
      t = 0;
      */
    }


  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen("0.0.0.0", port)) //changed for windows
  //if (h.listen(port)) //original
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
