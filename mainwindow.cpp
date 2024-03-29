#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdebug"
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include<iostream>
#include<fstream>
#include <QFile>


QVector<int> CenterPoint_array;//Accurate template center coordinates read from txt file
Point TrueCenterPoint(0,0);
//Test Outcome Variables
float MNS_ST=0;//Sum of Time(s)
int MNS_QS=0;//Quantity of Success
float MNS_Sa=0;//Sum of accur
float MNS_SAE=0;//Sum of Angle Error

float MBNS_ST=0;//Sum of Time(s)
int MBNS_QS=0;//Quantity of Success
float MBNS_Sa=0;//Sum of accur
float MBNS_SAE=0;//Sum of Angle Error


float SAD_ST=0;//Sum of Time(s)
int SAD_QS=0;//Quantity of Success
float SAD_Sa=0;//Sum of accur

float SSD_ST=0;//Sum of Time(s)
int SSD_QS=0;//Quantity of Success
float SSD_Sa=0;//Sum of accur

float NCC_ST=0;//Sum of Time(s)
int NCC_QS=0;//Quantity of Success
float NCC_Sa=0;//Sum of accur

int inumber=0;

int flag00=0;


//模板匹配参数
//定义全局变量
//定义用于控制mat显示的类指针
//QpMatView *ROIview;
//QpMatView *Imgview;

int Ldisk=16;//局部方块边长
int R=(int)Ldisk/2;//局部方块半径
float PSMode=2;//邻居距离倍数；邻居距离=PSMode*R；
float Thresh_distance=(float)(PSMode*R);//邻居距离阈值
int MatchingNum2=5;//环投影候选点数量
int Thresh_MBNSnum=20;//MBNS邻居数量阈值
int N=20;//Zernike矩的阶数/2=N
float Rthre=0.9;//Zernike矩阈值
bool templateProcessFlag=0;//模板图处理过程的标志
int graphicsViewWidth;//图片显示框大小（正方形）
QString datasetpath="";



//遗传算法参数
int popular=0;//total group number
int Maxgen=0;//The maximum number of iterations (the maximum evolutionary generation of the population)



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsViewTemp->setScene(new QGraphicsScene(this));
    ui->graphicsViewROI->setScene(new QGraphicsScene(this));
    //绑定mat显示控件，必须在setupUi之后
    //    mview = new QpMatView(ui->graphicsViewTemp);
    //    ROIview = new QpMatView(ui->graphicsViewROI);
    //    Imgview = new QpMatView(ui->graphicsViewImg);

    //show template matching initial input parameters
    QString CharPSMode;
    CharPSMode=QString::number(PSMode);
    ui->lineEdit_3->setText(CharPSMode);

    QString CharLdisk;
    CharLdisk=QString::number(Ldisk);
    ui->lineEdit->setText(CharLdisk);

    QString CharRthre;
    CharRthre=QString::number(Rthre);
    ui->lineEdit_2->setText(CharRthre);

    QString CharThresh_MBNSnum;
    CharThresh_MBNSnum=QString::number(Thresh_MBNSnum);
    ui->lineEdit_6->setText(CharThresh_MBNSnum);

    QString CharMatchingNum2;
    CharMatchingNum2=QString::number(MatchingNum2);
    ui->lineEdit_4->setText(CharMatchingNum2);

    QString CharN;
    CharN=QString::number(N);
    ui->lineEdit_5->setText(CharN);

    QString Charpopular;
    Charpopular=QString::number(popular);
    ui->lineEdit_8->setText(Charpopular);

    QString CharMaxgen;
    CharMaxgen=QString::number(Maxgen);
    ui->lineEdit_7->setText(CharMaxgen);



    graphicsViewWidth=ui->graphicsViewImg->width();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//图像添加白噪声函数
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdlib>
#include <limits>
#include <cmath>
using namespace cv;
using namespace std;
double generateGaussianNoise(double mu, double sigma)
{
    //define a very small value
    const double epsilon = numeric_limits<double>::min();//Returns the absolute value of the difference between the closest positive number to 1 and the difference between 1 that the target data type can represent
    static double z0, z1;
    static bool flag = false;
    flag = !flag;
    //flag为假，构造高斯随机变量
    if (!flag)
        return z1*sigma + mu;
    double u1, u2;
    //Construct random variables

    do
    {
        u1 = rand()*(1.0 / RAND_MAX);
        u2 = rand()*(1.0 / RAND_MAX);
    } while (u1 <= epsilon);
    //flag为真构造高斯随机变量X
    z0 = sqrt(-2.0*log(u1))*cos(2 * CV_PI * u2);
    z1 = sqrt(-2.0*log(u1))*sin(2 * CV_PI * u2);
    return z1*sigma + mu;
}
//为图像添加高斯噪声
Mat addGaussianNoise(Mat& srcImage,uint Gtimes)
{
    Mat resultImage = srcImage.clone();    //深拷贝,克隆
    int channels = resultImage.channels();    //获取图像的通道
    int nRows = resultImage.rows;    //图像的行数

    int nCols = resultImage.cols*channels;   //图像的总列数
    //判断图像的连续性
    if (resultImage.isContinuous())    //判断矩阵是否连续，若连续，我们相当于只需要遍历一个一维数组
    {
        nCols *= nRows;
        nRows = 1;
    }
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {	//添加高斯噪声
            int val = resultImage.ptr<uchar>(i)[j] + generateGaussianNoise(2, 0.8)*Gtimes;//均值为2，方差为0.8的高斯噪声;Gtimes为高斯噪声倍数
            if (val < 0)
                val = 0;
            if (val > 255)
                val = 255;
            resultImage.ptr<uchar>(i)[j] = (uchar)val;
        }
    }
    return resultImage;
}
//为图像添加加性噪声（改变图像整体亮度）
Mat addAddNoise(Mat& srcImage,int AddValue)
{
    Mat resultImage = srcImage.clone();    //深拷贝,克隆
    int channels = resultImage.channels();    //获取图像的通道
    int nRows = resultImage.rows;    //图像的行数

    int nCols = resultImage.cols*channels;   //图像的总列数
    //判断图像的连续性
    if (resultImage.isContinuous())    //判断矩阵是否连续，若连续，我们相当于只需要遍历一个一维数组
    {
        nCols *= nRows;
        nRows = 1;
    }
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {	//添加加性噪声
            int val = resultImage.ptr<uchar>(i)[j] + AddValue;//AddValue为迭加的灰度值
            if (val < 0)
                val = 0;
            if (val > 255)
                val = 255;
            resultImage.ptr<uchar>(i)[j] = (uchar)val;
        }
    }
    return resultImage;
}
//为图像添加乘性噪声（改变图像对比度）
Mat addContrastNoise(Mat& srcImage,float MulValue)
{
    Mat resultImage = srcImage.clone();    //深拷贝,克隆
    int channels = resultImage.channels();    //获取图像的通道
    int nRows = resultImage.rows;    //图像的行数

    int nCols = resultImage.cols*channels;   //图像的总列数
    //判断图像的连续性
    if (resultImage.isContinuous())    //判断矩阵是否连续，若连续，我们相当于只需要遍历一个一维数组
    {
        nCols *= nRows;
        nRows = 1;
    }
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {	//添加加性噪声
            int val = resultImage.ptr<uchar>(i)[j] * MulValue;//MulValue为迭加的灰度值
            if (val < 0)
                val = 0;
            if (val > 255)
                val = 255;
            resultImage.ptr<uchar>(i)[j] = (uchar)val;
        }
    }
    return resultImage;
}


//Load template which to be using by mouse
Mat TemplateImg;
int beta=0;//图像显示的缩放倍数
void MainWindow::on_pushButtonTemp_clicked()
{
    //创建一张100*100的蓝色图片
    //Mat img(100,100,CV_8UC3,Scalar(255,0,0));

    //读模板图
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName.length()!=0){
        TemplateImg=imread(fileName.toStdString());
        //自适应窗口显示图像
        Mat TemplateImgDisp;
        int resizeWidth;//显示宽度
        int resizeHeight;//显示高度
        int maxLength;//图像最长边
        maxLength = (TemplateImg.cols>TemplateImg.rows)?TemplateImg.cols:TemplateImg.rows;
        //        beta=(int)maxLength/graphicsViewWidth;

        switch(beta){
        case 0:
        {
            resizeWidth=TemplateImg.cols;
            resizeHeight=TemplateImg.rows;
        }break;
        case 1:
        {
            resizeWidth=(int)TemplateImg.cols/2;
            resizeHeight=(int)TemplateImg.rows/2;
        }break;
        case 2:
        {
            resizeWidth=(int)TemplateImg.cols/3;
            resizeHeight=(int)TemplateImg.rows/3;
        }break;
        case 3:
        {
            resizeWidth=(int)TemplateImg.cols/4;
            resizeHeight=(int)TemplateImg.rows/4;
        }break;
        case 4:
        {
            resizeWidth=(int)TemplateImg.cols/5;
            resizeHeight=(int)TemplateImg.rows/5;
        }break;
        case 5:
        {
            resizeWidth=(int)TemplateImg.cols/6;
            resizeHeight=(int)TemplateImg.rows/6;
        }break;
        case 6:
        {
            resizeWidth=(int)TemplateImg.cols/7;
            resizeHeight=(int)TemplateImg.rows/7;
        }break;
        case 7:
        {
            resizeWidth=(int)TemplateImg.cols/8;
            resizeHeight=(int)TemplateImg.rows/8;
        }break;
        case 8:
        {
            resizeWidth=(int)TemplateImg.cols/9;
            resizeHeight=(int)TemplateImg.rows/9;
        }break;
        case 9:
        {
            resizeWidth=(int)TemplateImg.cols/10;
            resizeHeight=(int)TemplateImg.rows/10;
        }break;
        case 10:
        {
            resizeWidth=(int)TemplateImg.cols/11;
            resizeHeight=(int)TemplateImg.rows/11;
        }break;
        default:
        {
            resizeWidth=(int)TemplateImg.cols/12;
            resizeHeight=(int)TemplateImg.rows/12;
        }break;
        }


        //resize函数前加cv：：，因为Qt也有resize函数，故加cv：：加以区别，不然会出现错误
        cv::resize(TemplateImg,TemplateImgDisp,Size(resizeWidth,resizeHeight),0,0,3);
        //        mview->imshow(TemplateImgDisp);//显示到绑定的控件上
        QImage qimg(TemplateImgDisp.data, TemplateImgDisp.cols, TemplateImgDisp.rows, TemplateImgDisp.step, QImage::Format_BGR888);
        //        scene=new QGraphicsScene() ;
        //        QGraphicsView* view = new QGraphicsView(scene);
        //        scene->addItem(item);
        //        view->show();
        //        ui->graphicsViewTemp->setScene(scene);

        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(qimg));
        ui->graphicsViewTemp->scene()->addItem(item);
        ui->graphicsViewTemp->fitInView(item, Qt::KeepAspectRatio);
        //cv::resize(TemplateImg,TemplateImg,Size(resizeWidth,resizeHeight),0,0,3);
        //mview->imshow(TemplateImg);//显示到绑定的控件上
    }


    //透视变换



    /*
    //获取模板匹配测试图像集（添加白噪声后旋转）
    Mat gaussianImg,gaussresultImage;
    TemplateImg.copyTo(gaussianImg);
    gaussianImg.copyTo(gaussresultImage);

    //Point RotationCenter2=Point(376,240);//绕图像中心
    //Point RotationCenter2=Point(356,283);//绕60华为模板
    //Point RotationCenter2=Point(355,224);//绕120华为模板
    //Point RotationCenter2=Point(368,207);//绕200老干妈模板
    //Point RotationCenter2=Point(380,235);//绕200华为LOGO模板

    //Point RotationCenter2=Point(170,246);//绕200logo模板
    Point RotationCenter2=Point(437,303);//绕200word模板
    double RotationScale2=1;
    Mat RotationMat2(2,3,CV_32FC1);
    Mat RotationImg2;
    for(int i=0;i<360;i++)
    {

        //gaussresultImage = addGaussianNoise(gaussianImg,30);//添加高斯噪声：倍数区间1到12，每隔1取值
        //gaussresultImage = addAddNoise(gaussianImg,-60);//添加加性噪声（改变图像整体亮度）:叠加亮度区间-30到30，每隔5取值
        gaussresultImage = addContrastNoise(gaussianImg,0.2);//添加乘性噪声（改变图像对比度）：倍数区间
        RotationMat2=getRotationMatrix2D(RotationCenter2,(double)i,RotationScale2);
        warpAffine(gaussresultImage,RotationImg2,RotationMat2,TemplateImg.size());
        char str[200];
        //sprintf(str,"F:/Project/0QT/QtCode/image_Match/huawei/%d.bmp",i);
        //sprintf(str,"F:/Project/0QT/QtCode/image_Match/laoganma/%d.bmp",i);
        //添加白噪声的文件夹
        //sprintf(str,"F:/Project/0QT/QtCode/image_Match/60Gausshuawei/%d.bmp",i);
        //sprintf(str,"F:/Project/0QT/QtCode/image_Match/200Gausslaoganma/%d.bmp",i);
        sprintf(str,"F:/Project/0QT/QtCode/image_Match/temp/%d.bmp",i);
        imwrite(str,RotationImg2);
    }

*/
    /*
    //求测试图旋转后，模板图的精确坐标
    Point ImgCenter,OriginTemplateCenter60,OriginTemplateCenter120,OriginTemplateCenter200;//中心坐标
    ImgCenter.x=376;
    ImgCenter.y=240;
    OriginTemplateCenter60.x=326;
    OriginTemplateCenter60.y=253;
    OriginTemplateCenter120.x=295;
    OriginTemplateCenter120.y=164;
    OriginTemplateCenter200.x=268;
    OriginTemplateCenter200.y=107;
    Point TemplateCenter60,TemplateCenter120,TemplateCenter200;//模板中心相对测试图的坐标
    TemplateCenter60.x=OriginTemplateCenter60.x-ImgCenter.x;
    TemplateCenter60.y=OriginTemplateCenter60.y-ImgCenter.y;
    TemplateCenter120.x=OriginTemplateCenter120.x-ImgCenter.x;
    TemplateCenter120.y=OriginTemplateCenter120.y-ImgCenter.y;
    TemplateCenter200.x=OriginTemplateCenter200.x-ImgCenter.x;
    TemplateCenter200.y=OriginTemplateCenter200.y-ImgCenter.y;

    Point tempCoor;
    int tempcoorX,tempcoorY;
    FILE* fp;
    fp=fopen("xycoordinate60.txt","w");
    for(int i=0;i<360;i++)
    {
        tempcoorX=(int)((float)TemplateCenter60.x*cos(i)-(float)TemplateCenter60.y*sin(i)+0.5);
        tempcoorY=(int)((float)TemplateCenter60.x*sin(i)+(float)TemplateCenter60.y*cos(i)+0.5);
        tempCoor.x=ImgCenter.x+tempcoorX;
        tempCoor.y=ImgCenter.y+tempcoorY;
        fprintf(fp,"(%d,%d)\n",tempCoor.x,tempCoor.y);

    }
    fclose(fp);
    */

}

//Mouse to draw a rectangle to select ROI
Mat org,ROI,img,tmp;
void on_mouse(int event,int x,int y,int flags,void *ustc)//event mouse event code, x, y mouse coordinates, flags drag and keyboard operation code
{
    static Point pre_pt = Point(-1,-1);//initial coordinates
    static Point cur_pt = Point(-1,-1);//real-time coordinates
    char temp[16];
    if (event == CV_EVENT_LBUTTONDOWN)//Press the left button, read the initial coordinates, and draw a circle at this point on the image
    {
        org.copyTo(img);//Copy the original image into img
        sprintf(temp,"(%d,%d)",x,y);
        pre_pt = Point(x,y);
        putText(img,temp,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,255));//Display coordinates on the window
        circle(img,pre_pt,2,Scalar(255,0,0),CV_FILLED,CV_AA,0);//draw a circle
        imshow("TemplateImg",img);
    }
    else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//The handler for moving the mouse when the left button is not pressed
    {
        img.copyTo(tmp);//Copy img to temporary image tmp for displaying live coordinates
        sprintf(temp,"(%d,%d)",x,y);
        cur_pt = Point(x,y);
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,255));//Just display the coordinates of the mouse movement in real time
        imshow("TemplateImg",tmp);
    }
    else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//When the left button is pressed and the mouse moves, a rectangle is drawn on the image
    {
        img.copyTo(tmp);
        sprintf(temp,"(%d,%d)",x,y);
        cur_pt = Point(x,y);
        putText(tmp,temp,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,255));
        rectangle(tmp,pre_pt,cur_pt,Scalar(0,255,0),1,8,0);//Real-time display of rectangles formed when the mouse is dragged on a temporary image
        imshow("TemplateImg",tmp);
    }
    else if (event == CV_EVENT_LBUTTONUP)//Release the left button, a rectangle will be drawn on the image
    {
        org.copyTo(img);
        //        sprintf(temp,"(%d,%d)[%d x %d]",x,y,cur_pt.x-pre_pt.x,cur_pt.y-pre_pt.y);
        cur_pt = Point(x,y);
        putText(img,temp,Point(10,15),FONT_HERSHEY_SIMPLEX,0.4,Scalar(0,0,255));
        qDebug("ROI width and height = [%d x %d]",cur_pt.x-pre_pt.x,cur_pt.y-pre_pt.y);
        circle(img,pre_pt,2,Scalar(255,0,0),CV_FILLED,CV_AA,0);
        rectangle(img,pre_pt,cur_pt,Scalar(0,255,0),1,8,0);//According to the initial point and the end point, draw the rectangle on the img
        imshow("TemplateImg",img);
        img.copyTo(tmp);
        //Capture the image enclosed by the rectangle and save it to dst
        int width = abs(pre_pt.x - cur_pt.x);
        int height = abs(pre_pt.y - cur_pt.y);
        if (width == 0 || height == 0)
        {
            printf("width == 0 || height == 0");
            return;
        }
        ROI = org(Rect(min(cur_pt.x,pre_pt.x),min(cur_pt.y,pre_pt.y),width,height));
        //Adaptive window display image
        Mat ROIDisp;
        int resizeWidth;//display width
        int resizeHeight;//display height
        //        beta = 3;
        switch(beta){
        case 0:
        {
            resizeWidth=ROI.cols;
            resizeHeight=ROI.rows;
        }break;
        case 1:
        {
            resizeWidth=(int)ROI.cols/2;
            resizeHeight=(int)ROI.rows/2;
        }break;
        case 2:
        {
            resizeWidth=(int)ROI.cols/3;
            resizeHeight=(int)ROI.rows/3;
        }break;
        case 3:
        {
            resizeWidth=(int)ROI.cols/4;
            resizeHeight=(int)ROI.rows/4;
        }break;
        case 4:
        {
            resizeWidth=(int)ROI.cols/5;
            resizeHeight=(int)ROI.rows/5;
        }break;
        case 5:
        {
            resizeWidth=(int)ROI.cols/6;
            resizeHeight=(int)ROI.rows/6;
        }break;
        case 6:
        {
            resizeWidth=(int)ROI.cols/7;
            resizeHeight=(int)ROI.rows/7;
        }break;
        case 7:
        {
            resizeWidth=(int)ROI.cols/8;
            resizeHeight=(int)ROI.rows/8;
        }break;
        case 8:
        {
            resizeWidth=(int)ROI.cols/9;
            resizeHeight=(int)ROI.rows/9;
        }break;
        case 9:
        {
            resizeWidth=(int)ROI.cols/10;
            resizeHeight=(int)ROI.rows/10;
        }break;
        case 10:
        {
            resizeWidth=(int)ROI.cols/11;
            resizeHeight=(int)ROI.rows/11;
        }break;
        default:
        {
            resizeWidth=(int)ROI.cols/12;
            resizeHeight=(int)ROI.rows/12;
        }break;
        }

        //Add cv:: before the resize function, because Qt also has a resize function, so add cv:: to distinguish, otherwise an error will occur
        cv::resize(ROI,ROIDisp,Size(resizeWidth,resizeHeight),0,0,3);
        cv::resize(ROI,ROI,Size(resizeWidth,resizeHeight),0,0,3);
        //        ROIview->imshow(ROIDisp);//显示到绑定的控件上
        //        cv::imshow("Cropped ROI",ROIDisp);
        Ldisk=(int)sqrt(ROI.cols*ROI.rows/26);
        R=(int)Ldisk/2;//局部方块半径
        qDebug("Ldisk=%d",Ldisk);

        templateProcessFlag=!templateProcessFlag;
    }
}
//Mouse to draw a rectangle to select ROI
void MainWindow::on_pushButtonROI_clicked()
{
    TemplateImg.copyTo(org);
    org.copyTo(img);
    org.copyTo(tmp);
    namedWindow("TemplateImg");//define an img window
    setMouseCallback("TemplateImg",on_mouse,0);//Call callback function
    QImage qimgROI(ROI.data, ROI.cols, ROI.rows, ROI.step, QImage::Format_BGR888);
    //        scene=new QGraphicsScene() ;
    //        QGraphicsView* view = new QGraphicsView(scene);
    //        scene->addItem(item);
    //        view->show();
    //        ui->graphicsViewTemp->setScene(scene);

    QGraphicsPixmapItem* itemRoi = new QGraphicsPixmapItem(QPixmap::fromImage(qimgROI));
    ui->graphicsViewROI->scene()->addItem(itemRoi);
    ui->graphicsViewROI->fitInView(itemRoi, Qt::KeepAspectRatio);
}


//Load ROI Directly
void MainWindow::on_pushButtonROI_4_clicked()
{
    //接读取ROI
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));
    //    qDebug(fileName.toStdString());
    if(fileName.length()!=0){
        ROI=imread(fileName.toStdString());
        //自适应窗口显示图像
        Mat ROIDisp;
        int resizeWidth;//显示宽度
        int resizeHeight;//显示高度

        switch(beta){
        case 0:
        {
            resizeWidth=ROI.cols;
            resizeHeight=ROI.rows;
        }break;
        case 1:
        {
            resizeWidth=(int)ROI.cols/2;
            resizeHeight=(int)ROI.rows/2;
        }break;
        case 2:
        {
            resizeWidth=(int)ROI.cols/3;
            resizeHeight=(int)ROI.rows/3;
        }break;
        case 3:
        {
            resizeWidth=(int)ROI.cols/4;
            resizeHeight=(int)ROI.rows/4;
        }break;
        case 4:
        {
            resizeWidth=(int)ROI.cols/5;
            resizeHeight=(int)ROI.rows/5;
        }break;
        case 5:
        {
            resizeWidth=(int)ROI.cols/6;
            resizeHeight=(int)ROI.rows/6;
        }break;
        case 6:
        {
            resizeWidth=(int)ROI.cols/7;
            resizeHeight=(int)ROI.rows/7;
        }break;
        case 7:
        {
            resizeWidth=(int)ROI.cols/8;
            resizeHeight=(int)ROI.rows/8;
        }break;
        case 8:
        {
            resizeWidth=(int)ROI.cols/9;
            resizeHeight=(int)ROI.rows/9;
        }break;
        case 9:
        {
            resizeWidth=(int)ROI.cols/10;
            resizeHeight=(int)ROI.rows/10;
        }break;
        case 10:
        {
            resizeWidth=(int)ROI.cols/11;
            resizeHeight=(int)ROI.rows/11;
        }break;
        default:
        {
            resizeWidth=(int)ROI.cols/12;
            resizeHeight=(int)ROI.rows/12;
        }break;
        }

        //resize函数前加cv：：，因为Qt也有resize函数，故加cv：：加以区别，不然会出现错误
        cv::resize(ROI,ROIDisp,Size(resizeWidth,resizeHeight),0,0,3);
        //        ROIview->imshow(ROIDisp);//显示到绑定的控件上
        QImage qimg(ROIDisp.data, ROIDisp.cols, ROIDisp.rows, ROIDisp.step, QImage::Format_BGR888);
        //        scene=new QGraphicsScene() ;
        //        QGraphicsView* view = new QGraphicsView(scene);
        //        scene->addItem(item);
        //        view->show();
        //        ui->graphicsViewTemp->setScene(scene);

        QGraphicsPixmapItem* itemROI = new QGraphicsPixmapItem(QPixmap::fromImage(qimg));
        ui->graphicsViewROI->scene()->addItem(itemROI);
        ui->graphicsViewROI->fitInView(itemROI, Qt::KeepAspectRatio);

        Ldisk=(int)sqrt(ROI.cols*ROI.rows/26);
        R=(int)Ldisk/2;//局部方块半径
        qDebug("Ldisk=%d",Ldisk);

        templateProcessFlag=!templateProcessFlag;
        //        ui->graphicsViewROI->scene()->removeItem(itemROI);
    }
}


//Load image
Mat Image;
void MainWindow::on_pushButtonROI_2_clicked()
{

    //    //读取txt
    //    QString fileName1 = QFileDialog::getOpenFileName(NULL,tr("Open txt"),".",tr("Txt Files (*.txt)"));
    //    if(fileName1.length()!=0){
    //        QFile file(fileName1);

    //        if(file.open(QIODevice::ReadOnly))
    //        {
    //             QTextStream stream(&file);
    //             while(!file.atEnd())
    //             {
    //                 int buf;
    //                 QStringList list=stream.readAll().split(",");
    //                 QListIterator<QString> li(list);
    //                 while(li.hasNext())
    //                 {
    //                    buf=li.next().toInt();
    //                    //qDebug("%d",buf);
    //                    CenterPoint_array.append(buf);
    //                 }
    //             }
    //         }
    //    }



    QString CharLdisk;
    CharLdisk=QString::number(Ldisk);
    ui->lineEdit->setText(CharLdisk);
    //读待匹配图
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open Image"),".",tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName.length()!=0){
        Image=imread(fileName.toStdString());
        //自适应窗口显示图像
        Mat ImageDisp;
        int resizeWidth;//显示宽度
        int resizeHeight;//显示高度
        //        beta = 3;
        switch(beta){
        case 0:
        {
            resizeWidth=Image.cols;
            resizeHeight=Image.rows;
        }break;
        case 1:
        {
            resizeWidth=(int)Image.cols/2;
            resizeHeight=(int)Image.rows/2;
        }break;
        case 2:
        {
            resizeWidth=(int)Image.cols/3;
            resizeHeight=(int)Image.rows/3;
        }break;
        case 3:
        {
            resizeWidth=(int)Image.cols/4;
            resizeHeight=(int)Image.rows/4;
        }break;
        case 4:
        {
            resizeWidth=(int)Image.cols/5;
            resizeHeight=(int)Image.rows/5;
        }break;
        case 5:
        {
            resizeWidth=(int)Image.cols/6;
            resizeHeight=(int)Image.rows/6;
        }break;
        case 6:
        {
            resizeWidth=(int)Image.cols/7;
            resizeHeight=(int)Image.rows/7;
        }break;
        case 7:
        {
            resizeWidth=(int)Image.cols/8;
            resizeHeight=(int)Image.rows/8;
        }break;
        case 8:
        {
            resizeWidth=(int)Image.cols/9;
            resizeHeight=(int)Image.rows/9;
        }break;
        case 9:
        {
            resizeWidth=(int)Image.cols/10;
            resizeHeight=(int)Image.rows/10;
        }break;
        case 10:
        {
            resizeWidth=(int)Image.cols/11;
            resizeHeight=(int)Image.rows/11;
        }break;
        default:
        {
            resizeWidth=(int)Image.cols/12;
            resizeHeight=(int)Image.rows/12;
        }break;
        }


        //resize函数前加cv：：，因为Qt也有resize函数，故加cv：：加以区别，不然会出现错误
        cv::resize(Image,ImageDisp,Size(resizeWidth,resizeHeight),0,0,3);
        cv::resize(Image,Image,Size(resizeWidth,resizeHeight),0,0,3);
        //        Imgview->imshow(ImageDisp);//显示到绑定的控件上
        //cv::resize(Image,Image,Size(resizeWidth,resizeHeight),0,0,3);
        //Imgview->imshow(Image);//显示到绑定的控件上




        // for(int i=0;i<20;i++)
        //  qDebug("CenterPoint_array%d=%d",i,CenterPoint_array.at(i));


    }
}



//************模板匹配相关函数**************
//计算两点距离
float get_distance(Point Point1, Point Point2)
{
    float distance=0;
    distance=sqrt((Point1.x-Point2.x)*(Point1.x-Point2.x)+(Point1.y-Point2.y)*(Point1.y-Point2.y));

    return distance;

}


//Calculate the ring projection vector
void Anvector(Mat DiskImage,vector<uint> Antable,int xnum,vector<uint> aNumber,vector<float> &P)
{
    int i,j;
    int Length = DiskImage.rows;
    //计算圆投影向量P[R+1]
    uchar* Imagedata;
    //    uint S[xnum]={0};//同个环上像素的和
    vector<uint> S(xnum);
    //求和
    for( i = 0; i < Length; i++)
    {
        Imagedata = DiskImage.ptr<uchar>(i);//获取行指针
        for ( j = 0; j < Length; j++)
        {
            if(Antable[i*Length+j]<xnum)
            {
                S[Antable[i*Length+j]] = S[Antable[i*Length+j]] + Imagedata[j];
            }
        }
    }
    //求平均，得到圆投影向量
    for(i=0;i<xnum;i++)
    {
        P[i] = (float)S[i]/aNumber[i];
    }
}

//阶乘函数
int Multip(int a)
{
    int b=1;
    for(int i=1;i<a+1;i++)
    {
        b=b*i;
    }
    if(a==0)
        return 1;
    else
        return b;
}


//Calculate Zernike moments
//int N: order
void Zvector(Mat Img,int minLength,int N,uint* Rtable,uint* Atable,double* ZRtable,double* Z)
{
    int R=(int)minLength/2;
    int num_N=0;
    int n,m;
    int i,j;
    for(n=0;n<20;n++)
    {
        for(m=0;m<20;m++)
        {
            if((n>=m) && ((n-m)%2==0))
            {
                uchar* Imgdata;
                double Znms=0;//实部
                double Znmx=0;//虚部
                for(i=0;i<minLength;i++)
                {
                    Imgdata=Img.ptr<uchar>(i);
                    for(j=0;j<minLength;j++)
                    {
                        int r=Rtable[i*minLength+j];
                        float sita=(float)Atable[i*minLength+j]/180*3.14159;
                        if(r<R+1)
                        {
                            double Vnms=0;//实部
                            double Vnmx=0;//虚部

                            Vnms=ZRtable[num_N*(R+1)+r]*cos(m*sita);
                            Vnmx=-ZRtable[num_N*(R+1)+r]*sin(m*sita);

                            Znms=Znms+Imgdata[j]*Vnms;
                            Znmx=Znmx+Imgdata[j]*Vnmx;

                        }
                    }
                }

                Znms=Znms*(n+1)/3.14159;
                Znmx=Znmx*(n+1)/3.14159;
                Z[num_N]=sqrt(Znms*Znms+Znmx*Znmx);

                num_N++;
            }

            if(num_N==N)
                break;
        }
        if(num_N==N)
            break;
    }

}

//Draw an angled rotated rectangle
void DrawRotationRect(Mat img,Point start,int Width,int Height,double angle)
{
    int thickness=2;
    int lineType=8;
    double cosAngle=(double)cos(angle/180*3.14159);
    double sinAngle=(double)sin(angle/180*3.14159);
    Point upR;
    Point upL;
    Point downR;
    Point downL;
    upR.x=(int)(Width*cosAngle+start.x);
    upR.y=(int)(-Width*sinAngle+start.y);

    upL.x=start.x;
    upL.y=start.y;

    downR.x=(int)(Width*cosAngle+Height*sinAngle+start.x);
    downR.y=(int)(-Width*sinAngle+Height*cosAngle+start.y);

    downL.x=(int)(Height*sinAngle+start.x);
    downL.y=(int)(Height*cosAngle+start.y);

    line(img,upL,upR,Scalar(255,255,255),thickness,lineType);
    line(img,upL,downL,Scalar(255,255,255),thickness,lineType);
    line(img,downL,downR,Scalar(255,255,255),thickness,lineType);
    line(img,upR,downR,Scalar(255,255,255),thickness,lineType);

}

//数组求平均
float mean_float(float* array,int length)
{
    int i;
    float m=0;
    for(i=0;i<length;i++)
    {
        m=m+array[i];
    }
    m=m/length;

    return(m);
}


//圆投影向量平方差匹配公式查找表
//输入：
//float* P:圆投影向量
//int R：圆投影向量半径
//uint* rNumber：半径r对应的像素数
//输出：
//float* Stable：圆投影向量平方差匹配公式查找表
void Ring_Stable(float* P,float Paver,int R,Mat* Stable)
{
    int i,j,r;

    float PP_Normalize=0;//归一化互相关系数
    for(r=0;r<R+1;r++)
    {
        PP_Normalize=PP_Normalize+(P[r]-Paver)*(P[r]-Paver);
    }
    PP_Normalize=sqrt((double)PP_Normalize);
    if(PP_Normalize==0)
        PP_Normalize=1;

    //Mat tempMat(256,256,CV_32FC1,Scalar(0));
    float* buffer;
    for(r=0;r<R+1;r++)
    {
        //tempMat.copyTo(Stable[r]);
        Stable[r]=Mat::zeros(Size(256,256),CV_32FC1);
        for(i=0;i<256;i++)
        {
            buffer=Stable[r].ptr<float>(i);
            for(j=0;j<256;j++)
            {

                float TT_Normalize=0;//归一化互相关系数
                for(int rr=0;rr<R+1;rr++)
                {
                    TT_Normalize=TT_Normalize+(i-j)*(i-j);
                }
                TT_Normalize=sqrt((double)TT_Normalize);
                if(TT_Normalize==0)
                    TT_Normalize=1;


                buffer[j]=buffer[j]+(float)((i-j)-(P[r]-Paver))*((i-j)-(P[r]-Paver))/PP_Normalize/TT_Normalize*2*3.14159*r;

            }
        }
    }
}


//极坐标角度计算0°-360°
float AnglePolar(Point A)
{
    float Angle;//旋转角
    int x=A.x;
    int y=-A.y;
    float YdivX=(float)y/x;//斜率
    if(x>=0 && y==0)
    {
        Angle=0;
    }
    else if(x<0 && y==0)
    {
        Angle=180;
    }
    else if(x==0 && y>=0)
    {
        Angle=90;
    }
    else if(x==0 && y<0)
    {
        Angle=270;
    }
    else if(x>0 && y>0)
    {
        Angle=(int)(atan(YdivX)*180/3.14159);
    }
    else if(x<0 && y>0)
    {
        Angle=(int)(-atan(-YdivX)*180/3.14159)+180;
    }
    else if(x<0 && y<0)
    {
        Angle=(int)(atan(YdivX)*180/3.14159)+180;
    }
    else if(x>0 && y<0)
    {
        Angle=(int)(-atan(-YdivX)*180/3.14159)+360;
    }

    return Angle;
}

//计算邻居数量
//返回值：PointNum为邻居数量
//输入值：
//int Sort,单个图像方块的编号
//Point* Points, 所有的图像方块编号
//int sizeofPoints, Points的大小
//vector<Point> DisksCenterPoints, 所有的图像方块中心点坐标
//float ThreshDis, 判定为邻居的距离阈值
//输出值：
//float* SumDistance, 距离偏差总和
int CountNeighbors(int Sort, Point* Points, int sizeofPoints, vector<Point> DisksCenterPoints, float ThreshDis)
{
    int PointNum=0;
    float distance=0;

    if(Points[Sort].x==0 && Points[Sort].y==0)
    {
        PointNum=0;
    }
    else
    {
        for(int i=0;i<sizeofPoints;i++)
        {
            if(i!=Sort)
            {
                if(Points[i].x==0 && Points[i].y==0)
                {
                    distance=999999999;
                }
                else
                {
                    distance=get_distance(Points[Sort],Points[i]);
                    float distemp=get_distance(DisksCenterPoints[Sort],DisksCenterPoints[i]);
                    distance=fabs(distance-distemp);//距离
                }

                if(distance<ThreshDis)
                {
                    PointNum++;
                }
            }
        }
    }

    return PointNum;
}


//**********************************************************

//Start template matching
//Mat CV_resultImage;
//int MatchMethod=1;
Point RectUplOutPoint_MBNS;
void MBNS_tMatching(Mat Image,Mat TemplateImage){    

    int i,j;
    //1. The gray diagram of the image and the template is turned into a single channel
    Mat GrayImage;
    //    int resizeImageW = (int)Image.cols/1;
    //    int resizeImageH = (int)Image.rows/1;

    cvtColor(Image,GrayImage,CV_RGB2GRAY,1);
    //    cv::resize(GrayImage,GrayImage,Size(resizeImageW,resizeImageH),0,0,3);
    //    imshow("image",GrayImage);
    Mat GrayTemplate;
    //    int resizeTemplateW = (int)TemplateImage.cols/1;
    //    int resizeTemplateH = (int)TemplateImage.rows/1;

    cvtColor(TemplateImage,GrayTemplate,CV_RGB2GRAY,1);
    //    cv::resize(GrayTemplate,GrayTemplate,Size(resizeTemplateW,resizeTemplateH),0,0,3);
    //    imshow("template",GrayTemplate);
    qDebug("The width and height of the image to be matched=[%d x %d]; the width and height of the template image=[%d x %d]",GrayImage.cols,GrayImage.rows,GrayTemplate.cols,GrayTemplate.rows);

    //二、将模板图分成多个Ldisk*Ldisk的小方块，每个方块圆心相距R
    R=(int)Ldisk/2;//局部方块半径
    int WidthNum=(int)GrayTemplate.cols/Ldisk*2 - 1;//一行的方块数
    int HeightNum=(int)GrayTemplate.rows/Ldisk*2 - 1;//一列的方块数
    qDebug("Template block side length=%d",Ldisk);
    qDebug("Number of template blocks=%d",WidthNum*HeightNum);

    //四（1）、建立环投影的环查找表
    //    int x[R]={0};//环距离
    //    int x[R];
    vector<int> x(R);
    //    memset( x, 0, R*sizeof(int) );
    x[0]=1;
    int xnum=1;
    for(i=0;i<R;i++)
    {
        x[i+1]=(int)(R-sqrt((R-x[i])*(R-x[i])-(2*R-1))+0.5);
        xnum++;
        if(x[i+1]<=x[i])
        {
            x[i+1]=0;
            xnum--;
            break;
        }
    }

    //    uint AnnulusTable[Ldisk*Ldisk];//Ring search table
    vector<uint> AnnulusTable(Ldisk*Ldisk);
    vector<uint> anNumber(xnum);//Statistics the number of pixels in the same ring
    //    memset( anNumber, 0, xnum*sizeof(uint) );
    vector<uint> Rtable(Ldisk*Ldisk);//Radius search table
    vector<uint> rNumber(R+1);//Count the number of the same radius value
    //    memset( rNumber, 0, R*sizeof(uint) );
    vector<uint> aNumber(360);//Count the number of the same angle value
    vector<uint> Atable(Ldisk*Ldisk);//Angle Lookup Table
    double YdivX;//Obligation
    int A=0;//angle
    int r;//radius

    for(i=0;i<Ldisk;i++)
    {
        for(j=0;j<Ldisk;j++)
        {
            r=(int)(sqrt((i-R)*(i-R)+(j-R)*(j-R)) + 0.5);

            //Create pole coordinates (radius and angle) search table
            //Angle conversion
            if(j>R-1 && i==R)
            {
                A=0;
            }
            else if(j<R && i==R)
            {
                A=180;
            }
            else if(j==R && i<R)
            {
                A=90;
            }
            else if(j==R && i>R)
            {
                A=270;
            }
            else if(j>R && i<R)
            {
                YdivX=(double)(R-i)/(j-R);
                A=(int)(atan(YdivX)*180/3.14159);
            }
            else if(j<R && i<R)
            {
                YdivX=(double)(R-j)/(R-i);
                A=(int)(atan(YdivX)*180/3.14159)+90;
            }
            else if(j<R && i>R)
            {
                YdivX=(double)(i-R)/(R-j);
                A=(int)(atan(YdivX)*180/3.14159)+180;
            }
            else if(j>R && i>R)
            {
                YdivX=(double)(j-R)/(i-R);
                A=(int)(atan(YdivX)*180/3.14159)+270;
            }


            if(r>R)
            {
                Rtable[i*Ldisk+j] = R+1;
                Atable[i*Ldisk+j] = 361;
            }
            else
            {
                Rtable[i*Ldisk+j] = r;
                Atable[i*Ldisk+j] = A;
                rNumber[r]++;
                aNumber[A]++;
            }

            // **************************
            // **************************
            //Create a ring search table
            if(r>R)
            {
                AnnulusTable[i*Ldisk+j] = xnum;
            }
            else
            {
                for(int ii=0;ii<xnum;ii++)
                {
                    if(r>(R-x[ii]))
                    {
                        AnnulusTable[i*Ldisk+j] = ii;
                        anNumber[ii]++;
                        break;
                    }
                }

                if(r<=(R-x[xnum-1]))
                {
                    AnnulusTable[i*Ldisk+j] = xnum-1;
                    anNumber[xnum-1]++;
                }
            }

        }
    }

    //Fourth (2), the ring projection vector of the calculation template diagram
    vector<Mat> Disks;//Block
    vector<Point> DisksCenterPoints;//Central coordinates of each block
    int DiskNum=0;//Real quantity
    for(i=0;i<WidthNum*HeightNum;i++)
    {
        Mat GrayTemplatetemp(GrayTemplate(Rect(((int)i%WidthNum)*R,((int)i/WidthNum)*R,Ldisk,Ldisk)));
        vector<float> Ptemp(xnum);
        //        memset( Ptemp, 0, xnum*sizeof(float) );

        Anvector(GrayTemplatetemp,AnnulusTable,xnum,anNumber,Ptemp);
        float PtempAver=0;
        for(j=0;j<xnum;j++)
        {
            PtempAver=PtempAver+Ptemp[j];
        }
        PtempAver=PtempAver/xnum;
        float P_SSD=0;
        for(j=0;j<xnum;j++)
        {
            P_SSD=P_SSD+(Ptemp[j]-PtempAver)*(Ptemp[j]-PtempAver);
        }

        if(P_SSD>4*xnum)
        {
            Disks.push_back(GrayTemplate(Rect(((int)i%WidthNum)*R,((int)i/WidthNum)*R,Ldisk,Ldisk)));
            DisksCenterPoints.push_back(Point(((int)i%WidthNum)*R+R,((int)i/WidthNum)*R+R));
            DiskNum++;
        }

    }
    qDebug("Actual number of blocks = %d",DiskNum);

    //Ring project vector
    vector<vector<float>> P(DiskNum,vector<float>(xnum));//Template Graph Ring Projection Vector
    //    memset(P, 0, DiskNum*xnum*sizeof(float));
    for(i=0;i<DiskNum;i++)
    {
        //Ring project vector
        Anvector(Disks[i],AnnulusTable,xnum,anNumber,P[i]);

    }

    //*****五、开始匹配************************************
    //*******************************************
    double timeSum;//Total time consumption
    timeSum=static_cast<double>(getTickCount());
    //***********************************************************
    double timeAnnulus;
    timeAnnulus=static_cast<double>(getTickCount());
    //Set up a circular search scope
    int LeftUpx=0;
    int LeftUpy=0;
    int RrightDownx=(int)(GrayImage.cols-Ldisk-1);
    int RrightDowny=(int)(GrayImage.rows-Ldisk-1);

    //Get and mark the position of the candidate point
    double RminValue=0;
    Point RminLoc;
    double RmaxValue=0;
    Point RmaxLoc;
    Mat GrayImageDisp;
    GrayImage.copyTo(GrayImageDisp);

    int CandidateNum=MatchingNum2;
    vector<Point>* Points = new vector<Point>[DiskNum];//Accurate matching point coordinate array
    vector<Mat> MatchingValue;//Cirin project similarity value
    for(int ii=0;ii<DiskNum;ii++)
    {
        Mat MatchingValueTemp(GrayImage.rows-Ldisk,GrayImage.cols-Ldisk,CV_32FC1,Scalar(999999999));//Float single channel, initialization of each point
        MatchingValue.push_back(MatchingValueTemp);
    }
    for(i=LeftUpy;i<RrightDowny+1;i++)
    {

        for(j=LeftUpx;j<RrightDownx;j++)
        {

            Mat ScanROI;//Sub -map to be matched
            ScanROI=GrayImage(Rect(j,i,Ldisk,Ldisk));
            vector<float> T(xnum);//模板图环投影向量
            //            memset( T, 0, xnum*sizeof(float) );

            Anvector(ScanROI,AnnulusTable,xnum,anNumber,T);
            float T_Aver=0;
            for(int it=0;it<xnum;it++)
            {
                T_Aver=T_Aver+T[it];
            }
            T_Aver=T_Aver/xnum;
            float T_SSD=0;
            for(int it=0;it<xnum;it++)
            {
                T_SSD=T_SSD+(T[it]-T_Aver)*(T[it]-T_Aver);
            }

            if(T_SSD>4*xnum)
            {
                for(int ii=0;ii<DiskNum;ii++)
                {
                    float* buffer;
                    buffer=MatchingValue[ii].ptr<float>(i);
                    //similarity calculation
                    int tempbuffer=0;
                    for(int k=0;k<xnum;k++)
                    {
                        tempbuffer=tempbuffer+(P[ii][k]-T[k])*(P[ii][k]-T[k]);

                    }
                    buffer[j]=tempbuffer;

                }
            }
        }
    }

    for(int ii=0;ii<DiskNum;ii++)
    {
        for(i=0;i<CandidateNum;i++)
        {
            //1. Select the candidate matching point
            minMaxLoc(MatchingValue[ii],&RminValue,&RmaxValue,&RminLoc,&RmaxLoc);
            Point BestMatch=RminLoc;
            Point Centerpoint=Point(BestMatch.x+R,BestMatch.y+R);

            //Draw a circle
            circle(GrayImageDisp,Centerpoint,2,CV_RGB(255,255,255),2,8,0);
            Points[ii].push_back(Centerpoint);

            float* buffervalue=MatchingValue[ii].ptr<float>(BestMatch.y);
            buffervalue[BestMatch.x]=999999999;
        }
    }
    timeAnnulus=((double)getTickCount() - timeAnnulus)/getTickFrequency();//operation hours
    qDebug("Ring projection matching time: %f seconds",timeAnnulus);


    double timeMBNS;//MBNS耗时
    timeMBNS=static_cast<double>(getTickCount());
    //***************************************************
    //Neighbors Constraint Similarity确定最终匹配点
    //***************************************************
    Point* BestPoints = new Point[DiskNum];//最佳匹配点
    vector<int> BestPointNum(DiskNum);//最佳匹配点邻居数量
    //    memset( BestPointNum, 0, DiskNum*sizeof(int) );

    Thresh_distance=(float)(PSMode*R);//邻居距离阈值
    qDebug("Neighbor Distance Threshold MBNS: %f",Thresh_distance);
    Mat ImageFinalResult;//显示结果
    GrayImage.copyTo(ImageFinalResult);

    for(int ii=0;ii<DiskNum;ii++)
    {

        for(i=0;i<Points[ii].size();i++)
        {

            float tempMinDis=999999999;

            //Number of neighbors
            int PointNum=0;
            Point Pointtemp(0,0);
            Point* Pointss = new Point[DiskNum];
            for(j=0;j<DiskNum;j++)
            {
                Pointss[j]=Point(0,0);
                if(j==ii)   Pointss[j]=Points[ii][i];
            }

            for(int jj=0;jj<DiskNum;jj++)
            {
                //Get neighbor coordinates
                for(j=0;j<Points[jj].size();j++)
                {
                    if(jj!=ii)
                    {
                        float distance=get_distance(Points[ii][i],Points[jj][j]);
                        float distemp=get_distance(DisksCenterPoints[ii],DisksCenterPoints[jj]);
                        distance=fabs(distance-distemp);//距离
                        if(distance<tempMinDis)
                        {
                            tempMinDis=distance;
                            Pointtemp=Points[jj][j];
                        }
                    }

                }
                if(tempMinDis<Thresh_distance)
                {
                    Pointss[jj]=Pointtemp;
                    PointNum++;//Number of neighbors
                }
                tempMinDis=999999999;

            }
            //Threshold: The number of neighbors with the largest number is not less than Thresh_MBNSnum
            if(PointNum>Thresh_MBNSnum)
            {
                PointNum=0;
                //Calculate the number of second most neighbors
                int PointNumtemp=0;
                for(int tt=0;tt<DiskNum;tt++)
                {
                    if(tt!=ii)
                    {
                        PointNumtemp=CountNeighbors(tt, Pointss, DiskNum,DisksCenterPoints, Thresh_distance);
                        if(PointNum<PointNumtemp)
                        {
                            PointNum=PointNumtemp;//Second largest number of neighbors
                        }
                    }
                }

                //Get the best point coordinates
                if(PointNum>BestPointNum[ii])
                {
                    BestPoints[ii]=Points[ii][i];
                    BestPointNum[ii]=PointNum;
                }

            }

        }
        if(BestPointNum[ii]<Thresh_MBNSnum)  BestPoints[ii]=Point(0,0);//Threshold: The number of second most neighbors is not less than Thresh_MBNSnum


    }

//    //****Determine the final matching point from the WidthNum*HeightNum points obtained by exact matching****
//    Point FirstBestPoint=BestPoints[0];
//    int FirstBestPointNum=0;
//    int FirstBestPointSort=0;
//    for(i=0;i<DiskNum;i++)
//    {
//        int PointNum=0;

//        PointNum=CountNeighbors(i, BestPoints, DiskNum,DisksCenterPoints, Thresh_distance);//Thresh_distance=R
//        //Get the sweet spot
//        if(PointNum>FirstBestPointNum)
//        {
//            FirstBestPoint=BestPoints[i];
//            FirstBestPointNum=PointNum;
//            FirstBestPointSort=i;
//        }


//    }

//    //Eliminate wrong points, and the points that are neighbors with the best matching point are considered to be correct points
//    Point RectUplPoint=(FirstBestPoint-DisksCenterPoints[FirstBestPointSort]);//The coordinates of the upper left corner of the rectangle
//    int rectupnum=1;
//    for(i=0;i<DiskNum;i++)
//    {
//        if(BestPoints[i].x==0 && BestPoints[i].y==0)
//        {
//            BestPoints[i]=Point(0,0);
//        }
//        else
//        {
//            if(FirstBestPointSort!=i)
//            {
//                float distance=get_distance(FirstBestPoint,BestPoints[i]);
//                float distemp=get_distance(DisksCenterPoints[FirstBestPointSort],DisksCenterPoints[i]);
//                distance=fabs(distance-distemp);//距离
//                if(distance>Thresh_distance)
//                {
//                    BestPoints[i]=Point(0,0);
//                }
//                else
//                {
//                    RectUplPoint=RectUplPoint+(BestPoints[i]-DisksCenterPoints[i]);
//                    rectupnum++;
//                }
//            }
//        }


//        circle(ImageFinalResult,BestPoints[i],2,CV_RGB(255,255,255),2,8,0);
//    }

//    timeMBNS=((double)getTickCount() - timeMBNS)/getTickFrequency();//operation hours
//    qDebug("MBNS time: %f seconds",timeMBNS);
//    Mat ImageFinalResult2;
//    GrayImage.copyTo(ImageFinalResult2);
//    circle(ImageFinalResult2,FirstBestPoint,2,CV_RGB(255,255,255),2,8,0);


//    RectUplPoint.x=(int)RectUplPoint.x/rectupnum;
//    RectUplPoint.y=(int)RectUplPoint.y/rectupnum;
//    RectUplOutPoint_MBNS = RectUplPoint;
//    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)0);


    /*
    //Draw a rectangle using the 2 best matching points
    //计算两向量夹角
    float AvectorAngle=AnglePolar(DisksCenterPoints[SecondBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle=AnglePolar(SecondBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita=BvectorAngle-AvectorAngle;
    if(Sita<0) Sita=Sita+360;


    Point RectUplPoint;//矩形左上角坐标
    RectUplPoint.x=(int)(-DisksCenterPoints[FirstBestPointSort].x*cos(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*sin(Sita/180*3.14) + FirstBestPoint.x);
    RectUplPoint.y=(int)(-(-DisksCenterPoints[FirstBestPointSort].x)*sin(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*cos(Sita/180*3.14)  + FirstBestPoint.y);
    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)Sita);
*/

    //****从精确匹配得到的WidthNum*HeightNum个点中确定最终匹配点****
    Point FirstBestPoint=BestPoints[0];
    int FirstBestPointNum=0;
    int FirstBestPointSort=0;

    Point SecondBestPoint=BestPoints[1];
    int SecondBestPointNum=0;
    int SecondBestPointSort=1;

    Point ThirdBestPoint=BestPoints[2];
    int ThirdBestPointNum=0;
    int ThirdBestPointSort=2;

    Point FourthBestPoint=BestPoints[3];
    int FourthBestPointNum=0;
    int FourthBestPointSort=3;
    for(i=0;i<DiskNum;i++)
    {
        int PointNum=0;

        PointNum=CountNeighbors(i, BestPoints, DiskNum,DisksCenterPoints, Thresh_distance);//Thresh_distance=R

        if(PointNum>FourthBestPointNum)
        {
            if(PointNum>ThirdBestPointNum)
            {
                if(PointNum>SecondBestPointNum)
                {
                    //获取最佳点
                    if(PointNum>FirstBestPointNum)
                    {
                        FirstBestPoint=BestPoints[i];
                        FirstBestPointNum=PointNum;
                        FirstBestPointSort=i;
                    }
                    else
                    {
                        SecondBestPoint=BestPoints[i];
                        SecondBestPointNum=PointNum;
                        SecondBestPointSort=i;
                    }
                }
                else
                {
                    ThirdBestPoint=BestPoints[i];
                    ThirdBestPointNum=PointNum;
                    ThirdBestPointSort=i;
                }
            }
            else
            {
                FourthBestPoint=BestPoints[i];
                FourthBestPointNum=PointNum;
                FourthBestPointSort=i;
            }
        }

    }
    //计算两向量夹角
    //12
    float AvectorAngle12=AnglePolar(DisksCenterPoints[SecondBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle12=AnglePolar(SecondBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita12=BvectorAngle12-AvectorAngle12;
    if(Sita12<0) Sita12=Sita12+360;
    //13
    float AvectorAngle13=AnglePolar(DisksCenterPoints[ThirdBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle13=AnglePolar(ThirdBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita13=BvectorAngle13-AvectorAngle13;
    if(Sita13<0) Sita13=Sita13+360;
    //14
    float AvectorAngle14=AnglePolar(DisksCenterPoints[FourthBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle14=AnglePolar(FourthBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita14=BvectorAngle14-AvectorAngle14;
    if(Sita14<0) Sita14=Sita14+360;


    //剔除错误点，与最佳匹配点是邻居的点认为是正确点
    int Sita12Num=0;
    int Sita13Num=0;
    int Sita14Num=0;
    float Sita12Sum=0;
    float Sita13Sum=0;
    float Sita14Sum=0;
    for(i=0;i<DiskNum;i++)
    {
        if(BestPoints[i].x==0 && BestPoints[i].y==0)
        {
            BestPoints[i]=Point(0,0);
        }
        else
        {
            if(FirstBestPointSort!=i)
            {
                float distance=get_distance(FirstBestPoint,BestPoints[i]);
                float distemp=get_distance(DisksCenterPoints[FirstBestPointSort],DisksCenterPoints[i]);
                distance=fabs(distance-distemp);//距离
                if(distance>Thresh_distance)
                {
                    BestPoints[i]=Point(0,0);
                }
                else
                {

                    float AvectorAngle=AnglePolar(DisksCenterPoints[i]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
                    float BvectorAngle=AnglePolar(BestPoints[i]-FirstBestPoint);//实际向量
                    //两向量夹角0-360°
                    float Sita=BvectorAngle-AvectorAngle;
                    if(Sita<0) Sita=Sita+360;

                    if(fabs(Sita-Sita12)<20)
                    {
                        Sita12Sum=Sita12Sum+Sita12;
                        Sita12Num++;

                    }
                    if(fabs(Sita-Sita13)<20)
                    {
                        Sita13Sum=Sita13Sum+Sita13;
                        Sita13Num++;

                    }
                    if(fabs(Sita-Sita14)<20)
                    {
                        Sita14Sum=Sita14Sum+Sita14;
                        Sita14Num++;

                    }
                }
            }
        }


        circle(ImageFinalResult,BestPoints[i],2,CV_RGB(255,255,255),2,8,0);
    }

    timeMBNS=((double)getTickCount() - timeMBNS)/getTickFrequency();//运行时间
    qDebug("MBNS耗时：%f秒",timeMBNS);
    Mat ImageFinalResult2;
    GrayImage.copyTo(ImageFinalResult2);
    circle(ImageFinalResult2,FirstBestPoint,2,CV_RGB(255,255,255),2,8,0);

    int SitaMostNum=Sita12Num;
    float Sita=Sita12Sum/Sita12Num;
    if(SitaMostNum<Sita13Num)
    {
        SitaMostNum=Sita13Num;
        Sita=Sita13Sum/Sita13Num;
    }
    if(SitaMostNum<Sita14Num)
    {
        SitaMostNum=Sita14Num;
        Sita=Sita14Sum/Sita14Num;
    }

    Point RectUplPoint;//矩形左上角坐标
    RectUplPoint.x=(int)(-DisksCenterPoints[FirstBestPointSort].x*cos(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*sin(Sita/180*3.14) + FirstBestPoint.x);
    RectUplPoint.y=(int)(-(-DisksCenterPoints[FirstBestPointSort].x)*sin(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*cos(Sita/180*3.14)  + FirstBestPoint.y);
    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)Sita);


    //显示结果
    qDebug("The coordinates of the upper left corner of the final matching rectangle=(%d,%d); rotation angle=%f°",RectUplPoint.x,RectUplPoint.y,0);
    imshow("GrayImageDisp",GrayImageDisp);//Local characteristics are rough matching to get candidate matching points
    imshow("ImageFinalResult",ImageFinalResult);//Most Neighbors Similarity: 获取9个最佳匹配点
    imshow("ImageFinalResult2",ImageFinalResult2);//Most Neighbors Similarity: 从9个匹配点中获取最佳的2个匹配点

    //***********************************************************************
    //***********************************************************************
    timeSum=((double)getTickCount() - timeSum)/getTickFrequency();//运行时间
    qDebug("Template matching is over, total time: %f seconds",timeSum);
    qDebug("***********************");


    Mat srcImageDisp;
    //resize函数前加cv：：，因为Qt也有resize函数，故加cv：：加以区别，不然会出现错误
    int resizeW=(int)ImageFinalResult2.cols/(beta+1);
    int resizeH=(int)ImageFinalResult2.rows/(beta+1);
    cv::resize(ImageFinalResult2,srcImageDisp,Size(resizeW,resizeH),0,0,3);
    //    Imgview->imshow(srcImageDisp);//显示到绑定的控件上







    //    if(flag00==1)
    //    {
    //         TrueCenterPoint=Point(CenterPoint_array.at(2*(inumber-2)),CenterPoint_array.at(2*(inumber-2)+1));
    //        //测试结果
    //        MBNS_ST=MBNS_ST+timeSum;

    //        float distance1=get_distance(Point(RectUplPoint.x+(int)TemplateImage.cols/2,RectUplPoint.y+(int)TemplateImage.rows/2),TrueCenterPoint);
    //        float distance2=sqrt(TemplateImage.cols*TemplateImage.rows)/2;

    //        if(distance1<=distance2)
    //        {
    //            MBNS_QS++;
    //            MBNS_Sa=MBNS_Sa+(1-(float)distance1/distance2);
    //        }

    //    }



}

Point RectUplOutPoint_MNS;
void MNS_tMatching(Mat Image,Mat TemplateImage){

    int i,j;
    //一、图像和模板都转为单通道的灰度图
    Mat GrayImage;
    cvtColor(Image,GrayImage,CV_RGB2GRAY,1);
    Mat GrayTemplate;
    cvtColor(TemplateImage,GrayTemplate,CV_RGB2GRAY,1);
    //qDebug("待匹配图宽高=【%d x %d】；模板图宽高=【%d x %d】",GrayImage.cols,GrayImage.rows,GrayTemplate.cols,GrayTemplate.rows);

    //二、将模板图分成多个Ldisk*Ldisk的小方块，每个方块圆心相距R
    R=(int)Ldisk/2;//局部方块半径
    int WidthNum=(int)GrayTemplate.cols/Ldisk*2 - 1;//一行的方块数
    int HeightNum=(int)GrayTemplate.rows/Ldisk*2 - 1;//一列的方块数
    //qDebug("模板方块边长=%d",Ldisk);
    //qDebug("模板方块数量=%d",WidthNum*HeightNum);

    //四（1）、建立环投影的环查找表
    //    int x[R];//环距离
    //    memset( x, 0, R*sizeof(int) );
    vector<int> x(R);
    x[0]=1;
    int xnum=1;
    for(i=0;i<R;i++)
    {
        x[i+1]=(int)(R-sqrt((R-x[i])*(R-x[i])-(2*R-1))+0.5);
        xnum++;
        if(x[i+1]<=x[i])
        {
            x[i+1]=0;
            xnum--;
            break;
        }
    }

    //    uint AnnulusTable[Ldisk*Ldisk];//环查找表
    vector<uint> AnnulusTable(Ldisk*Ldisk);
    vector<uint> anNumber(xnum);//统计同个环的像素数量
    //    memset( anNumber, 0, xnum*sizeof(uint) );
    vector<uint> Rtable(Ldisk*Ldisk);//半径查找表
    vector<uint> rNumber(R+1);//统计同个半径值的数量
    //    memset( rNumber, 0, R*sizeof(uint) );
    vector<uint> aNumber(360);//统计同个角度值的数量
    vector<uint> Atable(Ldisk*Ldisk);//角度查找表
    double YdivX;//斜率
    int A=0;//角度
    int r;//半径


    for(i=0;i<Ldisk;i++)
    {
        for(j=0;j<Ldisk;j++)
        {
            r=(int)(sqrt((i-R)*(i-R)+(j-R)*(j-R)) + 0.5);

            //建立极坐标（半径和角度）查找表
            //角度转换
            if(j>R-1 && i==R)
            {
                A=0;
            }
            else if(j<R && i==R)
            {
                A=180;
            }
            else if(j==R && i<R)
            {
                A=90;
            }
            else if(j==R && i>R)
            {
                A=270;
            }
            else if(j>R && i<R)
            {
                YdivX=(double)(R-i)/(j-R);
                A=(int)(atan(YdivX)*180/3.14159);
            }
            else if(j<R && i<R)
            {
                YdivX=(double)(R-j)/(R-i);
                A=(int)(atan(YdivX)*180/3.14159)+90;
            }
            else if(j<R && i>R)
            {
                YdivX=(double)(i-R)/(R-j);
                A=(int)(atan(YdivX)*180/3.14159)+180;
            }
            else if(j>R && i>R)
            {
                YdivX=(double)(j-R)/(i-R);
                A=(int)(atan(YdivX)*180/3.14159)+270;
            }


            if(r>R)
            {
                Rtable[i*Ldisk+j] = R+1;
                Atable[i*Ldisk+j] = 361;
            }
            else
            {
                Rtable[i*Ldisk+j] = r;
                Atable[i*Ldisk+j] = A;
                rNumber[r]++;
                aNumber[A]++;
            }

            // **************************
            // **************************
            //建立环查找表
            if(r>R)
            {
                AnnulusTable[i*Ldisk+j] = xnum;
            }
            else
            {
                for(int ii=0;ii<xnum;ii++)
                {
                    if(r>(R-x[ii]))
                    {
                        AnnulusTable[i*Ldisk+j] = ii;
                        anNumber[ii]++;
                        break;
                    }
                }

                if(r<=(R-x[xnum-1]))
                {
                    AnnulusTable[i*Ldisk+j] = xnum-1;
                    anNumber[xnum-1]++;
                }
            }

        }
    }

    //四（2）、计算模板图的环投影向量
    vector<Mat> Disks;//方块
    vector<Point> DisksCenterPoints;//每个方块的中心坐标点
    int DiskNum=0;//真正的方块数量
    for(i=0;i<WidthNum*HeightNum;i++)
    {
        Mat GrayTemplatetemp(GrayTemplate(Rect(((int)i%WidthNum)*R,((int)i/WidthNum)*R,Ldisk,Ldisk)));
        vector<float> Ptemp(xnum);

        Anvector(GrayTemplatetemp,AnnulusTable,xnum,anNumber,Ptemp);
        float PtempAver=0;
        for(j=0;j<xnum;j++)
        {
            PtempAver=PtempAver+Ptemp[j];
        }
        PtempAver=PtempAver/xnum;
        float P_SSD=0;
        for(j=0;j<xnum;j++)
        {
            P_SSD=P_SSD+(Ptemp[j]-PtempAver)*(Ptemp[j]-PtempAver);
        }

        if(P_SSD>4*xnum)
        {
            Disks.push_back(GrayTemplate(Rect(((int)i%WidthNum)*R,((int)i/WidthNum)*R,Ldisk,Ldisk)));
            DisksCenterPoints.push_back(Point(((int)i%WidthNum)*R+R,((int)i/WidthNum)*R+R));
            DiskNum++;
        }

    }
    qDebug("Actual number of blocks = %d",DiskNum);

    //环投影向量
    vector<vector<float>> P(DiskNum,vector<float>(xnum));//模板图环投影向量

    for(i=0;i<DiskNum;i++)
    {
        //环投影向量
        Anvector(Disks[i],AnnulusTable,xnum,anNumber,P[i]);

    }

    //*****五、开始匹配************************************
    //*******************************************
    double timeSum;//总耗时
    timeSum=static_cast<double>(getTickCount());
    //***********************************************************
    double timeAnnulus;
    timeAnnulus=static_cast<double>(getTickCount());
    //设置圆投影搜索范围
    int LeftUpx=0;
    int LeftUpy=0;
    int RrightDownx=(int)(GrayImage.cols-Ldisk-1);
    int RrightDowny=(int)(GrayImage.rows-Ldisk-1);

    //获取并标出候选点位置
    double RminValue=0;
    Point RminLoc;
    double RmaxValue=0;
    Point RmaxLoc;
    Mat GrayImageDisp;
    GrayImage.copyTo(GrayImageDisp);

    int CandidateNum=MatchingNum2;
    vector<Point>* Points = new vector<Point>[DiskNum];//精确匹配点坐标数组
    vector<Mat> MatchingValue;//环投影相似度值
    for(int ii=0;ii<DiskNum;ii++)
    {
        Mat MatchingValueTemp(GrayImage.rows-Ldisk,GrayImage.cols-Ldisk,CV_32FC1,Scalar(999999999));//float型单通道，每个点初始化
        MatchingValue.push_back(MatchingValueTemp);
    }
    for(i=LeftUpy;i<RrightDowny+1;i++)
    {

        for(j=LeftUpx;j<RrightDownx;j++)
        {

            Mat ScanROI;//待匹配的子图
            ScanROI=GrayImage(Rect(j,i,Ldisk,Ldisk));
            vector<float> T(xnum);//模板图环投影向量

            Anvector(ScanROI,AnnulusTable,xnum,anNumber,T);
            float T_Aver=0;
            for(int it=0;it<xnum;it++)
            {
                T_Aver=T_Aver+T[it];
            }
            T_Aver=T_Aver/xnum;
            float T_SSD=0;
            for(int it=0;it<xnum;it++)
            {
                T_SSD=T_SSD+(T[it]-T_Aver)*(T[it]-T_Aver);
            }

            if(T_SSD>4*xnum)
            {
                for(int ii=0;ii<DiskNum;ii++)
                {
                    float* buffer;
                    buffer=MatchingValue[ii].ptr<float>(i);
                    //相似度计算
                    int tempbuffer=0;
                    for(int k=0;k<xnum;k++)
                    {
                        tempbuffer=tempbuffer+(P[ii][k]-T[k])*(P[ii][k]-T[k]);

                    }
                    buffer[j]=tempbuffer;

                }
            }
        }
    }

    for(int ii=0;ii<DiskNum;ii++)
    {
        for(i=0;i<CandidateNum;i++)
        {
            //1、选取候选匹配点
            minMaxLoc(MatchingValue[ii],&RminValue,&RmaxValue,&RminLoc,&RmaxLoc);
            Point BestMatch=RminLoc;
            Point Centerpoint=Point(BestMatch.x+R,BestMatch.y+R);

            //画圆
            //circle(GrayImageDisp,Centerpoint,2,CV_RGB(255,255,255),2,8,0);
            Points[ii].push_back(Centerpoint);

            float* buffervalue=MatchingValue[ii].ptr<float>(BestMatch.y);
            buffervalue[BestMatch.x]=999999999;
        }
    }
    timeAnnulus=((double)getTickCount() - timeAnnulus)/getTickFrequency();//运行时间
    //qDebug("环投影匹配耗时：%f秒",timeAnnulus);


    double timeMBNS;//MBNS耗时
    timeMBNS=static_cast<double>(getTickCount());
    //***************************************************
    //Neighbors Constraint Similarity确定最终匹配点
    //***************************************************
    Point* BestPoints = new Point[DiskNum];//最佳匹配点
    vector<int> BestPointNum(DiskNum);//最佳匹配点邻居数量

    Thresh_distance=(float)(PSMode*R);//邻居距离阈值
    Mat ImageFinalResult;//显示结果
    GrayImage.copyTo(ImageFinalResult);

    for(int ii=0;ii<DiskNum;ii++)
    {

        for(i=0;i<Points[ii].size();i++)
        {

            //邻居数量
            int PointNum=0;
            for(int jj=0;jj<DiskNum;jj++)
            {
                //获取邻居点坐标
                for(j=0;j<Points[jj].size();j++)
                {
                    if(jj!=ii)
                    {
                        float distance=get_distance(Points[ii][i],Points[jj][j]);
                        float distemp=get_distance(DisksCenterPoints[ii],DisksCenterPoints[jj]);
                        distance=fabs(distance-distemp);
                        if(distance<Thresh_distance)
                        {
                            PointNum++;//第一多的邻居数量
                            break;
                        }
                    }

                }

                //获取最佳点坐标
                if(PointNum>BestPointNum[ii])
                {
                    BestPoints[ii]=Points[ii][i];
                    BestPointNum[ii]=PointNum;
                }
            }

        }
        if(BestPointNum[ii]<Thresh_MBNSnum)  BestPoints[ii]=Point(0,0);//阈值：最多邻居数量不少于Thresh_MBNSnum

    }


//    //****从精确匹配得到的WidthNum*HeightNum个点中确定最终匹配点****
//    Point FirstBestPoint=BestPoints[0];
//    int FirstBestPointNum=0;
//    int FirstBestPointSort=0;

//    for(i=0;i<DiskNum;i++)
//    {
//        int PointNum=0;

//        PointNum=CountNeighbors(i, BestPoints, DiskNum,DisksCenterPoints, Thresh_distance);//Thresh_distance=R
//        //获取最佳点
//        if(PointNum>FirstBestPointNum)
//        {
//            FirstBestPoint=BestPoints[i];
//            FirstBestPointNum=PointNum;
//            FirstBestPointSort=i;
//        }

//    }

//    //剔除错误点，与最佳匹配点是邻居的点认为是正确点
//    Point RectUplPoint=(FirstBestPoint-DisksCenterPoints[FirstBestPointSort]);//矩形左上角坐标
//    int rectupnum=1;
//    for(i=0;i<DiskNum;i++)
//    {
//        if(BestPoints[i].x==0 && BestPoints[i].y==0)
//        {
//            BestPoints[i]=Point(0,0);
//        }
//        else
//        {
//            if(FirstBestPointSort!=i)
//            {
//                float distance=get_distance(FirstBestPoint,BestPoints[i]);
//                float distemp=get_distance(DisksCenterPoints[FirstBestPointSort],DisksCenterPoints[i]);
//                distance=fabs(distance-distemp);//距离
//                if(distance>Thresh_distance)
//                {
//                    BestPoints[i]=Point(0,0);
//                }
//                else
//                {
//                    RectUplPoint=RectUplPoint+(BestPoints[i]-DisksCenterPoints[i]);
//                    rectupnum++;
//                }
//            }
//        }


//        //circle(ImageFinalResult,BestPoints[i],2,CV_RGB(255,255,255),2,8,0);
//    }

//    timeMBNS=((double)getTickCount() - timeMBNS)/getTickFrequency();//运行时间
//    qDebug("MBNS耗时：%f秒",timeMBNS);
//    Mat ImageFinalResult2;
//    GrayImage.copyTo(ImageFinalResult2);
//    circle(ImageFinalResult2,FirstBestPoint,2,CV_RGB(255,255,255),2,8,0);

//    RectUplPoint.x=(int)RectUplPoint.x/rectupnum;
//    RectUplPoint.y=(int)RectUplPoint.y/rectupnum;
//    RectUplOutPoint_MNS = RectUplPoint;
//    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)0);
//    //    cv::imshow("Final results",ImageFinalResult2);
    //****从精确匹配得到的WidthNum*HeightNum个点中确定最终匹配点****
    Point FirstBestPoint=BestPoints[0];
    int FirstBestPointNum=0;
    int FirstBestPointSort=0;

    Point SecondBestPoint=BestPoints[1];
    int SecondBestPointNum=0;
    int SecondBestPointSort=1;

    Point ThirdBestPoint=BestPoints[2];
    int ThirdBestPointNum=0;
    int ThirdBestPointSort=2;

    Point FourthBestPoint=BestPoints[3];
    int FourthBestPointNum=0;
    int FourthBestPointSort=3;
    for(i=0;i<DiskNum;i++)
    {
        int PointNum=0;

        PointNum=CountNeighbors(i, BestPoints, DiskNum,DisksCenterPoints, Thresh_distance);//Thresh_distance=R

        if(PointNum>FourthBestPointNum)
        {
            if(PointNum>ThirdBestPointNum)
            {
                if(PointNum>SecondBestPointNum)
                {
                    //获取最佳点
                    if(PointNum>FirstBestPointNum)
                    {
                        FirstBestPoint=BestPoints[i];
                        FirstBestPointNum=PointNum;
                        FirstBestPointSort=i;
                    }
                    else
                    {
                        SecondBestPoint=BestPoints[i];
                        SecondBestPointNum=PointNum;
                        SecondBestPointSort=i;
                    }
                }
                else
                {
                    ThirdBestPoint=BestPoints[i];
                    ThirdBestPointNum=PointNum;
                    ThirdBestPointSort=i;
                }
            }
            else
            {
                FourthBestPoint=BestPoints[i];
                FourthBestPointNum=PointNum;
                FourthBestPointSort=i;
            }
        }

    }
    //计算两向量夹角
    //12
    float AvectorAngle12=AnglePolar(DisksCenterPoints[SecondBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle12=AnglePolar(SecondBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita12=BvectorAngle12-AvectorAngle12;
    if(Sita12<0) Sita12=Sita12+360;
    //13
    float AvectorAngle13=AnglePolar(DisksCenterPoints[ThirdBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle13=AnglePolar(ThirdBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita13=BvectorAngle13-AvectorAngle13;
    if(Sita13<0) Sita13=Sita13+360;
    //14
    float AvectorAngle14=AnglePolar(DisksCenterPoints[FourthBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle14=AnglePolar(FourthBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita14=BvectorAngle14-AvectorAngle14;
    if(Sita14<0) Sita14=Sita14+360;


    //剔除错误点，与最佳匹配点是邻居的点认为是正确点
    int Sita12Num=0;
    int Sita13Num=0;
    int Sita14Num=0;
    float Sita12Sum=0;
    float Sita13Sum=0;
    float Sita14Sum=0;
    for(i=0;i<DiskNum;i++)
    {
        if(BestPoints[i].x==0 && BestPoints[i].y==0)
        {
            BestPoints[i]=Point(0,0);
        }
        else
        {
            if(FirstBestPointSort!=i)
            {
                float distance=get_distance(FirstBestPoint,BestPoints[i]);
                float distemp=get_distance(DisksCenterPoints[FirstBestPointSort],DisksCenterPoints[i]);
                distance=fabs(distance-distemp);//距离
                if(distance>Thresh_distance)
                {
                    BestPoints[i]=Point(0,0);
                }
                else
                {

                    float AvectorAngle=AnglePolar(DisksCenterPoints[i]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
                    float BvectorAngle=AnglePolar(BestPoints[i]-FirstBestPoint);//实际向量
                    //两向量夹角0-360°
                    float Sita=BvectorAngle-AvectorAngle;
                    if(Sita<0) Sita=Sita+360;

                    if(fabs(Sita-Sita12)<20)
                    {
                        Sita12Sum=Sita12Sum+Sita12;
                        Sita12Num++;

                    }
                    if(fabs(Sita-Sita13)<20)
                    {
                        Sita13Sum=Sita13Sum+Sita13;
                        Sita13Num++;

                    }
                    if(fabs(Sita-Sita14)<20)
                    {
                        Sita14Sum=Sita14Sum+Sita14;
                        Sita14Num++;

                    }
                }
            }
        }


        circle(ImageFinalResult,BestPoints[i],2,CV_RGB(255,255,255),2,8,0);
    }

    timeMBNS=((double)getTickCount() - timeMBNS)/getTickFrequency();//运行时间
    qDebug("MBNS耗时：%f秒",timeMBNS);
    Mat ImageFinalResult2;
    GrayImage.copyTo(ImageFinalResult2);
    circle(ImageFinalResult2,FirstBestPoint,2,CV_RGB(255,255,255),2,8,0);

    int SitaMostNum=Sita12Num;
    float Sita=Sita12Sum/Sita12Num;
    if(SitaMostNum<Sita13Num)
    {
        SitaMostNum=Sita13Num;
        Sita=Sita13Sum/Sita13Num;
    }
    if(SitaMostNum<Sita14Num)
    {
        SitaMostNum=Sita14Num;
        Sita=Sita14Sum/Sita14Num;
    }

    Point RectUplPoint;//矩形左上角坐标
    RectUplPoint.x=(int)(-DisksCenterPoints[FirstBestPointSort].x*cos(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*sin(Sita/180*3.14) + FirstBestPoint.x);
    RectUplPoint.y=(int)(-(-DisksCenterPoints[FirstBestPointSort].x)*sin(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*cos(Sita/180*3.14)  + FirstBestPoint.y);
    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)Sita);

    /*
    //利用2个最佳匹配点画矩形框
    //计算两向量夹角
    float AvectorAngle=AnglePolar(DisksCenterPoints[SecondBestPointSort]-DisksCenterPoints[FirstBestPointSort]);//原始参考向量
    float BvectorAngle=AnglePolar(SecondBestPoint-FirstBestPoint);//实际向量
    //两向量夹角0-360°
    float Sita=BvectorAngle-AvectorAngle;
    if(Sita<0) Sita=Sita+360;


    Point RectUplPoint;//矩形左上角坐标
    RectUplPoint.x=(int)(-DisksCenterPoints[FirstBestPointSort].x*cos(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*sin(Sita/180*3.14) + FirstBestPoint.x);
    RectUplPoint.y=(int)(-(-DisksCenterPoints[FirstBestPointSort].x)*sin(Sita/180*3.14)+(-DisksCenterPoints[FirstBestPointSort].y)*cos(Sita/180*3.14)  + FirstBestPoint.y);
    DrawRotationRect(ImageFinalResult2,RectUplPoint,GrayTemplate.cols,GrayTemplate.rows,(double)Sita);
*/


    //***********************************************************************
    //***********************************************************************

    //显示结果
    qDebug("The coordinates of the upper left corner of the final matching rectangle=(%d,%d); rotation angle=%f°",RectUplPoint.x,RectUplPoint.y,0);
    imshow("GrayImageDisp MNS",GrayImageDisp);//局部特征粗匹配得到候选匹配点集
    imshow("ImageFinalResult MNS",ImageFinalResult);//Most Neighbors Similarity: 获取9个最佳匹配点
    imshow("ImageFinalResult2 MNS",ImageFinalResult2);//Most Neighbors Similarity: 从9个匹配点中获取最佳的2个匹配点

    //***********************************************************************
    //***********************************************************************
    timeSum=((double)getTickCount() - timeSum)/getTickFrequency();//运行时间
    qDebug("Template matching is over, total time: %f seconds",timeSum);
    qDebug("***********************");


    Mat srcImageDisp;
    //resize函数前加cv：：，因为Qt也有resize函数，故加cv：：加以区别，不然会出现错误
    int resizeW=(int)ImageFinalResult2.cols/(beta+1);
    int resizeH=(int)ImageFinalResult2.rows/(beta+1);
    cv::resize(ImageFinalResult2,srcImageDisp,Size(resizeW,resizeH),0,0,3);


    if(flag00==1)
    {
        //测试结果
        MNS_ST=MNS_ST+timeSum;

        float distance1=get_distance(Point(RectUplPoint.x+(int)TemplateImage.cols/2,RectUplPoint.y+(int)TemplateImage.rows/2),TrueCenterPoint);
        float distance2=sqrt(TemplateImage.cols*TemplateImage.rows)/2;

        if(distance1<=distance2)
        {
            MNS_QS++;
            MNS_Sa=MNS_Sa+(1-(float)distance1/distance2);
        }

    }



}


void SAD_tMatching(Mat Image,Mat TemplateImage){

    int i,j;
    //一、图像和模板都转为单通道的灰度图
    Mat GrayImage;
    cvtColor(Image,GrayImage,CV_RGB2GRAY,1);
    Mat GrayTemplate;
    cvtColor(TemplateImage,GrayTemplate,CV_RGB2GRAY,1);


    //*****五、开始匹配************************************
    double timeSum;//总耗时
    timeSum=static_cast<double>(getTickCount());

    //遍历范围
    int LeftUpx=0;
    int LeftUpy=0;
    int RrightDownx=GrayImage.cols-GrayTemplate.cols-1;
    int RrightDowny=GrayImage.rows-GrayTemplate.rows-1;

    Mat MatchingValue(RrightDowny,RrightDownx,CV_32FC1,Scalar(0));//float型单通道，每个点初始化为0
    float* buffer;//MatchingValue的行指针
    uchar* buffer_AD_Img;

    //遍历待匹配图
    for(i=LeftUpy;i<RrightDowny;i++)
    {
        buffer=MatchingValue.ptr<float>(i);
        for(j=LeftUpx;j<RrightDownx;j++)
        {

            Mat Sub_MatchImg;//待匹配子图
            Sub_MatchImg = GrayImage(Rect(j,i,GrayTemplate.cols,GrayTemplate.rows));
            Mat AD_Img;//绝对差值图：两图相减再求绝对值
            absdiff(GrayTemplate,Sub_MatchImg,AD_Img);

            //遍历计算模板和子图的相似度
            float tempsum=0;
            for(int ii=0;ii<GrayTemplate.rows;ii++)
            {
                buffer_AD_Img=AD_Img.ptr<uchar>(ii);
                for(int jj=0;jj<GrayTemplate.cols;jj++)
                {
                    uchar x=buffer_AD_Img[jj];

                    //各类相似度公式
                    float SAD=x;//1、SAD:h(x)=x
                    //float SSD=x*x;//2、SSD:h(x)=x*x
                    tempsum=tempsum+SAD;

                }
            }

            buffer[j]=tempsum;
        }

    }

    Point MinLoc,MaxLoc,BestLoc;
    minMaxLoc(MatchingValue,NULL,NULL,&MinLoc,&MaxLoc);

    BestLoc=MinLoc;

    timeSum=((double)getTickCount() - timeSum)/getTickFrequency();//运行时间
    qDebug("模板匹配结束，总耗时：%f秒",timeSum);

    Point Centerpoint=Point(BestLoc.x+(int)GrayTemplate.cols/2,BestLoc.y+(int)GrayTemplate.rows/2);



    if(flag00==1)
    {
        //测试结果
        SAD_ST=SAD_ST+timeSum;

        float distance1=get_distance(Centerpoint,TrueCenterPoint);
        float distance2=sqrt(TemplateImage.cols*TemplateImage.rows)/2;

        if(distance1<=distance2)
        {
            SAD_QS++;
            SAD_Sa=SAD_Sa+(1-(float)distance1/distance2);
        }

    }

}


void SSD_tMatching(Mat Image,Mat TemplateImage){

    int i,j;
    //一、图像和模板都转为单通道的灰度图
    Mat GrayImage;
    cvtColor(Image,GrayImage,CV_RGB2GRAY,1);
    Mat GrayTemplate;
    cvtColor(TemplateImage,GrayTemplate,CV_RGB2GRAY,1);


    //*****五、开始匹配************************************
    double timeSum;//总耗时
    timeSum=static_cast<double>(getTickCount());

    //遍历范围
    int LeftUpx=0;
    int LeftUpy=0;
    int RrightDownx=GrayImage.cols-GrayTemplate.cols-1;
    int RrightDowny=GrayImage.rows-GrayTemplate.rows-1;

    Mat MatchingValue(RrightDowny,RrightDownx,CV_32FC1,Scalar(0));//float型单通道，每个点初始化为0
    float* buffer;//MatchingValue的行指针
    uchar* buffer_AD_Img;

    //遍历待匹配图
    for(i=LeftUpy;i<RrightDowny;i++)
    {
        buffer=MatchingValue.ptr<float>(i);
        for(j=LeftUpx;j<RrightDownx;j++)
        {

            Mat Sub_MatchImg;//待匹配子图
            Sub_MatchImg = GrayImage(Rect(j,i,GrayTemplate.cols,GrayTemplate.rows));
            Mat AD_Img;//绝对差值图：两图相减再求绝对值
            absdiff(GrayTemplate,Sub_MatchImg,AD_Img);

            //遍历计算模板和子图的相似度
            float tempsum=0;
            for(int ii=0;ii<GrayTemplate.rows;ii++)
            {
                buffer_AD_Img=AD_Img.ptr<uchar>(ii);
                for(int jj=0;jj<GrayTemplate.cols;jj++)
                {
                    uchar x=buffer_AD_Img[jj];

                    //各类相似度公式
                    //float SAD=x;//1、SAD:h(x)=x
                    float SSD=x*x;//2、SSD:h(x)=x*x
                    tempsum=tempsum+SSD;

                }
            }

            buffer[j]=tempsum;
        }

    }

    Point MinLoc,MaxLoc,BestLoc;
    minMaxLoc(MatchingValue,NULL,NULL,&MinLoc,&MaxLoc);

    BestLoc=MinLoc;

    timeSum=((double)getTickCount() - timeSum)/getTickFrequency();//运行时间
    qDebug("模板匹配结束，总耗时：%f秒",timeSum);

    Point Centerpoint=Point(BestLoc.x+(int)GrayTemplate.cols/2,BestLoc.y+(int)GrayTemplate.rows/2);


    if(flag00==1)
    {
        //测试结果
        SSD_ST=SSD_ST+timeSum;

        float distance1=get_distance(Centerpoint,TrueCenterPoint);
        float distance2=sqrt(TemplateImage.cols*TemplateImage.rows)/2;

        if(distance1<=distance2)
        {
            SSD_QS++;
            SSD_Sa=SSD_Sa+(1-(float)distance1/distance2);
        }

    }
}

void NCC_tMatching(Mat Image,Mat TemplateImage){

    int i,j;
    //一、图像和模板都转为单通道的灰度图
    Mat GrayImage;
    cvtColor(Image,GrayImage,CV_RGB2GRAY,1);
    Mat GrayTemplate;
    cvtColor(TemplateImage,GrayTemplate,CV_RGB2GRAY,1);


    Scalar Templatemean;
    Scalar Templatedev;
    cv::meanStdDev ( GrayTemplate, Templatemean, Templatedev );
    float Templatem = Templatemean.val[0];
    //float Templates = Templatedev.val[0];


    //*****五、开始匹配************************************
    double timeSum;//总耗时
    timeSum=static_cast<double>(getTickCount());

    //遍历范围
    int LeftUpx=0;
    int LeftUpy=0;
    int RrightDownx=GrayImage.cols-GrayTemplate.cols-1;
    int RrightDowny=GrayImage.rows-GrayTemplate.rows-1;

    Mat MatchingValue(RrightDowny,RrightDownx,CV_32FC1,Scalar(0));//float型单通道，每个点初始化为0
    float* buffer;//MatchingValue的行指针
    uchar* buffer_Img;
    uchar* buffer_template;

    //遍历待匹配图
    for(i=LeftUpy;i<RrightDowny;i++)
    {
        buffer=MatchingValue.ptr<float>(i);
        for(j=LeftUpx;j<RrightDownx;j++)
        {

            Mat Sub_MatchImg;//待匹配子图
            Sub_MatchImg = GrayImage(Rect(j,i,GrayTemplate.cols,GrayTemplate.rows));
            Scalar Matchmean;
            Scalar Matchdev;
            cv::meanStdDev ( Sub_MatchImg, Matchmean, Matchdev );
            float Matchm = Matchmean.val[0];
            //float Matchs = Matchdev.val[0];

            //遍历计算模板和子图的相似度
            float NCC1=0;
            float NCC2=0;
            float NCC3=0;
            for(int ii=0;ii<GrayTemplate.rows;ii++)
            {
                buffer_Img=Sub_MatchImg.ptr<uchar>(ii);
                buffer_template=GrayTemplate.ptr<uchar>(ii);

                for(int jj=0;jj<GrayTemplate.cols;jj++)
                {
                    //NCC
                    NCC1=NCC1+fabs(buffer_Img[jj]-Matchm)*fabs(buffer_template[jj]-Templatem);
                    NCC2=NCC2+(buffer_Img[jj]-Matchm)*(buffer_Img[jj]-Matchm);
                    NCC3=NCC3+(buffer_template[jj]-Templatem)*(buffer_template[jj]-Templatem);

                }
            }

            buffer[j]=NCC1/sqrt(NCC2)/sqrt(NCC3);
        }

    }

    Point MinLoc,MaxLoc,BestLoc;
    minMaxLoc(MatchingValue,NULL,NULL,&MinLoc,&MaxLoc);

    BestLoc=MaxLoc;

    timeSum=((double)getTickCount() - timeSum)/getTickFrequency();//运行时间
    qDebug("模板匹配结束，总耗时：%f秒",timeSum);

    Point Centerpoint=Point(BestLoc.x+(int)GrayTemplate.cols/2,BestLoc.y+(int)GrayTemplate.rows/2);

    if(flag00==1)
    {
        //测试结果
        NCC_ST=NCC_ST+timeSum;

        float distance1=get_distance(Centerpoint,TrueCenterPoint);
        float distance2=sqrt(TemplateImage.cols*TemplateImage.rows)/2;

        if(distance1<=distance2)
        {
            NCC_QS++;
            NCC_Sa=NCC_Sa+(1-(float)distance1/distance2);
        }

    }

}




#include <fstream>
#include <string>
#include <iostream>
#include <streambuf>
void MainWindow::on_pushButtonMatch_clicked()
{

    //MBNS_tMatching(Image,ROI);

    if(flag00==0)
    {
        //第0次匹配，校准
        MBNS_tMatching(Image,ROI);
        MNS_tMatching(Image,ROI);
    }
    flag00=0;

    //****************************
    //****************************
    //    int QI=6;//Quantity of Images

    //    for(inumber=2;inumber<QI+2;inumber++)
    //    {
    //        char strpath[200];
    //        sprintf(strpath,"F:/MyAlgorithmProject/20170601_TemplateMatching_FeatureBase_RPT_Zernike/QtCode/QtCode_SelfBestBuddiesSimilarity(20180223)/TextImage/5Deformation/wine_lable16/%d.png",inumber);
    //        Image=imread(strpath);

    //        MBNS_tMatching(Image,ROI);
    ////        MNS_tMatching(Image,ROI);
    //        SAD_tMatching(Image,ROI);
    //        SSD_tMatching(Image,ROI);
    //        NCC_tMatching(Image,ROI);

    //    }

    //qDebug("QI=%d",QI);
    //qDebug("ST=%f",ST);
    //qDebug("QS=%d",QS);
    //qDebug("Sa=%f",Sa);

    //输出到exel
    //定义文件输出流
    //    ofstream oFile;
    //    //打开要输出的文件
    //    oFile.open("data.csv", ios::out | ios::trunc);    // 这样就很容易的输出一个需要的excel 文件
    //    oFile << QI << "," << QI << "," << QI << "," << QI << "," << QI << endl;
    //    oFile << MNS_ST << "," << MBNS_ST << "," << SAD_ST << "," << SSD_ST << "," << NCC_ST << endl;
    //    oFile << MNS_QS << "," << MBNS_QS << "," << SAD_QS << "," << SSD_QS << "," << NCC_QS << endl;
    //    oFile << MNS_Sa << "," << MBNS_Sa << "," << SAD_Sa << "," << SSD_Sa << "," << NCC_Sa << endl;
    //    //oFile << MNS_SAE << "," << MBNS_SAE << endl;
    //    oFile.close();
    //    //打开要输出的文件
    //    //ifstream iFile("data.csv");
    //    //string   readStr((std::istreambuf_iterator<char>(iFile)),  std::istreambuf_iterator<char>());
    //     //cout <<  readStr.c_str();

    qDebug("OK!OK!OK!");
    qDebug("OK!OK!OK!");


}

void MainWindow::on_pushButtonROI_3_clicked()
{
    QString str0=ui->lineEdit_3->text();
    PSMode=str0.toFloat();
    qDebug(" PS Mode: %f",PSMode);
    Thresh_distance=(float)(PSMode*R);//邻居距离阈值

    QString str=ui->lineEdit->text();
    Ldisk=str.toInt();
    qDebug("Ldisk: %d",Ldisk);
    R=(int)Ldisk/2;//局部方块半径

    QString str2=ui->lineEdit_2->text();
    Rthre=str2.toFloat();
    qDebug("%f",Rthre);

    QString str6=ui->lineEdit_6->text();
    Thresh_MBNSnum=str6.toInt();
    qDebug("%d",Thresh_MBNSnum);

    QString str4=ui->lineEdit_4->text();
    MatchingNum2=str4.toFloat();
    qDebug("%d",MatchingNum2);

    QString str5=ui->lineEdit_5->text();
    N=str5.toFloat();
    qDebug("%d",N);

    QString str8=ui->lineEdit_8->text();
    popular=str8.toInt();
    qDebug("%d",popular);

    QString str7=ui->lineEdit_7->text();
    Maxgen=str7.toInt();
    qDebug("%d",Maxgen);

}

void MainWindow::on_pushButtonTestUI_clicked()
{
    qDebug("Test UI");
}


#include <fstream>

// copy in binary mode
bool copyFile(const char *SRC, const char* DEST)
{
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    return src && dest;
}


void MainWindow::on_pushButtonSearch_clicked()
{
    qDebug("In search of incredible Search");

    QString outpath = "D:/4000436/XAI/test_25/test_images_release/test_out_AMNS/";

    vector<cv::String> fn;
//    glob("D:/4000436/XAI/test_25/test_images_release/test_matchting/*.png", fn, false);
    auto datasetroot = QString("%1/*.png").arg(datasetpath);
    glob(datasetroot.toLocal8Bit().constData(),fn,false);

    vector<Mat> images;
    vector<cv::String> imagenames;
    size_t count = fn.size(); //number of png files in images folder
    int resizeWidth;//显示宽度
    int resizeHeight;//显示高度

    for (size_t i=0; i<count; i++)
    {
        cv::String s = fn[i];
        cv::String filename = s.substr(s.find("\\")+1,s.length());
        imagenames.push_back(filename);
        //        beta = 3;
        Mat Image = imread(fn[i]);
        switch(beta){
        case 0:
        {
            resizeWidth=Image.cols;
            resizeHeight=Image.rows;
        }break;
        case 1:
        {
            resizeWidth=(int)Image.cols/2;
            resizeHeight=(int)Image.rows/2;
        }break;
        case 2:
        {
            resizeWidth=(int)Image.cols/3;
            resizeHeight=(int)Image.rows/3;
        }break;
        case 3:
        {
            resizeWidth=(int)Image.cols/4;
            resizeHeight=(int)Image.rows/4;
        }break;
        case 4:
        {
            resizeWidth=(int)Image.cols/5;
            resizeHeight=(int)Image.rows/5;
        }break;
        case 5:
        {
            resizeWidth=(int)Image.cols/6;
            resizeHeight=(int)Image.rows/6;
        }break;
        case 6:
        {
            resizeWidth=(int)Image.cols/7;
            resizeHeight=(int)Image.rows/7;
        }break;
        case 7:
        {
            resizeWidth=(int)Image.cols/8;
            resizeHeight=(int)Image.rows/8;
        }break;
        case 8:
        {
            resizeWidth=(int)Image.cols/9;
            resizeHeight=(int)Image.rows/9;
        }break;
        case 9:
        {
            resizeWidth=(int)Image.cols/10;
            resizeHeight=(int)Image.rows/10;
        }break;
        case 10:
        {
            resizeWidth=(int)Image.cols/11;
            resizeHeight=(int)Image.rows/11;
        }break;
        default:
        {
            resizeWidth=(int)Image.cols/12;
            resizeHeight=(int)Image.rows/12;
        }break;
        }

        cv::resize(Image,Image,Size(resizeWidth,resizeHeight),0,0,3);
        images.push_back(Image);
    }
    qDebug("===================================");
    qDebug("Number of image names: %d",imagenames.size());
    qDebug("===================================");
    //    Image=imread(fileName.toStdString());
    for (int i = 0; i < images.size(); i++) {

        cv::Mat image = images[i];
        MBNS_tMatching(image,ROI);
//        MNS_tMatching(image,ROI);
        if (RectUplOutPoint_MBNS.x >0 && RectUplOutPoint_MBNS.y>0)
        {

            int slash_pos = fn[i].find("\\");
            std::string src = fn[i];
            src.replace(slash_pos,1,"/");
            QString temppath = outpath+QString::fromStdString(imagenames[i]);
            //            copyFile(src.c_str(),dest);
            if (QFile::copy(QString::fromStdString(src), temppath)){
                qDebug("===================================");
                qDebug("Image number: %d is good to save",i);
                qDebug("===================================");
                RectUplOutPoint_MBNS.x = 0;
                RectUplOutPoint_MBNS.y = 0;
            }
        }

    }
    //    for (auto &image : images){} // access by reference to avoid copying
}


void MainWindow::on_pushButtonAMSS_clicked()
{
    beta = 3;
    qDebug("BEta is changed: %d",beta);
    datasetpath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    qDebug("Dataset path is: %s",qUtf8Printable(datasetpath));
}

