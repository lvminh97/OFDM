#ifndef _FFT_H
#define _FFT_H

#include <vector>
#include "complex.h"
using namespace std;

#ifndef PI
#define PI = 3.141592653589793238460
#endif

// FFT Function
void fft(vector<complex>& x){
        int N = x.size();
   if (N <= 1) return;
   // divide
   vector<complex> even,odd;
   for(int i = 0; i < N; i++){
        if(i % 2) odd.push_back(x[i]);
        else even.push_back(x[i]);
        }
   //
   fft(even);
   fft(odd);
   // combine
   for (int  k = 0; k < N / 2; k++) {
        complex tmp;
        tmp.setPole(1.0, -2 * PI * k / N);
                tmp = tmp * odd[k];
        x[k] = even[k] + tmp;
        x[k + N / 2] = even[k] - tmp;
   }
}

// inverse fft
void ifft(vector<complex>& x){
   // conjugate the complex numbers
        for(size_t i = 0; i < x.size(); i++){
        x[i] = x[i].conj();
        }
   // forward fft
   fft(x);
   // conjugate the complex numbers again
        for(size_t i = 0; i < x.size(); i++){
        x[i] = x[i].conj();
        x[i] = x[i] / (float) x.size();
        }
}

#endif
