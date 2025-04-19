// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_loadDataButton_clicked();
    void on_buildChartsButton_clicked();
    void on_addDataButton_clicked();
    void on_deleteDataButton_clicked();
    void on_saveDataButton_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    QChartView *chartView1;
    QChartView *chartView2;
    QString currentFileName;

    void buildCharts();
};

#endif // MAINWINDOW_H
