#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QVector>
#include <cmath>

struct Point {
    double x;
    double y;
    double z;

    // Helper functions for vector math
    inline Point operator*(double a) const {
        return {this->x*a, this->y*a, this->z*a};
    }
    inline Point operator/(double a) const {
        return {this->x/a, this->y/a, this->z/a};
    }
    inline Point operator+(const Point& rhs) const {
        return {this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
    }
    inline Point operator-(const Point& rhs) const {
        return {this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
    }
    inline double dot(const Point& rhs) const {
        return this->x*rhs.x + this->y*rhs.y + this->z*rhs.z;
    }
    inline double length() const {
        return sqrt(lengthSquared());
    }
    inline double lengthSquared() const {
        return x*x + y*y + z*z;
    }
    inline bool operator==(const Point& rhs) const {
        return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
    }
};

inline uint qHash(Point p) {
    uint ui;
    memcpy(&ui, &p, sizeof(ui));
    return ui & 0xFFFFF000;
}



enum class Axis {
    X,
    Y,
    Z,
};

enum class VisualType {
    Trajectory,
    Plan,
    Danger,
    Horizon,
};

struct Horizon {
    double x1, z1;
    double x2, z2;
};

struct InputData {
    double acceptableDeviation;
    QVector<Point> trajectory;
    QVector<Point> plan;
    QVector<Horizon> horizon;
};

#endif // MODEL_H
