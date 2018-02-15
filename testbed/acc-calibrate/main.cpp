#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include "Sensors.h"

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
   
    printf("Start accelerometer calibration procedure\n");
    printf("Place the autopilot >>>> Flat <<<< and press enter\n");
    getchar();
    az_max = avgAcc(sensors, 2);
    
    printf("Place the autopilot on its >>>> RIGHT<<<< and press enter\n");
    getchar();
    ay_max = avgAcc(sensors, 1);

    printf("Place the autopilot on its >>>> LEFT <<<< and press enter\n");
    getchar();
    ay_min = avgAcc(sensors, 1);

    printf("Place the autopilot facing >>>> DOWN <<<< and press enter\n");
    getchar();
    ax_max = avgAcc(sensors, 0);

    printf("Place the autopilot facing >>>> UP <<<< and press enter\n");
    getchar();
    ax_min = avgAcc(sensors, 0);

    printf("Place the autopilot on its >>>> BACK <<<< and press enter\n");
    getchar();
    az_min = avgAcc(sensors, 2);  
    
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
    printf("Bias  value for x-axis:%+10.5f  z-axis:%+10.5f  z-axis:%+10.5f\n",
            ax_bias,ay_bias,az_bias);
    printf("Scale value for x-axis:%+10.5f  z-axis:%+10.5f  z-axis:%+10.5f\n",
            ax_scale,ay_scale,az_scale);
    printf("-------------------------------------------------------------\n");

    fprintf (file,"bias %+10.5f %+10.5f %+10.5f\n",ax_bias,ay_bias,az_bias);
    fprintf (file,"scale  %+10.5f %+10.5f %+10.5f\n",ax_scale,ay_scale,az_scale);
    fclose(file);

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

float avgAcc(Sensors* sensors, int axis) {
    //--------------------------------------------------------------------------
    printf("Beginning axis-%d calibration...\n", axis);
    float maxCount = 500.0;
    float sum = 0;
    for (int i = 0; i < int(maxCount); i++) {
        sensors->updateIMU();
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
    sum /= maxCount;

    printf("average data of axis-%d is %+10.5f \n", axis, sum);
    
    return avg;
}

