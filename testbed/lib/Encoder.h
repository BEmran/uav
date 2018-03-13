/* 
 * File:   Encoder.h
 * Author: root
 *
 * Created on March 8, 2018, 11:18 AM
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <phidget22.h>
#include <unistd.h>

static void CCONV onAttachHandler(PhidgetHandle h, void *ctx);
static void CCONV onDetachHandler(PhidgetHandle h, void *ctx);
static void CCONV errorHandler(PhidgetHandle h, void *ctx,
        Phidget_ErrorEventCode errorCode, const char *errorString);

namespace {
    #define MAXCOUNT 40000.0
    #define PI 3.14159
}

class Encoder {
   
public:
    bool _is_enbaled[3];
    Encoder();
    virtual ~Encoder();
    bool getInfo(int i);
    bool init(int i);
    bool setEnable(bool en,int i);
    void updateCounts();
    void getCounts(long counts[]) const;
    void readAnglesRad(float angle[]) const;
    void readAnglesDeg(float angle[]) const;    
    void setChannel(int i);
    void setCount(const int ch, const long int count);
    void setCounts(const long int count[]);

private:
    int _serial;
    int _channel[3];
    int64_t _count[3];
    int64_t _index[3];
    PhidgetEncoderHandle _eh[3];
};

#endif /* ENCODER_H */

