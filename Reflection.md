# Reflection of CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Describe the effect each of the P, I, D components had in your implementation.

I use two PID controllers, one for the position and another for the velocity.


PID for position

* Proportional
This term control how much need to steer in relation to error position. When I set it, with a small value, the car takes a lot of time to recover to the path and has a small possibility to follow a smooth turn (or any) as I show [in this video](./video/Kp_03-Ki_none-Kd_none.mp4) where I set Kp = 0.03 and Ki and Kd = 0. On the other side, if the proportional constant is big, the car may be is able to make a turn but it going to have zigzag in a straightforward line path or even go out of the road. It seems that this term lonely is not capable of keeping the car safe on the road unless at 30 mph (a minimum velocity that I tried to drive the car since in the behavioral cloning project was the velocity I achieve).

* Integral
This term is normally used it for reducing the steady error consequence of the defects of the plant (in this case the car). I notice that when I send a zero steer control value to the vehicle, the angle in the simulation is 0.44°. That is a good reason to add an integral term to the controller in order to achieve better performance. From the lessons of Sebastian, we implement the error of this integral term as the sum of all the error since the beginning. However, we are trying to design a path tracking algorithm instead of regularization (the step function input in the classroom example). Considering the track like small steps, I reset the integral error every 10 control times. If I didn't do that, after a curve, where most of the time doesn't have a zero error, the steering angle will stay to a high degree and cause the car goes out of the road after the turn. The idea of doing this come to me by reading the [doi:10.1016/j.procs.2015.12.329](https://doi.org/10.1016/j.procs.2015.12.329). 

* Derivative
The derivative term has an important role in the control of the car. The ideal value in a step input target, allow having a smooth path as the cte approximates to zero, eliminating the zigzag that you have with a purely proportional control as it can be appreciated in [this video](./video/Kp_03-Ki_none-Kd_7.mp4) I made where we have Kp=0.03 and Kd=0.07. Low gain makes the car overshoot the target and after some bumps, it approximates the cte to zero. On the other hand, high gain makes the car approximates to the car very slowly. I find out in practice, that the "ideal value" for a step input target, cause little oscillations the car during a turn. For that reason, I choose a gain between low and ideal value looking for getting a better performance in a turn and only some bumps in a straightforward line. 


PID for velocity

This one, was difficult to see the effect of each gain, but this is was what I find:
* Proportional
This gain has the effect to try to get the value as close as possible.

* Integral
This adds robustness to the velocity control. Although in this simulation this gain may don't have a big impact, it can be appreciated in a sloped roads, like the second track. 

* Derivative
This value has an important role in case we need to change the desired velocity quickly or when we have sloped roads. For this road, may it's noted strictly necessary, but as it seems not affect the performance, I keep it.

An experiment I made for [control of the velocity can be found in this video](./video/PID_of_velocity.mp4), where I run manually the car at high speed until the bridge and then change to automatic mode to allow the control make the job. The desired velocity was set to 30mph.


## Describe how the final hyperparameters were chosen

In order to compare the performance with the behavioral cloning project, a constant velocity of 30mph was chosen. At the beginning, I tried to use the twiddle algorithm to tune the PID, but it gives me a solution that converts PID control into a kind of bang-bang control. For that reason I tried to follow the heuristic used commonly to manually tune a PID control for a line follower robot:

 1. Set all the gains to zero
 2. Increase only Kp (Proportional gain) to get an ultimate oscillating value Kp(max).
 3. Increase Kd (Derivative gain) until the oscillations disappear.
 4. Repeat steps 2 and 3 until increasing Kd does not dampen the oscillations
 5. Now increase Ki (Integral gain) to get a good system with the desired number of oscillations (Ideally zero)

 from [Quora](https://www.quora.com/What-would-be-appropriate-tuning-factors-for-PID-line-follower-robot). Specifically, I manually drove the car until the bridge, where I turn on the autonomous mode. That allow me, although not in a repetitive way, to represent a step input function. In that condition, I apply the steps 2 and 3 of above. Ending with a Kp = 0.03, Ki = 0 and Kd = 0.7. Then I turn on the autonomous mode with that gains and increase gradually Kp until successful complete all the track, that value was Kp = 0.07. I tried the same with Kd without better results, so I keep the same value as before. Finally, I add a little bit of integral gain, until I got a better performance (visually, not with the total cte).


 For PID for velocity, I concetrate most in the P gain. I set a boundary of the output of [20,-20] and play with the gain until I get the desired velocity without to much throtle. Base on that value and the experience gained tuning the PID for position, I set the I and D gains "aproximatly" in the same proportion as I and D of position PID.
