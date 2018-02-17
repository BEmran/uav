#include "Sensors.h"
#include <unistd.h>
#include <string>
#include <memory>
#include <sys/time.h>

//=============================================================================

float getTime(float hz){
    struct timeval tv;
    float dt;
    // Timing data
    static unsigned long previoustime, currenttime;

    //----------------------- Calculate delta time ----------------------------

        gettimeofday(&tv,NULL);
        previoustime = currenttime;
        currenttime = 1000000 * tv.tv_sec + tv.tv_usec;

        dt = (currenttime - previoustime) / 1000000.0;

        if(dt < 1/hz)
            usleep((1/hz-dt)*1000000);

        gettimeofday(&tv,NULL);
        currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
        dt = (currenttime - previoustime) / 1000000.0;
}
//=============================================================================
void print_help()
{
    printf("Possible parameters:\nSensor selection: -i [sensor name]\n");
    printf("Sensors names: mpu is MPU9250, lsm is LSM9DS1\nFor help: -h\n");
}
//=============================================================================
std::string get_sensor_name(int argc, char *argv[])
{
    if (get_navio_version() == NAVIO2) {

        if (argc < 2) {
            printf("Enter parameter\n");
            print_help();
            return std::string();
        }

        // prevent the error message
        opterr = 0;
        int parameter;

        while ((parameter = getopt(argc, argv, "i:h")) != -1) {
            switch (parameter) {
            case 'i': if (!strcmp(optarg,"mpu") ) return "mpu";
                            else return "lsm";
            case 'h': print_help(); return "-1";
            case '?': printf("Wrong parameter.\n");
                      print_help();
                      return std::string();
            }
        }

    } else { //sensor on NAVIO+

        return "mpu";
    }

}
//=============================================================================
int main(int argc, char *argv[])
{

    if (check_apm()) {
        return 1;
    }

    auto sensor_name = get_sensor_name(argc, argv);
    if (sensor_name.empty())
        return EXIT_FAILURE;


    Sensors* sensors = new Sensors(sensor_name);

    if (sensors->isISEnabled) {
        printf("Wrong sensor name. Select: mpu or lsm\n");
        return EXIT_FAILURE;
    }

   sensors->calibrateGyro();
//-------------------------------------------------------------------------
    float dt;
    static float dtsumm = 0;
    while(1) {
	sensors->update();
        dt = getTime(400.0);
	dtsumm += dt;
	if(dtsumm > 0.2){
            dtsumm = 0;
            printf("Hz %d  ", int(1/dt));
            printf("Acc: %+7.3f %+7.3f %+7.3f  ", sensors->imu.ax, sensors->imu.ay, sensors->imu.az);
            printf("Gyr: %+8.3f %+8.3f %+8.3f  ", sensors->imu.gx, sensors->imu.gy, sensors->imu.gz);
            printf("Mag: %+7.3f %+7.3f %+7.3f\n", sensors->imu.mx, sensors->imu.my, sensors->imu.mz);
        }
    }
}

