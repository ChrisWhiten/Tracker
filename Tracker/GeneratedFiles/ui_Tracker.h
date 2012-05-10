/********************************************************************************
** Form generated from reading UI file 'Tracker.ui'
**
** Created: Thu May 10 14:06:13 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRACKER_H
#define UI_TRACKER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackerClass
{
public:
    QAction *actionLoad;
    QAction *actionBeginTracking;
    QWidget *centralWidget;
    QLabel *input_sequence;
    QLabel *output_sequence;
    QLabel *input_label;
    QLabel *output_label;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TrackerClass)
    {
        if (TrackerClass->objectName().isEmpty())
            TrackerClass->setObjectName(QString::fromUtf8("TrackerClass"));
        TrackerClass->resize(600, 400);
        actionLoad = new QAction(TrackerClass);
        actionLoad->setObjectName(QString::fromUtf8("actionLoad"));
        actionBeginTracking = new QAction(TrackerClass);
        actionBeginTracking->setObjectName(QString::fromUtf8("actionBeginTracking"));
        centralWidget = new QWidget(TrackerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        input_sequence = new QLabel(centralWidget);
        input_sequence->setObjectName(QString::fromUtf8("input_sequence"));
        input_sequence->setGeometry(QRect(60, 70, 81, 16));
        output_sequence = new QLabel(centralWidget);
        output_sequence->setObjectName(QString::fromUtf8("output_sequence"));
        output_sequence->setGeometry(QRect(390, 70, 81, 16));
        input_label = new QLabel(centralWidget);
        input_label->setObjectName(QString::fromUtf8("input_label"));
        input_label->setGeometry(QRect(80, 260, 61, 16));
        output_label = new QLabel(centralWidget);
        output_label->setObjectName(QString::fromUtf8("output_label"));
        output_label->setGeometry(QRect(400, 260, 71, 16));
        TrackerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(TrackerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        TrackerClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(TrackerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        TrackerClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad);
        menuFile->addAction(actionBeginTracking);

        retranslateUi(TrackerClass);

        QMetaObject::connectSlotsByName(TrackerClass);
    } // setupUi

    void retranslateUi(QMainWindow *TrackerClass)
    {
        TrackerClass->setWindowTitle(QApplication::translate("TrackerClass", "Tracker", 0, QApplication::UnicodeUTF8));
        actionLoad->setText(QApplication::translate("TrackerClass", "Load", 0, QApplication::UnicodeUTF8));
        actionBeginTracking->setText(QApplication::translate("TrackerClass", "Begin Tracking", 0, QApplication::UnicodeUTF8));
        input_sequence->setText(QApplication::translate("TrackerClass", "Input Sequence", 0, QApplication::UnicodeUTF8));
        output_sequence->setText(QApplication::translate("TrackerClass", "Output Sequence", 0, QApplication::UnicodeUTF8));
        input_label->setText(QApplication::translate("TrackerClass", "Raw Frame", 0, QApplication::UnicodeUTF8));
        output_label->setText(QApplication::translate("TrackerClass", "Tracked Frame", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("TrackerClass", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TrackerClass: public Ui_TrackerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRACKER_H
