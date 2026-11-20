#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QRadioButton>
#include <QListWidget>
#include <QGridLayout>
#include <QTimer>
#include <vector>
#include "ProjectHeaders.h"
using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = NULL);
    ~MainWindow();

private slots:
    void createServers();
    void generateRequests();
    void failServer();
    void recoverServer();
    void applyScaling();
    void saveSummary();
    void nextSecond();

private:
    Logger* logger;
    LoadBalancer* lb;
    QTimer* timer;
    int shownLines;           // log lines already shown in the list
    vector<int> shownIds;     // server ids in the drop-down list

    // left side
    QSpinBox* serverCount;
    QLineEdit* capacityInput;
    QSpinBox* requestCount;
    QComboBox* serverList;
    QRadioButton* autoMode;
    QRadioButton* verticalMode;
    QRadioButton* horizontalMode;
    QSpinBox* stepInput;

    // right side
    QLabel* algoName;
    QLabel* algoReason;
    QLabel* utilLabel;
    QLabel* scaleInfo;
    QLabel* countsLabel;
    QGridLayout* serverGrid;
    QLabel* queueSlot[QUEUE_SIZE];
    QLabel* queueMark[QUEUE_SIZE];
    QLabel* queueInfo;
    QListWidget* logList;

    void refresh();
    void showServers();
    void showQueue();
    void showLog();
    void updateServerList();
    QWidget* makeServerCard(Server* s);
};

#endif
