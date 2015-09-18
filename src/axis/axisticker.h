/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.04.15                                             **
**          Version: 1.3.1                                                **
****************************************************************************/

#ifndef QCPAXISTICKER_H
#define QCPAXISTICKER_H

#include "../global.h"
#include "range.h"

class QCPAxisTicker
{
public:
  QCPAxisTicker();
  virtual ~QCPAxisTicker();
  
  // getters:
  int tickCount() const { return mTickCount; }
  double tickOrigin() const { return mTickOrigin; }
  
  // setters:
  void setTickCount(int count);
  void setTickOrigin(double origin);
  
  // introduced virtual methods:
  virtual void generate(const QCPRange &range, const QLocale &locale, QChar formatChar, int precision, QVector<double> &ticks, QVector<double> &subTicks, QVector<QString> &tickLabels);
  
protected:
  // property members:
  int mTickCount;
  double mTickOrigin;
  
  // introduced virtual methods:
  virtual double getTickStep(const QCPRange &range);
  virtual int getSubTickCount(double tickStep);
  virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision);
  virtual QVector<double> createTickVector(double tickStep, const QCPRange &range);
  virtual QVector<double> createSubTickVector(int subTickCount, const QVector<double> &ticks);
  virtual QVector<QString> createLabelVector(const QVector<double> &ticks, const QLocale &locale, QChar formatChar, int precision);
  
  // non-virtual methods:
  void trimTicks(const QCPRange &range, QVector<double> &ticks, bool keepOneOutlier) const;
  double pickClosest(double target, const QVector<double> &candidates) const;
  double getMantissa(double input, double *magnitude=0) const;
  double cleanMantissa(double input) const;
};

#endif // QCPAXISTICKER_H
