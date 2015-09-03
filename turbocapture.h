#ifndef TURBOCAPTURE_H
#define TURBOCAPTURE_H


#include <gphoto2/gphoto2-camera.h>

#include <QMainWindow>


#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTreeWidget>
#include <QDialog>
#include <QString>

namespace Ui {
class TurBoCapture;
}

class TurBoCapture : public QMainWindow
{
    Q_OBJECT

public:
    explicit TurBoCapture(QWidget *parent = 0);
    ~TurBoCapture();

private slots:

    static void ctx_error_func (GPContext *context, const char *str, void *data);

    static void ctx_status_func (GPContext *context, const char *str, void *data);

    int capture (const char *filename);

    void stich(const char *fileA, const char *fileB, const int ori );

    void stichv2(const char *fileA, const char *fileB, const int ori);

    void on_quitButton_clicked();

    void on_horizontalSpinBox_valueChanged(double arg1);

    void on_verticalSpinBox_valueChanged(double arg1);

    void on_takeFrameButton_clicked();

    void on_compileButton_clicked();

    void on_newRowButton_clicked();

private:
    Ui::TurBoCapture *ui;


    int ret;

    Camera *camera;
    GPContext *context;

    int width;
    int height;

    double horizontal;
    double vertical;

    int colsCount;
    int rowsCount;

    QGraphicsScene *scene;
    QGraphicsLineItem *h_line1;
    QGraphicsLineItem *h_line2;
    QGraphicsLineItem *v_line1;
    QGraphicsLineItem *v_line2;
    QGraphicsTextItem *text;


    QTreeWidgetItem *pic_cols;
    QTreeWidgetItem *pic_rows;

    void addTreeRoot(QString name);
    void addTreeChild(QTreeWidgetItem *parent,
                      QString name);

};

#endif // TURBOCAPTURE_H
