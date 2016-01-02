#include <node-edit.h>

namespace Coffee{
namespace CQt{

CQNodeItem::CQNodeItem(const QRectF &b, QGraphicsItem *p):
    QGraphicsRectItem(b,p),
    m_slotRadius(4.0)
{
    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsPanel);
}

void CQNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rec = this->rect();

    //Define header section
    QRectF headerSection;
    headerSection.setTop(rec.top());
    headerSection.setLeft(rec.left());
    headerSection.setRight(rec.right());
    headerSection.setBottom(rec.top()
                            +painter->font().pointSizeF()
                            *(1+std::max<qreal>(m_headerSpacing,0)));

    QRectF contentSection = rec;
    contentSection.setTop(headerSection.bottom()-m_padding);

    //Draw background
    QBrush bgBrush;
    bgBrush.setColor(m_bgColor);
    bgBrush.setStyle(Qt::SolidPattern);
    painter->setPen(m_edgeColor);
    painter->setBrush(bgBrush);
    painter->drawRoundRect(contentSection,m_padding,m_padding);

    //Draw header background
    QLinearGradient hdGradient(headerSection.topLeft(),
                               headerSection.bottomLeft());
    hdGradient.setColorAt(0.0, m_hdColor);
    hdGradient.setColorAt(0.9, m_edgeColor);
    QBrush hdBrush(hdGradient);
    painter->setBrush(hdBrush);
    painter->setPen(m_edgeColor);
    painter->drawRoundRect(headerSection,m_padding,m_padding);

    //Text will be drawn in a padded region, makes it prettier
    //Text will not be rendered outside the box.
    headerSection.setLeft(rec.left()+std::max(m_padding,0));
    headerSection.setRight(rec.right()-std::max(m_padding,0));

    //We align in vertically to the center and to the left, no wrapping allowed
    QTextOption opts;
    opts.setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    opts.setWrapMode(QTextOption::NoWrap);
    opts.setUseDesignMetrics(true);

    painter->setPen(m_textColor);
    painter->drawText(headerSection,m_text,opts);

    //Virtual function, can be reimplemented for different node types
    drawContent(painter);
}

QRectF CQNodeItem::boundingRect() const
{
    QRectF b(this->rect());
    b.adjust(-m_slotRadius,0,m_slotRadius,0);
    return b;
}

QPointF CQNodeItem::drawSlot(QPainter *painter, const QRectF &header,
                                bool isSlot, qreal radius,
                                const QString &label, bool connected, int i)
{
    return QPointF(0,0);
}

void CQNodeItem::drawContent(QPainter* painter)
{
    if(m_contentSize.isEmpty())
        return;

    QRectF hdr(headerSection(painter));
    //Draw content region, where we will later draw textures and similar
    QRectF contentRegion(hdr);
    contentRegion.setLeft(hdr.center().x()
                          -m_contentSize.width()/2);
    contentRegion.setRight(hdr.center().x()
                           +m_contentSize.width()/2);
    contentRegion.setTop(hdr.bottom());
    contentRegion.setBottom(hdr.bottom()
                            +m_contentSize.height());

    QBrush contentBrush;
    contentBrush.setStyle(Qt::DiagCrossPattern);
    contentBrush.setColor(m_hdColor);

    painter->setBrush(contentBrush);
    painter->drawRect(contentRegion);
}

QColor CQNodeItem::textColor() const
{
    return m_textColor;
}

void CQNodeItem::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}

QColor CQNodeItem::headerColor() const
{
    return m_hdColor;
}

void CQNodeItem::setHeaderColor(const QColor &hdColor)
{
    m_hdColor = hdColor;
}

QColor CQNodeItem::backgroundColor() const
{
    return m_bgColor;
}

void CQNodeItem::setBackgroundColor(const QColor &bgColor)
{
    m_bgColor = bgColor;
}

int CQNodeItem::padding() const
{
    return m_padding;
}

void CQNodeItem::setPadding(int padding)
{
    m_padding = padding;
}

qreal CQNodeItem::headerSpacing() const
{
    return m_headerSpacing;
}

void CQNodeItem::setHeaderSpacing(const qreal &headerSpacing)
{
    m_headerSpacing = headerSpacing;
}

QString CQNodeItem::text() const
{
    return m_text;
}

void CQNodeItem::setText(const QString &text)
{
    m_text = text;
}

QColor CQNodeItem::edgeColor() const
{
    return m_edgeColor;
}

void CQNodeItem::setEdgeColor(const QColor &edgeColor)
{
    m_edgeColor = edgeColor;
}

QSizeF CQNodeItem::contentSize() const
{
    return m_contentSize;
}

void CQNodeItem::setContentSize(const QSizeF &contentSize)
{
    m_contentSize = contentSize;
}

QRectF CQNodeItem::headerSection(QPainter* painter) const
{
    QRectF rec = this->rect();

    QRectF headerSection;
    headerSection.setTop(rec.top());
    headerSection.setLeft(rec.left());
    headerSection.setRight(rec.right());
    headerSection.setBottom(rec.top()
                            +painter->font().pointSizeF()
                            *(1.0+std::max<qreal>(m_headerSpacing,0)));
    return headerSection;
}

qreal CQNodeItem::slotRadius() const
{
    return m_slotRadius;
}

void CQNodeItem::setSlotRadius(const qreal &slotRadius)
{
    m_slotRadius = slotRadius;
}

CQNodeItem::CQNodeConnection::CQNodeConnection(const QString &label, bool input, CQNodeItem *parent):
    QGraphicsEllipseItem(parent),
    m_parent(parent),
    m_label(label),
    m_input(input)
{
}

void CQNodeItem::CQNodeConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->translate(m_parent->headerSection(painter).topLeft());

    qreal radius = m_parent->m_slotRadius;
    QRectF header = m_parent->headerSection(painter);
    QRectF ellipseRegion;

    qreal xbase = 0;
    qreal ybase = 0;

    if(m_input)
        xbase = 0;
    else
        xbase = header.width();

    ybase = header.height()
            + m_parent->contentSize().height()
            + m_parent->m_padding*6
            + m_index*(m_parent->m_padding+radius*2);

    ellipseRegion.setTop(ybase);
    ellipseRegion.setBottom(ellipseRegion.top()+radius*2);
    ellipseRegion.setLeft(xbase-radius);
    ellipseRegion.setRight(xbase+radius);

    painter->setPen(m_parent->m_bgColor);
    if(m_input)
        painter->setBrush(m_parent->m_bgColor.lighter(250));
    else
        painter->setBrush(m_parent->m_bgColor.darker());

    painter->drawEllipse(ellipseRegion);

    QRectF textRegion(ellipseRegion);
    textRegion.setTop(ybase-painter->fontMetrics().height()/2);
    textRegion.setBottom(ybase+painter->fontMetrics().height()/2);
    if(m_input)
    {
        textRegion.setLeft(ellipseRegion.right()+m_parent->m_padding);
        textRegion.setRight(ellipseRegion.right()
                            +painter->fontMetrics().width(m_label)
                            +m_parent->m_padding);
    }else{
        textRegion.setRight(ellipseRegion.left()
                            -m_parent->m_padding);
        textRegion.setLeft(ellipseRegion.left()
                           -painter->fontMetrics().width(m_label)
                           -m_parent->m_padding);
    }

    this->setRect(textRegion.united(ellipseRegion));

    painter->setPen(m_parent->m_textColor);

    painter->drawText(textRegion,m_label);

    painter->resetTransform();
}

QPainterPath CQNodeItem::CQNodeConnection::shape() const
{
    QPainterPath p;
    p.addEllipse(ellipseRegion());
    return p;
}

void CQNodeItem::CQNodeConnection::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    qDebug("Drag!");
}

void CQNodeItem::CQNodeConnection::dropEvent(QGraphicsSceneDragDropEvent *event)
{

}

void CQNodeItem::CQNodeConnection::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("Click!");
    m_activated = !m_activated;
}

int CQNodeItem::CQNodeConnection::index() const
{
    return m_index;
}

void CQNodeItem::CQNodeConnection::setIndex(int index)
{
    m_index = index;
}

QRectF CQNodeItem::CQNodeConnection::ellipseRegion() const
{
    QPointF base = basePoint();
    QRectF ellipseRegion;
    ellipseRegion.setTop(base.y());
    ellipseRegion.setBottom(ellipseRegion.top()+m_parent->slotRadius()*2);
    ellipseRegion.setLeft(base.x()-m_parent->slotRadius());
    ellipseRegion.setRight(base.x()+m_parent->slotRadius());
    return ellipseRegion;
}

QPointF CQNodeItem::CQNodeConnection::basePoint() const
{
    qreal xbase = 0;
    qreal ybase = 0;
    if(!m_input)
        xbase = m_parent->rect().width();
    ybase = QFont().pointSizeF()
            +m_parent->padding()*6
            +m_parent->contentSize().height()
            +m_index*(m_parent->padding()+m_parent->slotRadius());
    return QPointF(xbase,ybase);
}

CQNodeItem::CQNodeCurve::CQNodeCurve(CQNodeItem::CQNodeConnection *src, CQNodeItem::CQNodeConnection *trg, QGraphicsItem *parent):
    QGraphicsPathItem(parent),
    m_src(src),
    m_trg(trg)
{
}

QRectF CQNodeItem::CQNodeCurve::boundingRect() const
{
    QPointF s = mapFromItem(m_src,0,0);
    QPointF t = mapFromItem(m_trg,0,0);
    QRectF r;
    r.setTopLeft(s);
    r.setBottomRight(t);
    return r;
}

QPainterPath CQNodeItem::CQNodeCurve::shape() const
{
    return curvePath();
}

QPainterPath CQNodeItem::CQNodeCurve::curvePath() const
{
    QPainterPath curve;
    curve.arcMoveTo(boundingRect(),1000);
    return curve;
}

void CQNodeItem::CQNodeCurve::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QColor color = Qt::red;
    int width = 5;

    QPen p;
    p.setWidth(width);
    p.setColor(color);
    p.setCapStyle(Qt::RoundCap);
    painter->setPen(p);

    painter->drawPath(curvePath());
}

}
}
