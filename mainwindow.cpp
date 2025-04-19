#include "mainwindow.h"
#include "qheaderview.h"
#include "qpieseries.h"
#include "qstandarditemmodel.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPieSlice>
#include <QLocale>
#include <QColorDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    model->setHorizontalHeaderLabels({"Группа", "Курс", "Фамилия", "Имя", "Отчество", "Стипендия", "Староста", "Задолженность", "Куратор"});
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadDataButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) return;

    currentFileName = fileName;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        model->removeRows(0, model->rowCount());
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList values = line.split(' ', Qt::SkipEmptyParts);
            if (values.size() != 9) continue;
            QList<QStandardItem*> items;
            for (const QString &value : values) {
                items.append(new QStandardItem(value));
            }
            model->appendRow(items);
        }
        file.close();
        buildCharts();
    }
}

void MainWindow::on_buildChartsButton_clicked()
{
    if (model->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите данные.");
        return;
    }
    buildCharts();
}

void MainWindow::buildCharts()
{
    int total = 0, thirdYear = 0;
    QMap<QString, int> courseCount;

    for (int row = 0; row < model->rowCount(); ++row) {
        QString course = model->item(row, 1)->text();
        total++;
        courseCount[course]++;
        if (course == "3") thirdYear++;
    }

    if (total == 0) return;

    QPieSeries *series1 = new QPieSeries();
    if (thirdYear > 0) {
        qreal percent3 = 100.0 * thirdYear / total;
        auto slice3 = series1->append("3 курс (" + QString::number(percent3, 'f', 1) + "%)", thirdYear);
        slice3->setLabelVisible(true);
    }
    if (thirdYear < total) {
        qreal percentOther = 100.0 * (total - thirdYear) / total;
        auto sliceOther = series1->append("Остальные (" + QString::number(percentOther, 'f', 1) + "%)", total - thirdYear);
        sliceOther->setLabelVisible(true);
    }

    QChart *chart1 = new QChart();
    chart1->addSeries(series1);
    chart1->setTitle("Процент студентов 3 курса");
    chart1->setAnimationOptions(QChart::QChart::AllAnimations);
    chart1->legend()->setVisible(true);
    chart1->legend()->setAlignment(Qt::AlignRight);

    QPieSeries *series2 = new QPieSeries();
    for (auto it = courseCount.begin(); it != courseCount.end(); ++it) {
        auto slice = series2->append("Курс " + it.key(), it.value());
        slice->setLabelVisible(true);
    }

    QChart *chart2 = new QChart();
    chart2->addSeries(series2);
    chart2->setTitle("Распределение студентов по курсам");
    chart2->setAnimationOptions(QChart::QChart::AllAnimations);
    chart2->legend()->setVisible(true);
    chart2->legend()->setAlignment(Qt::AlignRight);

    QLayoutItem *child;
    while ((child = ui->chartLayout1->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    while ((child = ui->chartLayout2->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    ui->chartLayout1->addWidget(new QChartView(chart1));
    ui->chartLayout2->addWidget(new QChartView(chart2));
}

void MainWindow::on_addDataButton_clicked()
{
    QStringList labels = model->horizontalHeaderItem(0)->model()->headerData(0, Qt::Horizontal).toStringList();
    QString prompt = "Введите данные через пробел (9 полей): Группа Курс Фамилия Имя Отчество Стипендия Староста Задолженность Куратор";
    QString text = QInputDialog::getText(this, "Добавить данные", prompt);
    QStringList values = text.split(" ", Qt::SkipEmptyParts);
    if (values.size() != 9) {
        QMessageBox::warning(this, "Ошибка", "Ожидалось 9 значений через пробел.");
        return;
    }
    QList<QStandardItem*> items;
    for (const QString &v : values)
        items.append(new QStandardItem(v));
    model->appendRow(items);
    buildCharts();
}

void MainWindow::on_deleteDataButton_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid()) {
        model->removeRow(index.row());
        buildCharts();
    } else {
        QMessageBox::information(this, "Удаление", "Выберите строку для удаления.");
    }
}

void MainWindow::on_saveDataButton_clicked()
{
    if (currentFileName.isEmpty()) {
        currentFileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "Text Files (*.txt)");
        if (currentFileName.isEmpty()) return;
    }

    QFile file((currentFileName));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        for (int i = 0; i < model->rowCount(); ++i) {
            QStringList line;
            for (int j = 0; j < model->columnCount(); ++j) {
                line << model->item(i, j)->text();
            }
            out << line.join(" ") << "\n";
        }
        file.close();
    }
}
