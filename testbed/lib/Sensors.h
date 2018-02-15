#ifndef SENSORS_H
#define SENSORS_H

#include "Common/MPU9250.h"
#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"
#include <unistd.h>
#include <string>

#define G_SI 9.80665
#define PI   3.14159

struct imu_struct{
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
};

class Sensors{

public:
    bool isISEnabled;
    struct imu_struct imu;
    struct imu_struct bias;

    Sensors (std::string sensor_name);
    void update();
    void calibrateGyro();

private:
    InertialSensor *is;

};

#endif //SENSORS_H
