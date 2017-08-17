#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "avmanager.h"
#include "readthread.h"
#include "audiothread.h"
#include "videothread.h"
#include "myopenglwidget.h"
#include <QLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


protected:
    void keyPressEvent(QKeyEvent*event);

};

#endif // MAINWINDOW_H
