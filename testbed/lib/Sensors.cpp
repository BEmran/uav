#include "Sensors.h"

Sensors::Sensors (std::string sensor_name){

    if (sensor_name == "mpu") {
        printf("Selected: MPU9250\n");
        is = new MPU9250();
        isISEnabled = !is->probe();
        is->initialize();
    }
    else if (sensor_name == "lsm") {
        printf("Selected: LSM9DS1\n");
        is = new LSM9DS1();
        isISEnabled = !is->probe();
        is->initialize();
    }
    else {
        is = NULL;
        isISEnabled = false;
    }

}

void Sensors::update(){
    is->update();
    is->read_accelerometer(&imu.ax, &imu.ay, &imu.az);
    is->read_gyroscope(&imu.gx, &imu.gy, &imu.gz);
    is->read_magnetometer(&imu.mx, &imu.my, &imu.mz);
}

