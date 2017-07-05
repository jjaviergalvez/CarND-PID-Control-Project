#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double KP, double KI, double KD) {

	Kp = KP;
	Ki = KI;
	Kd = KD;

	i_error = 0;
	p_error = 0;
	d_error = 0;
}

void PID::UpdateError(double cte) {
	d_error = cte - p_error;
	i_error += cte;
	p_error = cte;
}

double PID::TotalError() {
	//double err;
	//err = p_error + i_error + d_error;
	return 0.0;
}

double PID::Control(){
	double u;
	u = -Kp*p_error - Ki*i_error - Kd*d_error;
	return u; 
}

