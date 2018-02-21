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
    printf("Possible parameters:\n"
           "- Sensor selection: -i [sensor name]\n");
    printf("                        Sensors names: \n"
           "                                     -> mpu is MPU9250\n"
           "                                     -> lsm is LSM9DS1\n"
           "                                     -> both for both sensors\n"
           "- For help: -h\n");
}
//=============================================================================
std::string get_sensor_name(int argc, char *argv[], bool& is_both_sensors)
{
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
        case 'i':
	    if (!strcmp(optarg,"mpu"))
		 return "mpu";
            else if (!strcmp(optarg,"lsm"))
		 return "lsm";
	    else if (!strcmp(optarg,"both"))
                 is_both_sensors = true;
   	    return "mpu";	// if both or anything else return mpu
        case 'h':
	    print_help();
	    return "-1";
        case '?':
	    printf("Wrong parameter.\n");
            print_help();
            return std::string();
        }
    }
}
//=============================================================================
int main(int argc, char *argv[])
{
   bool is_both_sensors = false;

    if (check_apm()) {
        return 1;
    }

    auto sensor_name = get_sensor_name(argc, argv, is_both_sensors);
    if (sensor_name.empty())
        return EXIT_FAILURE;

    Sensors* sensors1 = new Sensors(sensor_name);
    Sensors* sensors2;
    if (is_both_sensors)
	sensors2 = new Sensors("lsm");

    if (!sensors1->isISEnabled) {
        printf("Sensor is not enabled\n");
        return EXIT_FAILURE;
    }

//-------------------------------------------------------------------------
    float dt;
    static float dtsumm = 0;
    while(1) {
	sensors1->update();
	if (is_both_sensors)
	    sensors2->update();
        dt = getTime(400.0);
	dtsumm += dt;
	if(dtsumm > 0.2){
            dtsumm = 0;
            printf("Hz %d  ", int(1/dt));
            printf("Acc: %+7.3f %+7.3f %+7.3f  ", sensors1->imu.ax, sensors1->imu.ay, sensors1->imu.az);
            printf("Gyr: %+8.3f %+8.3f %+8.3f  ", sensors1->imu.gx, sensors1->imu.gy, sensors1->imu.gz);
            printf("Mag: %+7.3f %+7.3f %+7.3f\n", sensors1->imu.mx, sensors1->imu.my, sensors1->imu.mz);
        }
    }
}

