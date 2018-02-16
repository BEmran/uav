#include "Sensors.h"
#include <unistd.h>
#include <string>
#include <memory>
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

    while(1) {
	sensors->update();
        printf("Acc: %+7.3f %+7.3f %+7.3f  ", sensors->imu.ax, sensors->imu.ay, sensors->imu.az);
        printf("Gyr: %+8.3f %+8.3f %+8.3f  ", sensors->imu.gx, sensors->imu.gy, sensors->imu.gz);
        printf("Mag: %+7.3f %+7.3f %+7.3f\n", sensors->imu.mx, sensors->imu.my, sensors->imu.mz);

        usleep(100000);
    }
}

