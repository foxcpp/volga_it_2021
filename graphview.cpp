#include "graphview.h"
#include <QValueAxis>
#include <QDebug>

GraphView::GraphView(QWidget* parent) : QtCharts::QChartView(parent) {
    this->setChart(&_chart);
    this->setRubberBand(QChartView::RectangleRubberBand);
    _chart.legend()->setVisible(false);
    this->setRenderHint(QPainter::Antialiasing, true);
}

static QString axisTitle(Axis index) {
    switch (index) {
    case Axis::X:
        return QStringLiteral("X");
    case Axis::Y:
        return QStringLiteral("Y");
    case Axis::Z:
        return QStringLiteral("Z");
    }
    Q_UNREACHABLE();
}

static double selectCoordinate(Point p, Axis axis) {
    switch (axis) {
    case Axis::X:
        return p.x;
    case Axis::Y:
        return p.y;
    case Axis::Z:
        return p.z;
    }
    Q_UNREACHABLE();
}

void GraphView::setAxis(Axis vertical, Axis horizontal) {
    if (_vertAxis != nullptr) {
        _chart.removeAxis(_vertAxis);
        delete _vertAxis; _vertAxis = nullptr;
        _chart.removeAxis(_horiAxis);
        delete _horiAxis; _horiAxis = nullptr;
    }

    _chart.addAxis(_vertAxis = new QValueAxis(), Qt::AlignLeft);
    _vertAxis->setMin(0);
    _vertAxis->setMax(100);
    _vertAxis->setTitleText(axisTitle(vertical));
    _chart.addAxis(_horiAxis = new QValueAxis(), Qt::AlignBottom);
    _horiAxis->setMin(0);
    _horiAxis->setMax(100);
    _horiAxis->setTitleText(axisTitle(horizontal));

    _vertical = vertical;
    _horizontal = horizontal;
}

void GraphView::setType(VisualType type, QColor pointColor, QColor lineColor) {
    _pointColor.insert(type, pointColor);
    _lineColor.insert(type, lineColor);
}

void GraphView::addPoint(VisualType type, Point point) {
    auto seriesColor = _pointColor.find(type);
    if (seriesColor == _pointColor.end()) {
        return;
    }

    auto series = _points.find(type);
    if (series == _points.end()) {
        auto points = new QScatterSeries();
        _chart.addSeries(points);
        points->setColor(*seriesColor);
        points->attachAxis(_vertAxis);
        points->attachAxis(_horiAxis);
        series = _points.insert(type, points);
    }

    double first = selectCoordinate(point, _horizontal);
    double second = selectCoordinate(point, _vertical);

    if (first < minHorizontal) minHorizontal = first;
    if (first > maxHorizontal) maxHorizontal = first;
    if (second < minVertical) minVertical = second;
    if (second > maxVertical) maxVertical = second;

    series.value()->append(first, second);
}

void GraphView::addLine(VisualType type, Point start, Point end) {
    auto seriesColor = _lineColor.find(type);
    if (seriesColor == _lineColor.end()) {
        return;
    }

    auto series = new QLineSeries();
    _chart.addSeries(series);
    series->append(
        selectCoordinate(start, _horizontal),
        selectCoordinate(start, _vertical)
    );
    series->append(
        selectCoordinate(end, _horizontal),
        selectCoordinate(end, _vertical)
    );
    series->setColor(*seriesColor);
    series->attachAxis(_horiAxis);
    series->attachAxis(_vertAxis);
}

void GraphView::clear() {
    _chart.removeAllSeries();
    _points.clear();
}

void GraphView::resizeToFit() {
    this->_horiAxis->setMin(minHorizontal*1.10);
    this->_horiAxis->setMax(maxHorizontal*1.10);
    this->_vertAxis->setMin(minVertical*1.10);
    this->_vertAxis->setMax(maxVertical*1.10);
}

int GraphView::heightForWidth(int w) const {
    return w;
}

