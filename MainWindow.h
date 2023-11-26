#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GlobalAbilityList.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_simButton_clicked();
    
private:
    Ui::MainWindow *ui;
    GlobalAbilityList *gal = nullptr;
};

#endif // MAINWINDOW_H
