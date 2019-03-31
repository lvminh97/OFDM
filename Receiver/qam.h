#ifndef _QAM_H
#define _QAM_H

#include <vector>
#include "complex.h"
using namespace std;

#ifndef PI
#define PI 3.141592653589793238460
#endif

float amp[16]   = {1.0, 0.75, 1.0, 0.75, 0.75, 0.33, 0.75, 0.33, 1.0, 0.75, 1.0, 0.75, 0.75, 0.33, 0.75, 0.33};
float phase[16] = {0.75, 0.917, 1.25, 1.083, 0.583, 0.75, 1.417, 1.25, 0.25, 0.083, 1.75, 1.917, 0.417, 0.25, 1.583, 1.75};

complex mean(vector<complex> v){		// Tinh gia tri trung binh cua mot day so phuc
    complex t;
    for (size_t i = 0; i < v.size(); i++){
        t = t + v[i];
    }
    return t / v.size();
}

float stdDev(vector<complex> v){		// Tinh do lech chuan mau
    complex _mean = mean(v);
    float s = 0;
    for (size_t i = 0; i < v.size(); i++){
        float mod = (v[i] - _mean).module();
        s += mod * mod;
    }
    return sqrt(s / (v.size() - 1));
}

complex qamMap(unsigned char dat){
    complex tmp;
    tmp.setPole(amp[dat], phase[dat] * PI);
    return tmp;
}

unsigned char qamMap(complex c){
    unsigned char tmp = 0;
    if (c.argument() >= 0 && c.argument() < PI / 6){
        tmp = 9;
    }
    else if (c.argument() >= PI / 6 && c.argument() < PI / 3){
        tmp = c.module() >= 0.67 ? 8 : 13;
    }
    else if (c.argument() >= PI / 3 && c.argument() < PI / 2){
        tmp = 12;
    }
    else if (c.argument() >= PI / 2 && c.argument() < 2 * PI / 3){
        tmp = 4;
    }
    else if (c.argument() >= 2 * PI / 3 && c.argument() < 5 * PI / 6){
        tmp = c.module() >= 0.67 ? 0 : 5;
    }
    else if (c.argument() >= 5 * PI / 6 && c.argument() < PI){
        tmp = 1;
    }
    else if (c.argument() >= PI && c.argument() < 7 * PI / 6){
        tmp = 3;
    }
    else if (c.argument() >= 7 * PI / 6 && c.argument() < 4 * PI / 3){
        tmp = c.module() >= 0.67 ? 2 : 7;
    }
    else if (c.argument() >= 4 * PI / 3 && c.argument() < 3 * PI / 2){
        tmp = 6;
    }
    else if (c.argument() >= 3 * PI / 2 && c.argument() < 5 * PI / 3){
        tmp = 14;
    }
    else if (c.argument() >= 5 * PI / 3 && c.argument() < 11 * PI / 6){
        tmp = c.module() >= 0.67 ? 10 : 15;
    }
    else if (c.argument() >= 11 * PI / 6 && c.argument() < 2 * PI){
        tmp = 11;
    }
    return tmp;
}

vector<complex> qamMod(vector<unsigned char> dat){		// Dieu che QAM16
    vector<complex> qamSig;
    for (size_t i = 0; i < dat.size(); i++){		// Lay tung nhom 4 bit de xu ly
        qamSig.push_back(qamMap((dat[i] >> 4) & 15));
        qamSig.push_back(qamMap((dat[i] & 15)));
    }
    return qamSig;
}

vector<unsigned char> qamDemod(vector<complex> qamSig){	// Giai dieu che QAM16
    vector<unsigned char> dat;
    for (size_t i = 0; i < qamSig.size(); i += 2){
        unsigned char tmp = 0;
        tmp = qamMap(qamSig[i]);
        tmp <<= 4;
        tmp += qamMap(qamSig[i + 1]);
        dat.push_back(tmp);
    }
    return dat;
}

#endif
