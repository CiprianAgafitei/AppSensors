#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>

class ClickableFrame : public QFrame {
    Q_OBJECT
public:
    ClickableFrame(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *) override;
};

#endif // CLICKABLEFRAME_H
