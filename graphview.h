#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QMap>
#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include "model.h"

using namespace QtCharts;

/* Widget based on QChartView that displays 2D projection of points and lines added
 * using addPoint and addLine methods.
 *
 * Before using the widget, setAxis should be called to determine projection plane
 * (e.g. X;Y, X;Z or whatever).
 * Then setType should be called for each VisualType that should be displayed
 * on this view. If setType is never called for a particular value -
 * addPoint and addLine calls for it will be ignored.
 *
 * addPoint should always be used after corresponding addLine to ensure
 * correct drawing z-order.
 *
 * clear should be called if it is necessary to remove all items from the
 * GraphView.
 */
class GraphView : public QChartView
{
    Q_OBJECT
public:
    GraphView(QWidget* parent = nullptr);

    void setAxis(Axis vertical, Axis horizontal);
    void setType(VisualType type, QColor pointColor, QColor lineColor);
    void addPoint(VisualType type, Point point);
    void addLine(VisualType type, Point start, Point end);
    void clear();
    void resizeToFit();

    // XXX: not working?
    int heightForWidth(int w) const override;
private:
    QChart _chart;

    double maxVertical = 0, minVertical = 0;
    double maxHorizontal = 0, minHorizontal = 0;

    Axis _vertical, _horizontal;

    QMap<VisualType, QColor> _lineColor;
    QMap<VisualType, QColor> _pointColor;

    // everything * is owned by QChart
    QMap<VisualType, QScatterSeries*> _points;
    QValueAxis* _vertAxis = nullptr;
    QValueAxis* _horiAxis = nullptr;
};

#endif // GRAPHVIEW_H
