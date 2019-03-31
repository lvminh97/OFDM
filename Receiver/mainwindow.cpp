#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include "complex.h"
#include "qam.h"
#include "fft.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

vector<complex> qamSig;

float randn(){
    return (rand() % 19999 - 9999) / 10000.0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "",
                "Text file (*.txt)"
                );
    QFile inp(filename);
    inp.open(QFile::ReadOnly | QFile::Text);
    QTextStream in(&inp);
    size_t qamSigLen;
    in >> qamSigLen;
    qamSig.clear();
    for(size_t i = 0; i < qamSigLen; i++){
        float re, im;
        in >> re >> im;
        qamSig.push_back(complex(re, im));
    }
    inp.flush();
    inp.close();
}

void MainWindow::on_demodBtn_clicked()
{
    // Make AWGN  noise
    float Es = stdDev(qamSig);
    Es *= Es;
    int SNR = 60;
    float gauss = sqrt(Es / 2 / SNR);
    for(size_t i = 0; i < qamSig.size(); i++){
        qamSig[i] = qamSig[i] + complex(randn(), randn()) * gauss;
    }
    // FFT transformation
    for(size_t i = 0; i < qamSig.size(); i += 64){
        vector<complex> tmp(qamSig.begin() + i, qamSig.begin() + i + 64);
        fft(tmp);
        for(size_t j = i; j < i + 64; j++){
            qamSig[j].setVal(tmp[j - i].getRe(), tmp[j - i].getIm());
        }
    }
    // Guard bit removal
    size_t i = 0;
    while(i < qamSig.size()){
        qamSig.erase(qamSig.begin() + i, qamSig.begin() + i + 16);
        i += 48;
    }
    // Pilot removal
    i = 0;
    while(i < qamSig.size()){
        qamSig.erase(qamSig.begin() + i, qamSig.begin() + i + 1);
        i += 4;
    }
    // QAM Demodulation
    vector<unsigned char> dat = qamDemod(qamSig);
    QFile outFile("data.txt");
    outFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&outFile);
    for(size_t i = 0; i < dat.size(); i++){
        out << (char)dat[i];
    }
    outFile.flush();
    outFile.close();
}
