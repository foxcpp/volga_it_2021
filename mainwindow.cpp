#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QChartView>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <cmath>
#include <algorithm>
#include <exception>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->xyChart->setAxis(Axis::X, Axis::Y);
    ui->xyChart->setType(VisualType::Trajectory, Qt::black, Qt::black);
    ui->xyChart->setType(VisualType::Danger, Qt::red, Qt::red);
    ui->xyChart->setType(VisualType::Plan, Qt::blue, Qt::blue);
    ui->xyChart->chart()->setTitle(QStringLiteral("Вид сверху"));

    ui->xzChart->setAxis(Axis::X, Axis::Z);
    ui->xzChart->setType(VisualType::Trajectory, Qt::black, Qt::black);
    ui->xzChart->setType(VisualType::Danger, QColor(255, 40, 40), QColor(255, 40, 40));
    ui->xzChart->setType(VisualType::Plan, Qt::blue, Qt::blue);
    ui->xzChart->setType(VisualType::Horizon, Qt::green, Qt::green);
    ui->xzChart->chart()->setTitle(QStringLiteral("Вид сбоку"));
}

// https://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
bool intersectXZ(Point start1, Point end1, Point start2, Point end2) {
    auto ccw = [](Point a, Point b, Point c) -> bool {
        return (c.z - a.z) * (b.x - a.x) > (b.z - a.z) * (c.x - a.x);
    };
    return ccw(start1,start2,end2) != ccw(end1,start2,end2) && ccw(start1,end1,start2) != ccw(start1,end1,end2);
}

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
double distance(Point segStart, Point segEnd, Point point) {
    // Return minimum distance between line segment vw and point p
    const float l2 = (segEnd - segStart).lengthSquared();  // i.e. |w-v|^2 -  avoid a sqrt
    if (l2 == 0.0) return (point - segStart).length();   // v == w case
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    // We clamp t from [0,1] to handle points outside the segment vw.
    const double t = std::max(0.0, std::min(1.0, (point - segStart).dot(segEnd - segStart) / l2));
    const Point projection = segStart + (segEnd - segStart) * t;  // Projection falls on the segment
    return (point - projection).length();
}

void MainWindow::draw(const InputData& d, const QSet<Point>& dangerPoint, const QSet<Point>& dangerSegmentStart, int stopAtPoint) {
    // Due to QtChart mess we have a specific drawing order - always draw points
    // after lines. This means we have to always
    // redraw everything from scratch.
    ui->xzChart->clear();
    ui->xyChart->clear();

    for (const auto& h : d.horizon) {
        ui->xzChart->addLine(VisualType::Horizon,
                             {h.x1, 0, h.z1},
                             {h.x2, 0, h.z2});
    }
    for (const auto& h : d.horizon) {
        ui->xzChart->addPoint(VisualType::Horizon, Point{h.x1, 0, h.z1});
        ui->xzChart->addPoint(VisualType::Horizon, Point{h.x2, 0, h.z2});
    }

    for (int i = 0; i < d.plan.size() - 1; ++i) {
        ui->xyChart->addLine(VisualType::Plan,
                             d.plan[i], d.plan[i+1]);
        ui->xzChart->addLine(VisualType::Plan,
                             d.plan[i], d.plan[i+1]);
    }
    for (int i = 0; i < d.plan.size(); ++i) {
        ui->xyChart->addPoint(VisualType::Plan, d.plan[i]);
        ui->xzChart->addPoint(VisualType::Plan, d.plan[i]);
    }

    for (int i = 0; i < d.trajectory.size() - 1; ++i) {
        if (stopAtPoint >= 0 && i + 1 > stopAtPoint) break;
        VisualType t = VisualType::Trajectory;
        if (_dangerSegmentStart.contains(d.trajectory[i])) t = VisualType::Danger;

        ui->xyChart->addLine(t, d.trajectory[i], d.trajectory[i+1]);
        ui->xzChart->addLine(t, d.trajectory[i], d.trajectory[i+1]);
    }
    for (int i = 0; i < d.trajectory.size(); ++i) {
        if (stopAtPoint >= 0 && i > stopAtPoint) break;
        VisualType t = VisualType::Trajectory;
        if (_dangerPoints.contains(d.trajectory[i])) t = VisualType::Danger;

        ui->xyChart->addPoint(t, d.trajectory[i]);
        ui->xzChart->addPoint(t, d.trajectory[i]);
    }

    ui->xyChart->resizeToFit();
    ui->xzChart->resizeToFit();
}

MainWindow::~MainWindow() {
    delete ui;
}

static InputData readData(QString path) {
    QFile inputFile(path);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(inputFile.errorString().toStdString().c_str());
    }
    QTextStream in(&inputFile);
    int traj, plan, horizon;
    in >> traj >> plan >> horizon;
    if (in.status() != QTextStream::Ok) throw std::runtime_error("Malformed data");
    InputData d;
    in >> d.acceptableDeviation;
    for (int i = 0; i < traj; ++i) {
        Point p;
        in >> p.x >> p.y >> p.z;
        if (in.status() != QTextStream::Ok) throw std::runtime_error("Malformed data");
        d.trajectory.append(p);
    }
    for (int i = 0; i < plan; ++i) {
        Point p;
        in >> p.x >> p.y >> p.z;
        if (in.status() != QTextStream::Ok) throw std::runtime_error("Malformed data");
        d.plan.append(p);
    }
    for (int i = 0; i < horizon; ++i) {
        Horizon h;
        in >> h.x1 >> h.z1 >> h.x2 >> h.z2;
        if (in.status() != QTextStream::Ok) throw std::runtime_error("Malformed data");
        d.horizon.append(h);
    }

    return d;
}

static std::tuple<QSet<Point>, QSet<Point>> calculateDangerInfo(const InputData& d) {
    QSet<Point> points, segmentStart;
    for (int i = 0; i < d.trajectory.size(); ++i) {
        if (i != d.trajectory.size() - 1) {
            for (const auto& h : d.horizon) {
                bool is = intersectXZ(d.trajectory[i], d.trajectory[i+1],
                        {h.x1, 0, h.z1}, {h.x2, 0, h.z2});
                if (is) {
                    segmentStart.insert(d.trajectory[i]);
                    break;
                }
            }
        }

        double minDist = 100000000;
        for (int j = 0; j < d.plan.size() - 1; ++j) {
            const double dist = distance(d.plan[j], d.plan[j+1], d.trajectory[i]);
            if (dist < minDist) minDist = dist;
        }
        if (minDist > d.acceptableDeviation) points.insert(d.trajectory[i]);
    }

    return {points, segmentStart};
}

void MainWindow::drawNext() {
    this->draw(_data, _dangerPoints, _dangerSegmentStart, _drawUpTo);
    if (_drawUpTo == _data.trajectory.size() - 1) {
        this->ui->processBtn->setEnabled(true);
        this->ui->statusLabel->setText(QStringLiteral(""));
        return;
    }
    _drawUpTo++;
    QTimer::singleShot(250, this, &MainWindow::drawNext);
}

void MainWindow::on_processBtn_clicked() {
    this->ui->xyChart->clear();
    this->ui->xzChart->clear();
    _drawUpTo = 0;

    this->ui->processBtn->setEnabled(false);
    this->ui->statusLabel->setText(QStringLiteral("Пошаговая отрисовка..."));

    this->drawNext();
}

void MainWindow::on_loadDataBtn_clicked() {
    auto file = QFileDialog::getOpenFileName(this);
    try {
        this->ui->statusLabel->setText(QStringLiteral("Чтение данных...."));
        QApplication::processEvents();
        auto data = readData(file);
        _data = std::move(data);

        this->ui->statusLabel->setText(QStringLiteral("Вычисление тревожных точек, отрезков...."));
        QApplication::processEvents();
        std::tie(_dangerPoints, _dangerSegmentStart) = calculateDangerInfo(_data);
        this->draw(_data, _dangerPoints, _dangerSegmentStart);

        this->ui->statusLabel->setText(QLatin1String(""));
    } catch (std::exception& ex) {
        QMessageBox::critical(this, "Ошибка чтения файла", ex.what(),
                              QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    }
}

