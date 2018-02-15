#ifndef AHRS_HPP
#define AHRS_HPP

#include <cmath>
#include <stdio.h>
#include <Sensors.h>

#define G_SI 9.80665
#define PI   3.14159


struct imu_struct{
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
};


class AHRS{
private:
	float q0, q1, q2, q3;
	float gyroOffset[3];
	float twoKi;
	float twoKp;
	float integralFBx, integralFBy, integralFBz;
        Sensors* sensors;
public:
    AHRS(Sensors* sensors);

    void update(float dt);
    void updateIMU(float dt);
    void setGyroOffset();
    void getEuler(float* roll, float* pitch, float* yaw);

    float invSqrt(float x);
    float getW();
    float getX();
    float getY();
    float getZ();
};

#endif // AHRS_hpp
