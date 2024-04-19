#include "clickableframe.h"

ClickableFrame::ClickableFrame(QWidget *parent) : QFrame(parent) {}

void ClickableFrame::mousePressEvent(QMouseEvent *) {
    emit clicked();
}
