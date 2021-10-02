#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QChart>
#include "model.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void draw(const InputData& d, const QSet<Point>& dangerPoint, const QSet<Point>& dangerSegmentStart, int stopAtPoint = -1);
private slots:
    void on_processBtn_clicked();
    void on_loadDataBtn_clicked();

    void drawNext();
private:
    Ui::MainWindow *ui;

    int _drawUpTo = 0;

    InputData _data;
    QSet<Point> _dangerSegmentStart;
    QSet<Point> _dangerPoints;
};
#endif // MAIN_H
