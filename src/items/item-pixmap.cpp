/***************************************************************************
**                                                                        **
**  QCustomPlot, a simple to use, modern plotting widget for Qt           **
**  Copyright (C) 2011, 2012 Emanuel Eichhammer                           **
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
**  Website/Contact: http://www.WorksLikeClockwork.com/                   **
**             Date: 09.06.12                                             **
****************************************************************************/

#include "item-pixmap.h"

#include "../painter.h"
#include "../core.h"

// ================================================================================
// =================== QCPItemPixmap
// ================================================================================

/*! \class QCPItemPixmap
  \brief An arbitrary pixmap

  \image html QCPItemPixmap.png "Pixmap example. Blue dotted circles are anchors, solid blue discs are positions."

  It has two positions, \a topLeft and \a bottomRight, which define the rectangle the pixmap will
  be drawn in. Depending on the scale setting (\ref setScaled), the pixmap will be either scaled to
  fit the rectangle or be drawn aligned to the topLeft position.
  
  If scaling is enabled and \a topLeft is further to the bottom/right than \a bottomRight (as shown
  on the right side of the example image), the pixmap will be flipped in the respective
  orientations.
*/

/*!
  Creates a rectangle item and sets default values.
  
  The constructed item can be added to the plot with QCustomPlot::addItem.
*/
QCPItemPixmap::QCPItemPixmap(QCustomPlot *parentPlot) :
  QCPAbstractItem(parentPlot),
  topLeft(createPosition("topLeft")),
  bottomRight(createPosition("bottomRight")),
  top(createAnchor("top", aiTop)),
  topRight(createAnchor("topRight", aiTopRight)),
  right(createAnchor("right", aiRight)),
  bottom(createAnchor("bottom", aiBottom)),
  bottomLeft(createAnchor("bottomLeft", aiBottomLeft)),
  left(createAnchor("left", aiLeft))
{
  topLeft->setCoords(0, 1);
  bottomRight->setCoords(1, 0);
  
  setPen(Qt::NoPen);
  setSelectedPen(QPen(Qt::blue));
  setScaled(false, Qt::KeepAspectRatio);
}

QCPItemPixmap::~QCPItemPixmap()
{
}

/*!
  Sets the pixmap that will be displayed.
*/
void QCPItemPixmap::setPixmap(const QPixmap &pixmap)
{
  mPixmap = pixmap;
}

/*!
  Sets whether the pixmap will be scaled to fit the rectangle defined by the \a topLeft and \a
  bottomRight positions.
*/
void QCPItemPixmap::setScaled(bool scaled, Qt::AspectRatioMode aspectRatioMode)
{
  mScaled = scaled;
  mAspectRatioMode = aspectRatioMode;
  updateScaledPixmap();
}

/*!
  Sets the pen that will be used to draw a border around the pixmap.
  
  \see setSelectedPen, setBrush
*/
void QCPItemPixmap::setPen(const QPen &pen)
{
  mPen = pen;
}

/*!
  Sets the pen that will be used to draw a border around the pixmap when selected
  
  \see setPen, setSelected
*/
void QCPItemPixmap::setSelectedPen(const QPen &pen)
{
  mSelectedPen = pen;
}

/* inherits documentation from base class */
double QCPItemPixmap::selectTest(const QPointF &pos) const
{
  if (!mVisible)
    return -1;
  
  return rectSelectTest(getFinalRect(), pos, true);
}

/* inherits documentation from base class */
void QCPItemPixmap::draw(QCPPainter *painter)
{
  bool flipHorz = false;
  bool flipVert = false;
  QRect rect = getFinalRect(&flipHorz, &flipVert);
  double clipPad = mainPen().style() == Qt::NoPen ? 0 : mainPen().widthF();
  QRect boundingRect = rect.adjusted(-clipPad, -clipPad, clipPad, clipPad);
  if (boundingRect.intersects(clipRect()))
  {
    updateScaledPixmap(rect, flipHorz, flipVert);
    painter->drawPixmap(rect.topLeft(), mScaled ? mScaledPixmap : mPixmap);
    QPen pen = mainPen();
    if (pen.style() != Qt::NoPen)
    {
      painter->setPen(pen);
      painter->setBrush(Qt::NoBrush);
      painter->drawRect(rect);
    }
  }
}

/* inherits documentation from base class */
QPointF QCPItemPixmap::anchorPixelPoint(int anchorId) const
{
  bool flipHorz;
  bool flipVert;
  QRect rect = getFinalRect(&flipHorz, &flipVert);
  // we actually want denormal rects (negative width/height) here, so restore
  // the flipped state:
  if (flipHorz)
    rect.adjust(rect.width(), 0, -rect.width(), 0);
  if (flipVert)
    rect.adjust(0, rect.height(), 0, -rect.height());
  
  switch (anchorId)
  {
    case aiTop:         return (rect.topLeft()+rect.topRight())*0.5;
    case aiTopRight:    return rect.topRight();
    case aiRight:       return (rect.topRight()+rect.bottomRight())*0.5;
    case aiBottom:      return (rect.bottomLeft()+rect.bottomRight())*0.5;
    case aiBottomLeft:  return rect.bottomLeft();
    case aiLeft:        return (rect.topLeft()+rect.bottomLeft())*0.5;;
  }
  
  qDebug() << Q_FUNC_INFO << "invalid anchorId" << anchorId;
  return QPointF();
}

/*! \internal
  
  Creates the buffered scaled image (\a mScaledPixmap) to fit the specified \a finalRect. The
  parameters \a flipHorz and \a flipVert control whether the resulting image shall be flipped
  horizontally or vertically. (This is used when \a topLeft is further to the bottom/right than \a
  bottomRight.)
  
  This function only creates the scaled pixmap when the buffered pixmap has a different size than
  the expected result, so calling this function repeatedly, e.g. in the \ref draw function, does
  not cause expensive rescaling every time.
  
  If scaling is disabled, sets mScaledPixmap to a null QPixmap.
*/
void QCPItemPixmap::updateScaledPixmap(QRect finalRect, bool flipHorz, bool flipVert)
{
  if (mScaled)
  {
    if (finalRect.isNull())
      finalRect = getFinalRect(&flipHorz, &flipVert);
    if (finalRect.size() != mScaledPixmap.size())
    {
      mScaledPixmap = mPixmap.scaled(finalRect.size(), mAspectRatioMode, Qt::SmoothTransformation);
      if (flipHorz || flipVert)
        mScaledPixmap = QPixmap::fromImage(mScaledPixmap.toImage().mirrored(flipHorz, flipVert));
    }
  } else if (!mScaledPixmap.isNull())
    mScaledPixmap = QPixmap();
}

/*! \internal
  
  Returns the final (tight) rect the pixmap is drawn in, depending on the current item positions
  and scaling settings.
  
  The output parameters \a flippedHorz and \a flippedVert return whether the pixmap should be drawn
  flipped horizontally or vertically in the returned rect. (The returned rect itself is always
  normalized, i.e. the top left corner of the rect is actually further to the top/left than the
  bottom right corner). This is the case when the item position \a topLeft is further to the
  bottom/right than \a bottomRight.
  
  If scaling is disabled, returns a rect with size of the original pixmap and the top left corner
  aligned with the item position \a topLeft. The position \a bottomRight is ignored.
*/
QRect QCPItemPixmap::getFinalRect(bool *flippedHorz, bool *flippedVert) const
{
  QRect result;
  bool flipHorz = false;
  bool flipVert = false;
  QPoint p1 = topLeft->pixelPoint().toPoint();
  QPoint p2 = bottomRight->pixelPoint().toPoint();
  if (p1 == p2)
    return QRect(p1, QSize(0, 0));
  if (mScaled)
  {
    QSize newSize = QSize(p2.x()-p1.x(), p2.y()-p1.y());
    QPoint topLeft = p1;
    if (newSize.width() < 0)
    {
      flipHorz = true;
      newSize.rwidth() *= -1;
      topLeft.setX(p2.x());
    }
    if (newSize.height() < 0)
    {
      flipVert = true;
      newSize.rheight() *= -1;
      topLeft.setY(p2.y());
    }
    QSize scaledSize = mPixmap.size();
    scaledSize.scale(newSize, mAspectRatioMode);
    result = QRect(topLeft, scaledSize);
  } else
  {
    result = QRect(p1, mPixmap.size());
  }
  if (flippedHorz)
    *flippedHorz = flipHorz;
  if (flippedVert)
    *flippedVert = flipVert;
  return result;
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemPixmap::mainPen() const
{
  return mSelected ? mSelectedPen : mPen;
}