#ifndef COFFEE_EXT_QT_SHIM_QT_NODE_EDITOR_NODE_EDIT_H
#define COFFEE_EXT_QT_SHIM_QT_NODE_EDITOR_NODE_EDIT_H

#include <peripherals/stl/types.h>
#include <coffee/core/types/cdef/memtypes.h>

#include <QtCore/QDebug>
#include <QtCore/QRectF>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsRectItem>

#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QPainter>

namespace Coffee{
namespace CQt{

struct CNodeItem
{
    struct CInputProp
    {
        cstring label;
    };
    struct COutputProp
    {
        cstring label;
    };
    struct CConnection
    {
        cstring label;
        CNodeItem* source;
        COutputProp* binding;
    };

    /*!
     * \brief Acquire all backward connections to nodes. (Backward chaining only)
     * \return A constant reference to the list of connections.
     */
    virtual const Vector<CConnection>& connections() = 0;
    /*!
     * \brief Label of the node, displayed at top
     * \return
     */
    virtual const CString& label() = 0;
    /*!
     * \brief Data which will be displayed inside the node. May be an image, audio data, 3D mesh, etc. MIME is used to identify the data type, custom types are allowed.
     * \return
     */
    virtual const CMimeData& displayData() = 0;
};

class CQNodeItem : public QGraphicsRectItem
{
public:
    CQNodeItem(const QRectF& b,QGraphicsItem* p);

    //Inherited
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF boundingRect() const;

    //Class-defined
    QPointF drawSlot(QPainter* painter,
                        const QRectF& header, bool isConnection, qreal radius,
                        const QString& label, bool connected, int i);

    virtual void drawContent(QPainter *painter);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

    QColor headerColor() const;
    void setHeaderColor(const QColor &hdColor);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &backgroundColor);

    int padding() const;
    void setPadding(int padding);

    qreal headerSpacing() const;
    void setHeaderSpacing(const qreal &headerSpacing);

    QString text() const;
    void setText(const QString &text);

    QColor edgeColor() const;
    void setEdgeColor(const QColor &edgeColor);

    QSizeF contentSize() const;
    void setContentSize(const QSizeF &contentSize);

    QRectF headerSection(QPainter *painter) const;

    qreal slotRadius() const;
    void setSlotRadius(const qreal &slotRadius);

    class CQNodeConnection : public QGraphicsEllipseItem
    {
    public:
        CQNodeConnection(const QString& label, bool input, CQNodeItem* parent);
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

        QPainterPath shape() const;
        int index() const;
        void setIndex(int index);

        QRectF ellipseRegion() const;

        QPointF basePoint() const;

    protected:
        void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
        void dropEvent(QGraphicsSceneDragDropEvent *event);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

        CQNodeItem* m_parent;
        QString m_label;
        int m_index;
        bool m_input;
        bool m_connected;
        bool m_activated;
    };

    class CQNodeCurve : public QGraphicsPathItem
    {
    public:
        CQNodeCurve(CQNodeConnection* src, CQNodeConnection* trg,QGraphicsItem* parent);
        QRectF boundingRect() const;
        QPainterPath shape() const;
        QPainterPath curvePath() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    private:
        CQNodeConnection* m_src;
        CQNodeConnection* m_trg;
    };

protected:
    int m_padding;
    qreal m_slotRadius;
    qreal m_headerSpacing;
    QColor m_textColor;
    QColor m_hdColor;
    QColor m_bgColor;
    QColor m_edgeColor;
    QString m_text;
    QSizeF m_contentSize;
};

class CQNodeEditor : public QGraphicsScene
{
public:
    void insertNode(CQNodeItem* it)
    {
        this->addItem(it);
    }

protected:
    void drawBackground(QPainter *painter, const QRectF &rect)
    {
        painter->fillRect(rect,Qt::darkGray);
    }
};

}
}

#endif
