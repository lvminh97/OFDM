#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "complex.h"
#include "fft.h"
#include "qam.h"
#include <string>
#include <vector>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QtSerialPort/QSerialPort>

QString readData;

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
    readData = in.readAll();
    inp.flush();
    inp.close();
}

void MainWindow::on_sendBtn_clicked()
{
    string str = readData.toStdString();
    vector<unsigned char> dat;
    for(size_t i = 0; i < str.size(); i++){
        dat.push_back(str[i]);
    }
    // Signal Mapper
    vector<complex> qamSig = qamMod(dat);
    // Pilot Insertion
    size_t i = 0;
    while(i < qamSig.size()){
        qamSig.insert(qamSig.begin() + i, complex(0, 1));
        i += 5;
    }
    // Guard bit Insertion
    i = 0;
    while(i < qamSig.size()){
        vector<complex> tmp(qamSig.begin() + i + 32, qamSig.begin() + i + 48);
        qamSig.insert(qamSig.begin() + i, tmp.begin(), tmp.end());
        i += 64;
    }
    // Padding
    if(qamSig.size() % 64){
        int padLen = 64 - qamSig.size() % 64;
        for(int i = 0; i < padLen; i++){
            qamSig.push_back(complex(-0.707107, 0.707107));
        }
    }
    // IFFT transformation
    for(size_t i = 0; i < qamSig.size(); i += 64){
        vector<complex> tmp(qamSig.begin() + i, qamSig.begin() + i + 64);
        ifft(tmp);
        for(size_t j = i; j < i + 64; j++){
            qamSig[j].setVal(tmp[j - i].getRe(), tmp[j - i].getIm());
        }
    }
    QString outFilename("output.txt");
    QFile outFile(outFilename);
    outFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&outFile);
    out << qamSig.size() << endl;
    for(size_t i = 0; i < qamSig.size(); i++){
        out << qamSig[i].getRe() << " " << qamSig[i].getIm() << endl;
    }
    outFile.flush();
    outFile.close();
    QSerialPort *serial(new QSerialPort(this));
    connect(serial,SIGNAL(readyRead()),this,SLOT(toReadThePort()),Qt::QueuedConnection);
}
