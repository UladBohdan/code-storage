#ifndef _RHOMBUS_H
#define _RHOMBUS_H

#include "SymmetricPolygon.h"

class Rhombus: public SymmetricPolygon {
public:
    void SetPoints(QVector<QPoint> points);
};

#endif //_RHOMBUS_H