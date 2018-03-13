/* 
 * File:   Encoder.cpp
 * Author: root
 * 
 * Created on March 8, 2018, 11:18 AM
 */

#include "Encoder.h"

static void CCONV onAttachHandler(PhidgetHandle h, void *ctx) {
    int* channel = (int*) ctx;
    printf("channel %d on yor device attached\n", *channel);
}

static void CCONV onDetachHandler(PhidgetHandle h, void *ctx) {
    int* channel = (int*) ctx;
    printf("channel %d on your device detached\n", *channel);
}

static void CCONV errorHandler(PhidgetHandle h, void *ctx,
        Phidget_ErrorEventCode errorCode, const char *errorString) {
    fprintf(stderr, "Error: %s (%d)\n", errorString, errorCode);
}

Encoder::Encoder() {
    // Initialize parameters
    _count[0] = 0;
    _count[1] = 0;
    _count[2] = 0;
    _index[0] = 0;
    _index[1] = 1;
    _index[2] = 2;
    _serial = 0;
    _is_enbaled[0] = false;
    _is_enbaled[1] = false;
    _is_enbaled[2] = false;
    PhidgetReturnCode res;
    const char *errs;
    
    // Create a file to store the row data
    row_data_file = fopen("row_data_encoder.txt", "w");
    printf("Start storing the encoder data in the file \"row_data_encoder.txt\" \n");
    fprintf(row_data_file, "time, ang0, ang1, ang2\n");

    
    // initialize encoders
    for (int i = 0; i < 3; ++i) {
        if (init(i)) {
            // Set encoder to a specific channel
            setChannel(i);

            // get info of encoder device
            getInfo(i);

            // Set data interval to 8 ms
            PhidgetEncoder_setDataInterval(_eh[i], 8);

            // Open the channel synchronously: waiting a maximum of 5 seconds.
            res = Phidget_openWaitForAttachment((PhidgetHandle) _eh[i], 5000);
            if (res != EPHIDGET_OK) {
                if (res == EPHIDGET_TIMEOUT) {
                    printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
                } else {
                    Phidget_getErrorDescription(res, &errs);
                    fprintf(stderr, "failed to open channel:%s\n", errs);
                }
            }
            if (res != EPHIDGET_OK) {
                printf("Initialization of channel %d on device %d is successful\n", i, _serial);
            }
            setEnable(true, i);
        } else {
            printf("Initialization of channel %d on device %d is unsuccessful\n", i, _serial);
        }
    }
}

Encoder::~Encoder() {
    for (int i = 0; i < 3; ++i) {

        Phidget_close((PhidgetHandle) _eh[i]);
        PhidgetEncoder_delete(&_eh[i]);
    }
}

void Encoder::setChannel(int i) {
    _channel[i] = i;
    Phidget_setChannel((PhidgetHandle) _eh[i], _channel[i]);
}

void Encoder::updateCounts() {
    for (int ch = 0; ch < 3; ++ch) {
        PhidgetEncoder_getPosition(_eh[ch], &_count[ch]);
        PhidgetEncoder_getIndexPosition(_eh[ch], &_index[ch]);
        _count[ch] = _count[ch] - _index[ch];
    }
    storeData();
}

void Encoder::getCounts(long counts[]) const {
    for (int ch = 0; ch < 3; ++ch) {
        counts[ch] = _count[ch];
    }
}

void Encoder::readAnglesRad(float angle[]) const {
    for (int ch = 0; ch < 3; ++ch) {
        angle[ch] = _count[ch] / MAXCOUNT * 2 * PI;
    }
}

void Encoder::readAnglesDeg(float angle[]) const {
    for (int ch = 0; ch < 3; ++ch) {
        angle[ch] = _count[ch] / MAXCOUNT * 360.0;
    }
}


void Encoder::setCount(const int ch, const long int count) {
    PhidgetEncoder_setPosition(_eh[ch], count);
    _count[ch] = count;
}

void Encoder::setCounts(const long int count[]) {
    for (int ch = 0; ch < 3; ++ch) {
        setCount(ch, count[ch]);
    }
}

bool Encoder::setEnable(bool en, int i) {
    PhidgetReturnCode res;

    // Change encoder enable case
    res = PhidgetEncoder_setEnabled(_eh[i], 1);

    // No error occurred
    if (res == EPHIDGET_OK) {
        if (en) {
            printf("Encoder on ch:%d is now enable\n", _channel[i]);
            _is_enbaled[i] = true;
        } else {
            printf("Encoder on ch:%d is now disabled\n", _channel[i]);
            _is_enbaled[i] = true;
        }
        return true;
    } else {
        // An error occurred
        printf("Can not change the enable of encoder for ch:%d error code %x\n", _channel[i], res);
        return false;
    }
}

bool Encoder::init(int i) {
    PhidgetReturnCode res;

    // Enable logging to stdout
    PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

    // Create encoder handler
    res = PhidgetEncoder_create(&_eh[i]);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to create voltage ratio input channel\n");
        return false;
    }

    // Set attach handler function
    res = Phidget_setOnAttachHandler((PhidgetHandle) _eh[i], onAttachHandler, &_channel[i]);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on attach handler\n");
        return false;
    }

    // Set detach handler function
    res = Phidget_setOnDetachHandler((PhidgetHandle) _eh[i], onDetachHandler, &_channel[i]);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on detach handler\n");
        return false;
    }

    // Set error handler function
    res = Phidget_setOnErrorHandler((PhidgetHandle) _eh[i], errorHandler, &_channel[i]);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to assign on error handler\n");
        return false;
    }
    return true;
}

bool Encoder::getInfo(int i) {
    PhidgetReturnCode res;

    res = Phidget_getDeviceSerialNumber((PhidgetHandle) _eh[i], &_serial);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get device serial number\n");
        return false;
    }

    res = Phidget_getChannel((PhidgetHandle) _eh[i], &_channel[i]);
    if (res != EPHIDGET_OK) {
        fprintf(stderr, "failed to get channel number\n");
        return false;
    }

    return true;
}

//**************************************************************************
// Store row measurements
//**************************************************************************

void Encoder::storeData() {
    // get current time
    getTime();
    // read angle value
    float ang[3];
    readAnglesRad(ang);
    // Write data
    fprintf(row_data_file, "%12lu,",time_now);
    fprintf(row_data_file, " %+10.5f,  %+10.5f,  %+10.5f\n",ang[0], ang[1], ang[2]);
}
//**************************************************************************
// Get the current time
//**************************************************************************

void Encoder::getTime() {
    // Timing data
    struct timeval tv;

    // Calculate delta time
    gettimeofday(&tv, NULL);
    time_now = 1000000 * tv.tv_sec + tv.tv_usec;
}