#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//#include <QpMatView>
#include <opencv2/opencv.hpp>
#include <QGraphicsPixmapItem>
using namespace cv;

#include <QFileDialog>
#include <QGraphicsPixmapItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonTemp_clicked();

    void on_pushButtonROI_clicked();

    void on_pushButtonROI_2_clicked();

    void on_pushButtonMatch_clicked();

    void on_pushButtonROI_3_clicked();

    void on_pushButtonROI_4_clicked();

    void on_pushButtonTestUI_clicked();

    void on_pushButtonSearch_clicked();

    void on_pushButtonAMSS_clicked();

private:
    Ui::MainWindow *ui;

    //定义用于控制mat显示的类指针*mview
//    QpMatView *mview;
//    QPixmap mViewPixelMap;
//    QGraphicsPixmapItem pixmap;
//    QGraphicsPixmapItem pixmap1;
//    QGraphicsPixmapItem pixmap2;
    QGraphicsScene* scene;
};

#endif // MAINWINDOW_H
