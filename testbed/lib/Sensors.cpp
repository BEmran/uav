#include "Sensors.h"

Sensors::Sensors (std::string sensor_name){
    bias.gx = 0.0;
    bias.gy = 0.0;
    bias.gz = 0.0;

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

// apply calibration
    imu.gx -= bias.gx;
    imu.gy -= bias.gy;
    imu.gz -= bias.gz;
}

void Sensors::calibrateGyro()
{
    //---------------------- Calculate the offset -----------------------------

    float offset[3] = {0.0, 0.0, 0.0};

    //-------------------------------------------------------------------------

    printf("Beginning Gyro calibration...\n");
    for(int i = 0; i<500; i++)
    {
        update();

        imu.gx *= 180 / PI;
        imu.gy *= 180 / PI;
        imu.gz *= 180 / PI;

        offset[0] += imu.gx*0.0175;
        offset[1] += imu.gy*0.0175;
        offset[2] += imu.gz*0.0175;

        usleep(10000);
    }
    offset[0]/=500.0;
    offset[1]/=500.0;
    offset[2]/=500.0;

    printf("Offsets are: %f %f %f\n", offset[0], offset[1], offset[2]);

    bias.gx = offset[0];
    bias.gx = offset[1];
    bias.gx = offset[2];
}
