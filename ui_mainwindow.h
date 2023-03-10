/********************************************************************************
** Form generated from reading UI file 'mainwindowcLqwEf.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWCLQWEF_H
#define MAINWINDOWCLQWEF_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QGraphicsView *graphicsViewROI;
    QLabel *label_2;
    QGraphicsView *graphicsViewTemp;
    QPushButton *pushButtonTemp;
    QPushButton *pushButtonROI;
    QLabel *label_7;
    QPushButton *pushButtonROI_4;
    QPushButton *pushButtonAMSS;
    QGroupBox *groupBox_2;
    QGraphicsView *graphicsViewImg;
    QLabel *label_3;
    QPushButton *pushButtonROI_2;
    QPushButton *pushButtonMatch;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QLabel *label_6;
    QLineEdit *lineEdit_2;
    QPushButton *pushButtonROI_3;
    QLabel *label_8;
    QLineEdit *lineEdit_3;
    QLineEdit *lineEdit_4;
    QLabel *label_9;
    QLabel *label_10;
    QLineEdit *lineEdit_5;
    QLabel *label_11;
    QLineEdit *lineEdit_6;
    QLineEdit *lineEdit_7;
    QLabel *label_12;
    QLabel *label_13;
    QLineEdit *lineEdit_8;
    QPushButton *pushButtonSearch;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(868, 689);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 831, 301));
        QFont font;
        font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        groupBox->setFont(font);
        groupBox->setFlat(false);
        graphicsViewROI = new QGraphicsView(groupBox);
        graphicsViewROI->setObjectName(QString::fromUtf8("graphicsViewROI"));
        graphicsViewROI->setGeometry(QRect(530, 29, 280, 261));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(480, 30, 51, 21));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(9);
        font1.setBold(false);
        font1.setWeight(50);
        label_2->setFont(font1);
        label_2->setFrameShape(QFrame::Box);
        graphicsViewTemp = new QGraphicsView(groupBox);
        graphicsViewTemp->setObjectName(QString::fromUtf8("graphicsViewTemp"));
        graphicsViewTemp->setGeometry(QRect(160, 29, 280, 261));
        pushButtonTemp = new QPushButton(groupBox);
        pushButtonTemp->setObjectName(QString::fromUtf8("pushButtonTemp"));
        pushButtonTemp->setGeometry(QRect(20, 110, 101, 31));
        pushButtonTemp->setFont(font1);
        pushButtonROI = new QPushButton(groupBox);
        pushButtonROI->setObjectName(QString::fromUtf8("pushButtonROI"));
        pushButtonROI->setGeometry(QRect(20, 160, 101, 23));
        pushButtonROI->setFont(font1);
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(100, 30, 61, 21));
        label_7->setFont(font1);
        label_7->setFrameShape(QFrame::Box);
        pushButtonROI_4 = new QPushButton(groupBox);
        pushButtonROI_4->setObjectName(QString::fromUtf8("pushButtonROI_4"));
        pushButtonROI_4->setGeometry(QRect(20, 210, 101, 23));
        pushButtonROI_4->setFont(font1);
        pushButtonAMSS = new QPushButton(groupBox);
        pushButtonAMSS->setObjectName(QString::fromUtf8("pushButtonAMSS"));
        pushButtonAMSS->setGeometry(QRect(20, 70, 101, 24));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 310, 831, 321));
        groupBox_2->setFont(font);
        groupBox_2->setFlat(false);
        graphicsViewImg = new QGraphicsView(groupBox_2);
        graphicsViewImg->setObjectName(QString::fromUtf8("graphicsViewImg"));
        graphicsViewImg->setGeometry(QRect(530, 10, 280, 280));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(470, 10, 61, 21));
        label_3->setFont(font1);
        label_3->setFrameShape(QFrame::Box);
        pushButtonROI_2 = new QPushButton(groupBox_2);
        pushButtonROI_2->setObjectName(QString::fromUtf8("pushButtonROI_2"));
        pushButtonROI_2->setGeometry(QRect(10, 30, 101, 23));
        pushButtonROI_2->setFont(font1);
        pushButtonMatch = new QPushButton(groupBox_2);
        pushButtonMatch->setObjectName(QString::fromUtf8("pushButtonMatch"));
        pushButtonMatch->setGeometry(QRect(390, 120, 101, 23));
        pushButtonMatch->setFont(font1);
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 100, 91, 21));
        label_4->setFont(font1);
        label_4->setFrameShape(QFrame::Box);
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(390, 150, 101, 21));
        label_5->setFont(font1);
        label_5->setFrameShape(QFrame::Box);
        lineEdit = new QLineEdit(groupBox_2);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(110, 100, 61, 20));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 130, 91, 21));
        label_6->setFont(font1);
        label_6->setFrameShape(QFrame::Box);
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(110, 130, 61, 20));
        pushButtonROI_3 = new QPushButton(groupBox_2);
        pushButtonROI_3->setObjectName(QString::fromUtf8("pushButtonROI_3"));
        pushButtonROI_3->setGeometry(QRect(200, 200, 151, 23));
        pushButtonROI_3->setFont(font1);
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 70, 91, 21));
        label_8->setFont(font1);
        label_8->setFrameShape(QFrame::Box);
        lineEdit_3 = new QLineEdit(groupBox_2);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(110, 70, 61, 20));
        lineEdit_4 = new QLineEdit(groupBox_2);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(300, 70, 61, 20));
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(200, 70, 91, 21));
        label_9->setFont(font1);
        label_9->setFrameShape(QFrame::Box);
        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(200, 100, 91, 21));
        label_10->setFont(font1);
        label_10->setFrameShape(QFrame::Box);
        lineEdit_5 = new QLineEdit(groupBox_2);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(300, 100, 61, 20));
        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 160, 91, 21));
        label_11->setFont(font1);
        label_11->setFrameShape(QFrame::Box);
        lineEdit_6 = new QLineEdit(groupBox_2);
        lineEdit_6->setObjectName(QString::fromUtf8("lineEdit_6"));
        lineEdit_6->setGeometry(QRect(110, 160, 61, 20));
        lineEdit_7 = new QLineEdit(groupBox_2);
        lineEdit_7->setObjectName(QString::fromUtf8("lineEdit_7"));
        lineEdit_7->setGeometry(QRect(300, 160, 61, 20));
        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(200, 160, 91, 21));
        label_12->setFont(font1);
        label_12->setFrameShape(QFrame::Box);
        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(200, 130, 91, 21));
        label_13->setFont(font1);
        label_13->setFrameShape(QFrame::Box);
        lineEdit_8 = new QLineEdit(groupBox_2);
        lineEdit_8->setObjectName(QString::fromUtf8("lineEdit_8"));
        lineEdit_8->setGeometry(QRect(300, 130, 61, 20));
        pushButtonSearch = new QPushButton(groupBox_2);
        pushButtonSearch->setObjectName(QString::fromUtf8("pushButtonSearch"));
        pushButtonSearch->setGeometry(QRect(390, 190, 80, 21));
        MainWindow->setCentralWidget(centralWidget);
        groupBox_2->raise();
        groupBox->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 868, 20));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "ROI selection", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "ROI", nullptr));
        pushButtonTemp->setText(QCoreApplication::translate("MainWindow", "Load template", nullptr));
        pushButtonROI->setText(QCoreApplication::translate("MainWindow", "Select ROI", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Template", nullptr));
        pushButtonROI_4->setText(QCoreApplication::translate("MainWindow", "Read ROI", nullptr));
        pushButtonAMSS->setText(QCoreApplication::translate("MainWindow", "AMSS", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Template matching", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Matched", nullptr));
        pushButtonROI_2->setText(QCoreApplication::translate("MainWindow", "Select Image", nullptr));
        pushButtonMatch->setText(QCoreApplication::translate("MainWindow", "Start!!!", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Block size", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Output params", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Zernike threshold", nullptr));
        pushButtonROI_3->setText(QCoreApplication::translate("MainWindow", "Modify input params", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Neighbour distance", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Candidate points", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Zernike order", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "MBNS threshold", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "No. Iterations", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Total No.group", nullptr));
        pushButtonSearch->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWCLQWEF_H
