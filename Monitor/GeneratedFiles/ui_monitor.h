/********************************************************************************
** Form generated from reading UI file 'monitor.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONITOR_H
#define UI_MONITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MonitorClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MonitorClass)
    {
        if (MonitorClass->objectName().isEmpty())
            MonitorClass->setObjectName(QString::fromUtf8("MonitorClass"));
        MonitorClass->resize(600, 400);
        centralWidget = new QWidget(MonitorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        MonitorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MonitorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        MonitorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MonitorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MonitorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MonitorClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MonitorClass->setStatusBar(statusBar);

        retranslateUi(MonitorClass);

        QMetaObject::connectSlotsByName(MonitorClass);
    } // setupUi

    void retranslateUi(QMainWindow *MonitorClass)
    {
        MonitorClass->setWindowTitle(QApplication::translate("MonitorClass", "Monitor", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MonitorClass: public Ui_MonitorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITOR_H
