/********************************************************************************
** Form generated from reading UI file 'monitorreplaywidget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONITORREPLAYWIDGET_H
#define UI_MONITORREPLAYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MonitorReplayWidget
{
public:

    void setupUi(QWidget *MonitorReplayWidget)
    {
        if (MonitorReplayWidget->objectName().isEmpty())
            MonitorReplayWidget->setObjectName(QStringLiteral("MonitorReplayWidget"));
        MonitorReplayWidget->resize(400, 300);

        retranslateUi(MonitorReplayWidget);

        QMetaObject::connectSlotsByName(MonitorReplayWidget);
    } // setupUi

    void retranslateUi(QWidget *MonitorReplayWidget)
    {
        MonitorReplayWidget->setWindowTitle(QApplication::translate("MonitorReplayWidget", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MonitorReplayWidget: public Ui_MonitorReplayWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITORREPLAYWIDGET_H
