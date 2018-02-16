#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include "Sensors.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

float avgAcc(Sensors *sensors, int x, float tmp[3]);
void print_help()
{
    printf("Possible parameters:\nSensor selection: -i [sensor name]\n");
    printf("Sensors names: mpu is MPU9250, lsm is LSM9DS1\nFor help: -h\n");
}

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
            case 'h': print_help(); return "";
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

    FILE* file = fopen("acc_calib.txt","w");

    float ax_max,ay_max,az_max;
    float ax_min,ay_min,az_min;
    float tmpF[3], tmpB[3];
    float tmpR[3], tmpL[3];
    float tmpU[3], tmpD[3];
    printf("Start accelerometer calibration procedure\n");
    printf("Place the autopilot >>>> Flat <<<< and press enter\n");
    getchar();
    az_max = avgAcc(sensors, Z_AXIS, tmpF);

    printf("Place the autopilot on its >>>> RIGHT<<<< and press enter\n");
    getchar();
    ay_max = avgAcc(sensors, Y_AXIS, tmpR);

    printf("Place the autopilot on its >>>> LEFT <<<< and press enter\n");
    getchar();
    ay_min = avgAcc(sensors, Y_AXIS, tmpL);

    printf("Place the autopilot facing >>>> DOWN <<<< and press enter\n");
    getchar();
    ax_max = avgAcc(sensors, X_AXIS, tmpD);

    printf("Place the autopilot facing >>>> UP <<<< and press enter\n");
    getchar();
    ax_min = avgAcc(sensors, X_AXIS, tmpU);

    printf("Place the autopilot on its >>>> BACK <<<< and press enter\n");
    getchar();
    az_min = avgAcc(sensors, Z_AXIS, tmpB);

    float ax_bias,ay_bias,az_bias;
    float ax_scale,ay_scale,az_scale;

    ax_bias = (ax_max + ax_min)/2.0;
    ay_bias = (ay_max + ay_min)/2.0;
    az_bias = (az_max + az_min)/2.0;
    ax_scale = (ax_max - ax_min)/2.0;
    ay_scale = (ay_max - ay_min)/2.0;
    az_scale = (az_max - az_min)/2.0;

    printf("-------------------------------------------------------------\n");
    printf("Finish calibration, Here is the result:\n");
    printf("Bias  value for x-axis:%+10.5f  y-axis:%+10.5f  z-axis:%+10.5f\n",
            ax_bias,ay_bias,az_bias);
    printf("Scale value for x-axis:%+10.5f  y-axis:%+10.5f  z-axis:%+10.5f\n",
            ax_scale,ay_scale,az_scale);
    printf("-------------------------------------------------------------\n");

    fprintf (file,"bias %+10.5f %+10.5f %+10.5f\n",ax_bias,ay_bias,az_bias);
    fprintf (file,"scale  %+10.5f %+10.5f %+10.5f\n",ax_scale,ay_scale,az_scale);
    fclose(file);


    printf("Data for matrix missalignment\n");
    printf(" Flat  0  0  1 => %+10.5f %+10.5f %+10.5f\n", tmpF[0],tmpF[1],tmpF[2]);
    printf(" Back  0  0 -1 => %+10.5f %+10.5f %+10.5f\n", tmpB[0],tmpB[1],tmpB[2]);
    printf(" Right 0  1  0 => %+10.5f %+10.5f %+10.5f\n", tmpR[0],tmpR[1],tmpR[2]);
    printf(" Left  0 -1  0 => %+10.5f %+10.5f %+10.5f\n", tmpL[0],tmpL[1],tmpL[2]);
    printf(" Down  1  0  0 => %+10.5f %+10.5f %+10.5f\n", tmpD[0],tmpD[1],tmpD[2]);
    printf(" Up   -1  0  0 => %+10.5f %+10.5f %+10.5f\n", tmpU[0],tmpU[1],tmpU[2]);
    printf("-------------------------------------------------------------\n");
    printf("-------------------------------------------------------------\n");
 

    while(1){
        // update imu data
        sensors->update();

            // Console output
        printf("%d row %+10.5f %+10.5f %+10.5f calibrated %+10.5f %+10.5f %+10.5f \n",
        	sensors->imu.ax, sensors->imu.ay, sensors->imu.az,
        	(sensors->imu.ax - ax_bias)/ax_scale,
                (sensors->imu.ay - ay_bias)/ay_scale,
                (sensors->imu.az - az_bias)/az_scale);
	usleep(100000);
    }

}

//**************************************************************************
// Find average of a single axis
//**************************************************************************

float avgAcc(Sensors* sensors, int axis, float tmp[3]) {
    //--------------------------------------------------------------------------
    printf("Beginning axis-%d calibration...\n", axis);

    float maxCount = 500.0;
    float sum = 0;
    tmp[0] = 0.0;
    tmp[1] = 0.0;
    tmp[2] = 0.0;

    for (int i = 0; i < int(maxCount); i++) {
        sensors->update();
        tmp[0] += sensors->imu.ax;
        tmp[1] += sensors->imu.ay;
        tmp[2] += sensors->imu.az;

        switch (axis) {
            case(0):
                sum += sensors->imu.ax;
                break;
            case(1):
                sum += sensors->imu.ay;
                break;
            case(2):
                sum += sensors->imu.az;
                break;
        }

        usleep(10000);
    }

    tmp[0] /= maxCount;
    tmp[1] /= maxCount;
    tmp[2] /= maxCount;

    sum /= maxCount;

    printf("average data of axis-%d is %+10.5f \n", axis, sum);

    return sum;
}

