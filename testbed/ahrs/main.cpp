/*
This code is provided under the BSD license.
Copyright (c) 2014, Emlid Limited. All rights reserved.
Written by Igor Vereninov and Mikhail Avkhimenia
twitter.com/emlidtech || www.emlid.com || info@emlid.com

Application: Mahory AHRS algorithm supplied with data from MPU9250 and LSM9DS1.
Outputs roll, pitch and yaw in the console and sends quaternion
over the network - it can be used with 3D IMU visualizer located in
Navio/Applications/3D IMU visualizer.

To run this app navigate to the directory containing it and run following commands:
make
sudo ./AHRS -i [sensor name] ipaddress portnumber
Sensors names: mpu is MPU9250, lsm is LSM9DS1.
If you want to visualize IMU data on another machine pass it's address and port
For print help:
./AHRS -h

To achieve stable loop you need to run this application with a high priority
on a linux kernel with real-time patch. Raspbian distribution with real-time
kernel is available at emlid.com and priority can be set with chrt command:
chrt -f -p 99 PID
*/

#include <stdio.h>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include "AHRS.hpp"

class Socket
{

public:
    Socket(char * ip,char * port)
    {
        sockfd = socket(AF_INET,SOCK_DGRAM,0);
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ip);
        servaddr.sin_port = htons(atoi(port));
    }

    Socket()
    {
        sockfd = socket(AF_INET,SOCK_DGRAM,0);
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(7000);
    }

    void output(float W, float X, float Y, float Z, int Hz)
    {
        sprintf(sendline,"%10f %10f %10f %10f %dHz\n", W, X, Y, Z, Hz);
        sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }

private:
    int sockfd;
    struct sockaddr_in servaddr = {0};
    char sendline[80];

};


void print_help()
{
    printf("Possible parameters:\nSensor selection: -i [sensor name]\n");
    printf("Sensors names: mpu is MPU9250, lsm is LSM9DS1\nFor help: -h\n");
    printf("If you want to visualize IMU data on another machine,\n");
    printf("add IP address and port number (by default 7000):\n");
    printf("-i [sensor name] ipaddress portnumber\n");

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

//============================== Main loop ====================================

void imuLoop(AHRS* ahrs, Socket sock)
{
    // Orientation data

    float roll, pitch, yaw;

    struct timeval tv;
    float dt;
    // Timing data

    static float maxdt;
    static float mindt = 0.01;
    static float dtsumm = 0;
    static int isFirst = 1;
    static unsigned long previoustime, currenttime;


    //----------------------- Calculate delta time ----------------------------

	gettimeofday(&tv,NULL);
	previoustime = currenttime;
	currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
	dt = (currenttime - previoustime) / 1000000.0;
	if(dt < 1/1300.0) usleep((1/1300.0-dt)*1000000);
        gettimeofday(&tv,NULL);
        currenttime = 1000000 * tv.tv_sec + tv.tv_usec;
	dt = (currenttime - previoustime) / 1000000.0;

    //-------- Read raw measurements from the MPU and update AHRS --------------

    ahrs->updateIMU(dt);


    //------------------------ Read Euler angles ------------------------------

    ahrs->getEuler(&roll, &pitch, &yaw);

    //------------------- Discard the time of the first cycle -----------------

    if (!isFirst)
    {
    	if (dt > maxdt) maxdt = dt;
    	if (dt < mindt) mindt = dt;
    }
    isFirst = 0;

    //------------- Console and network output with a lowered rate ------------

    dtsumm += dt;
    if(dtsumm > 0.05)
    {
        // Console output
        printf("ROLL: %+05.2f PITCH: %+05.2f YAW: %+05.2f PERIOD %.4fs RATE %dHz \n", roll, pitch, yaw * -1, dt, int(1/dt));

        // Network output
        sock.output( ahrs->getW(), ahrs->getX(), ahrs->getY(), ahrs->getZ(), int(1/dt));

        dtsumm = 0;
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

    //--------------------------- Network setup -------------------------------

    Socket sock;

    if (argc == 5)
        sock = Socket(argv[3], argv[4]);
    else if ( (get_navio_version() == NAVIO) && (argc == 3) )
            sock = Socket(argv[1], argv[2]);
        else
            sock = Socket();

    AHRS *ahrs = new AHRS(sensors);

    //-------------------- Setup gyroscope offset -----------------------------

    ahrs->setGyroOffset();
    while(1)
        imuLoop(ahrs, sock);
}
