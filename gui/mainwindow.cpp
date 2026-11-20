// GUI module: main window of the simulator
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFrame>
#include <QMessageBox>
#include <QStringList>

const bool TEST_MODE = false; 

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    logger = new Logger("load_balancer_log.txt");
    lb = NULL;
    shownLines = 0;

    setWindowTitle("Multi-Algorithm Load Balancing Simulator");
    resize(1300, 860);
    setStyleSheet(
        "QWidget { color: #1f2328; font-size: 13px; }"
        "QMainWindow { background: #f6f7f9; }"
        "QLineEdit, QSpinBox, QComboBox { background: white; color: #1f2328;"
        "  border: 1px solid #c9ced5; border-radius: 4px; padding: 4px; }"
        "QComboBox QAbstractItemView { background: white; color: #1f2328; }"
        "QListWidget { background: white; color: #1f2328; border: 1px solid #d7dbe0; }"
        "QGroupBox { font-weight: bold; color: #57606a; background: white;"
        "  border: 1px solid #d7dbe0; border-radius: 6px; margin-top: 14px; padding: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }"
        "QPushButton { padding: 7px; border: 1px solid #c9ced5; border-radius: 5px; background: white; }"
        "QPushButton:hover { background: #eef3fb; }"
        "QPushButton#generateButton { background: #1f5fbf; color: white; border: none; font-weight: bold; }"
        "QPushButton#failButton { background: #fbe4e2; color: #a11b12; }"
        "QPushButton#recoverButton { background: #e3f1ec; color: #135c43; }"
        "QFrame#card { background: white; border: 1px solid #d7dbe0; border-radius: 8px; }"
    );

    QWidget* central = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    setCentralWidget(central);

    // ================= left side =================
    QVBoxLayout* left = new QVBoxLayout;

    QLabel* title = new QLabel("Multi-Algorithm Load Balancing Simulator");
    title->setWordWrap(true);
    title->setStyleSheet("font-size: 17px; font-weight: bold;");
    left->addWidget(title);

    // server setup
    QGroupBox* setupBox = new QGroupBox("SERVER SETUP");
    QVBoxLayout* setupLayout = new QVBoxLayout(setupBox);
    serverCount = new QSpinBox;
    serverCount->setRange(2, 10);
    serverCount->setValue(3);
    capacityInput = new QLineEdit("3, 4, 2");
    QPushButton* createButton = new QPushButton("Create Servers");
    setupLayout->addWidget(new QLabel("Number of servers"));
    setupLayout->addWidget(serverCount);
    setupLayout->addWidget(new QLabel("Capacity of each server (1-8)"));
    setupLayout->addWidget(capacityInput);
    setupLayout->addWidget(createButton);
    left->addWidget(setupBox);

    // generate requests
    QGroupBox* requestBox = new QGroupBox("GENERATE REQUESTS");
    QVBoxLayout* requestLayout = new QVBoxLayout(requestBox);
    requestCount = new QSpinBox;
    requestCount->setRange(1, 100);
    requestCount->setValue(5);
    QPushButton* generateButton = new QPushButton("Generate Requests");
    generateButton->setObjectName("generateButton");
    requestLayout->addWidget(new QLabel("Number of requests"));
    requestLayout->addWidget(requestCount);
    requestLayout->addWidget(generateButton);
    left->addWidget(requestBox);

    // server failure and recovery
    QGroupBox* failBox = new QGroupBox("SERVER FAILURE AND RECOVERY");
    QVBoxLayout* failLayout = new QVBoxLayout(failBox);
    serverList = new QComboBox;
    QPushButton* failButton = new QPushButton("Fail Server");
    QPushButton* recoverButton = new QPushButton("Recover Server");
    failButton->setObjectName("failButton");
    recoverButton->setObjectName("recoverButton");
    QHBoxLayout* failButtons = new QHBoxLayout;
    failButtons->addWidget(failButton);
    failButtons->addWidget(recoverButton);
    failLayout->addWidget(new QLabel("Server ID"));
    failLayout->addWidget(serverList);
    failLayout->addLayout(failButtons);
    left->addWidget(failBox);

    // auto-scaling settings
    QGroupBox* scaleBox = new QGroupBox("AUTO-SCALING");
    QVBoxLayout* scaleLayout = new QVBoxLayout(scaleBox);
    autoMode = new QRadioButton("Automatic (vertical first, then horizontal)");
    verticalMode = new QRadioButton("Vertical only (increase capacity)");
    horizontalMode = new QRadioButton("Horizontal only (add servers)");
    autoMode->setChecked(true);
    stepInput = new QSpinBox;
    stepInput->setRange(1, 4);
    stepInput->setValue(2);
    QPushButton* applyButton = new QPushButton("Apply Scaling Settings");
    scaleLayout->addWidget(new QLabel("Scaling type"));
    scaleLayout->addWidget(autoMode);
    scaleLayout->addWidget(verticalMode);
    scaleLayout->addWidget(horizontalMode);
    scaleLayout->addWidget(new QLabel("Capacity change per vertical step (1-4)"));
    scaleLayout->addWidget(stepInput);
    scaleLayout->addWidget(applyButton);
    left->addWidget(scaleBox);

    QPushButton* summaryButton = new QPushButton("Save Summary to Log File");
    left->addWidget(summaryButton);
    left->addStretch();

    QWidget* leftPanel = new QWidget;
    leftPanel->setLayout(left);
    leftPanel->setFixedWidth(300);
    mainLayout->addWidget(leftPanel);

    // ================= right side =================
    QVBoxLayout* right = new QVBoxLayout;

    // top cards: algorithm, auto-scaling, request counts
    QHBoxLayout* cards = new QHBoxLayout;

    QFrame* algoCard = new QFrame;
    algoCard->setObjectName("card");
    QVBoxLayout* algoLayout = new QVBoxLayout(algoCard);
    QLabel* algoTitle = new QLabel("ALGORITHM (selected automatically)");
    algoTitle->setStyleSheet("color: #57606a; font-weight: bold;");
    algoName = new QLabel("-");
    algoName->setStyleSheet("font-size: 20px; font-weight: bold;");
    algoReason = new QLabel("Create servers to start the simulation");
    algoReason->setWordWrap(true);
    algoLayout->addWidget(algoTitle);
    algoLayout->addWidget(algoName);
    algoLayout->addWidget(algoReason);
    cards->addWidget(algoCard, 2);

    QFrame* scaleCard = new QFrame;
    scaleCard->setObjectName("card");
    QVBoxLayout* scaleCardLayout = new QVBoxLayout(scaleCard);
    QLabel* scaleTitle = new QLabel("AUTO-SCALING");
    scaleTitle->setStyleSheet("color: #57606a; font-weight: bold;");
    utilLabel = new QLabel("-");
    utilLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    scaleInfo = new QLabel("");
    scaleInfo->setWordWrap(true);
    scaleCardLayout->addWidget(scaleTitle);
    scaleCardLayout->addWidget(utilLabel);
    scaleCardLayout->addWidget(scaleInfo);
    cards->addWidget(scaleCard, 2);

    QFrame* countCard = new QFrame;
    countCard->setObjectName("card");
    QVBoxLayout* countLayout = new QVBoxLayout(countCard);
    QLabel* countTitle = new QLabel("REQUESTS");
    countTitle->setStyleSheet("color: #57606a; font-weight: bold;");
    countsLabel = new QLabel("-");
    countsLabel->setStyleSheet("font-size: 14px;");
    countLayout->addWidget(countTitle);
    countLayout->addWidget(countsLabel);
    cards->addWidget(countCard, 1);

    right->addLayout(cards);

    // server status
    QGroupBox* serverBox = new QGroupBox("SERVER STATUS");
    serverGrid = new QGridLayout(serverBox);
    right->addWidget(serverBox, 3);

    // waiting requests (circular queue)
    QGroupBox* queueBox = new QGroupBox("WAITING REQUESTS (CIRCULAR QUEUE)");
    QVBoxLayout* queueLayout = new QVBoxLayout(queueBox);
    queueInfo = new QLabel("");
    queueLayout->addWidget(queueInfo);
    QGridLayout* queueGrid = new QGridLayout;
    for (int i = 0; i < QUEUE_SIZE; i++) {
        queueSlot[i] = new QLabel("empty");
        queueSlot[i]->setAlignment(Qt::AlignCenter);
        queueSlot[i]->setFixedSize(82, 40);
        queueMark[i] = new QLabel("[" + QString::number(i) + "]");
        queueMark[i]->setAlignment(Qt::AlignCenter);
        queueMark[i]->setMinimumHeight(42);
        int row = (i / 10) * 2;
        queueGrid->addWidget(queueSlot[i], row, i % 10);
        queueGrid->addWidget(queueMark[i], row + 1, i % 10);
    }
    queueLayout->addLayout(queueGrid);
    right->addWidget(queueBox);

    // activity log
    QGroupBox* logBox = new QGroupBox("ACTIVITY LOG (newest first)");
    QVBoxLayout* logLayout = new QVBoxLayout(logBox);
    logList = new QListWidget;
    logLayout->addWidget(logList);
    right->addWidget(logBox, 2);

    mainLayout->addLayout(right, 1);


    timer = new QTimer(this);

    connect(createButton, &QPushButton::clicked, this, &MainWindow::createServers);
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::generateRequests);
    connect(failButton, &QPushButton::clicked, this, &MainWindow::failServer);
    connect(recoverButton, &QPushButton::clicked, this, &MainWindow::recoverServer);
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::applyScaling);
    connect(summaryButton, &QPushButton::clicked, this, &MainWindow::saveSummary);
    connect(timer, &QTimer::timeout, this, &MainWindow::nextSecond);

    showLog();
}

MainWindow::~MainWindow() {
    timer->stop();
    if (lb != NULL)
        delete lb;
    delete logger;
}

// ================= button actions =================

void MainWindow::createServers() {
    QStringList parts = capacityInput->text().split(",");
    vector<int> capacities;

    for (int i = 0; i < parts.size(); i++) {
        QString part = parts[i].trimmed();
        if (part.isEmpty())
            continue;
        bool ok;
        int cap = part.toInt(&ok);
        if (!ok || cap < 1 || cap > MAX_CAPACITY) {
            QMessageBox::warning(this, "Wrong input", "Each capacity must be a number from 1 to 8.");
            return;
        }
        capacities.push_back(cap);
    }

    if ((int)capacities.size() != serverCount->value()) {
        QMessageBox::warning(this, "Wrong input",
                             "Enter one capacity for each server, separated by commas (example: 3, 4, 2).");
        return;
    }

    timer->stop();
    if (lb != NULL)
        delete lb;
    lb = new LoadBalancer(capacities, logger, TEST_MODE);
    applyScaling();        // use the scaling settings chosen on the left
    shownIds.clear();
    refresh();
    timer->start(1000);
}

void MainWindow::generateRequests() {
    if (lb == NULL) {
        QMessageBox::information(this, "No servers", "Create the servers first.");
        return;
    }
    lb->generateRequests(requestCount->value());
    refresh();
}

void MainWindow::failServer() {
    if (lb == NULL || serverList->count() == 0)
        return;
    int id = serverList->currentData().toInt();
    if (!lb->failServer(id))
        QMessageBox::information(this, "Server", "Server " + QString::number(id) + " has already failed.");
    refresh();
}

void MainWindow::recoverServer() {
    if (lb == NULL || serverList->count() == 0)
        return;
    int id = serverList->currentData().toInt();
    if (!lb->recoverServer(id))
        QMessageBox::information(this, "Server", "Server " + QString::number(id) + " is already active.");
    refresh();
}

void MainWindow::applyScaling() {
    if (lb == NULL)
        return;
    int mode = 0;                       // automatic
    if (verticalMode->isChecked())
        mode = 1;
    else if (horizontalMode->isChecked())
        mode = 2;
    lb->setScaling(mode, stepInput->value());
    refresh();
}

void MainWindow::saveSummary() {
    if (lb == NULL)
        return;
    lb->summary();
    refresh();
    QMessageBox::information(this, "Summary", "The summary was saved in load_balancer_log.txt");
}

// called by the timer every second
void MainWindow::nextSecond() {
    if (lb == NULL)
        return;
    lb->oneSecond();
    refresh();
}

// ================= display =================

void MainWindow::refresh() {
    if (lb == NULL)
        return;

    algoName->setText(QString::fromStdString(lb->getAlgorithmName()));
    algoReason->setText("Reason: " + QString::fromStdString(lb->getAlgorithmReason()));

    int util = lb->getUtilization();
    utilLabel->setText(QString::number(util) + "% used");
    if (util > 80)
        utilLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #a11b12;");
    else
        utilLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #135c43;");
    scaleInfo->setText("Threshold: 80%\nMode: " + QString::fromStdString(lb->getScalingMode()) +
                       "\nVertical step: " + QString::number(lb->getScalingStep()));

    countsLabel->setText("Time: " + QString::number(lb->getTime()) + " sec\n" +
                         "Generated: " + QString::number(lb->getGenerated()) + "\n" +
                         "Processing: " + QString::number(lb->getProcessing()) + "\n" +
                         "Waiting: " + QString::number(lb->getWaiting()) + "\n" +
                         "Completed: " + QString::number(lb->getCompleted()));

    showServers();
    showQueue();
    showLog();
    updateServerList();
}

void MainWindow::showServers() {
    QLayoutItem* item;
    while ((item = serverGrid->takeAt(0)) != NULL) {    
        if (item->widget() != NULL)
            delete item->widget();
        delete item;
    }

    vector<Server*> servers = lb->getServers();
    for (int i = 0; i < (int)servers.size(); i++)
        serverGrid->addWidget(makeServerCard(servers[i]), i / 4, i % 4);
}

QWidget* MainWindow::makeServerCard(Server* s) {
    QFrame* card = new QFrame;
    card->setObjectName("server");
    if (s->isActive())
        card->setStyleSheet("QFrame#server { background: white; border: 1px solid #d7dbe0; border-radius: 8px; }");
    else
        card->setStyleSheet("QFrame#server { background: #fcf7f6; border: 1px dashed #d99a94; border-radius: 8px; }");
    QVBoxLayout* layout = new QVBoxLayout(card);

    // name and status
    QHBoxLayout* top = new QHBoxLayout;
    QLabel* name = new QLabel("Server " + QString::number(s->getId()));
    name->setStyleSheet("font-size: 15px; font-weight: bold;");
    QLabel* status = new QLabel(s->isActive() ? "ACTIVE" : "FAILED");
    if (s->isActive())
        status->setStyleSheet("background: #e3f1ec; color: #135c43; font-weight: bold; padding: 2px 6px;");
    else
        status->setStyleSheet("background: #fbe4e2; color: #a11b12; font-weight: bold; padding: 2px 6px;");
    top->addWidget(name);
    top->addStretch();
    top->addWidget(status);
    layout->addLayout(top);

    QLabel* details = new QLabel("Capacity: " + QString::number(s->getCapacity()) +
                                 " | Load: " + QString::number(s->getLoad()) +
                                 " | Weight: " + QString::number(s->getCapacity()));
    layout->addWidget(details);

   
    QHBoxLayout* slotRow = new QHBoxLayout;
    for (int i = 0; i < s->getCapacity(); i++) {
        QLabel* box = new QLabel;
        box->setFixedSize(18, 18);
        if (i < s->getLoad())
            box->setStyleSheet("background: #1f5fbf; border-radius: 3px;");
        else
            box->setStyleSheet("background: #eef0f3; border: 1px solid #c9ced5; border-radius: 3px;");
        slotRow->addWidget(box);
    }
    slotRow->addStretch();
    layout->addLayout(slotRow);

    vector<Request> running = s->getRunning();
    for (int i = 0; i < (int)running.size(); i++) {
        QLabel* req = new QLabel("Request " + QString::number(running[i].id) +
                                 " (" + QString::fromUtf8(running[i].path) + ")  -  " +
                                 QString::number(running[i].remaining) + " sec left");
        req->setStyleSheet("background: #f3f5f8; padding: 3px;");
        layout->addWidget(req);
    }

    if (!s->isActive()) {
        QLabel* note = new QLabel("Server failed. No new requests are sent to it.");
        note->setWordWrap(true);
        layout->addWidget(note);
    }
    layout->addStretch();
    return card;
}

// queue display: every slot of the circular queue with FRONT and REAR
void MainWindow::showQueue() {
    CircularQueue* q = lb->getQueue();
    int front = q->getFront();
    int rear = q->getRear();
    int count = q->size();

    for (int i = 0; i < QUEUE_SIZE; i++) {
        int pos = (i - front + QUEUE_SIZE) % QUEUE_SIZE;    // position from the front
        bool used = pos < count;

        if (used) {
            Request r = q->slot(i);
            queueSlot[i]->setText("Request " + QString::number(r.id) + "\n" +
                                  QString::number(r.time) + " sec");
            queueSlot[i]->setStyleSheet("background: #eef3fb; border: 1px solid #9db8e3; border-radius: 5px;");
        } else {
            queueSlot[i]->setText("empty");
            queueSlot[i]->setStyleSheet("background: #f6f7f9; border: 1px dashed #c9ced5; border-radius: 5px; color: #8a929b;");
        }

        QString mark = "[" + QString::number(i) + "]";
        if (count > 0 && i == front)
            mark += "\nFRONT";
        if (count > 0 && i == rear)
            mark += "\nREAR";
        queueMark[i]->setText(mark);
    }

    queueInfo->setText("front = " + QString::number(front) +
                       "   |   rear = " + QString::number(rear) +
                       "   |   count = " + QString::number(count) +
                       "   |   size = " + QString::number(QUEUE_SIZE));
}

// activity log: shows only the new lines, newest at the top
void MainWindow::showLog() {
    vector<string> lines = logger->getLines();
    for (int i = shownLines; i < (int)lines.size(); i++)
        logList->insertItem(0, QString::fromStdString(lines[i]));
    shownLines = lines.size();

    while (logList->count() > 300)              // keep the list short
        delete logList->takeItem(logList->count() - 1);
}

// keeps the server drop-down list the same as the real servers
void MainWindow::updateServerList() {
    vector<Server*> servers = lb->getServers();
    vector<int> ids;
    for (int i = 0; i < (int)servers.size(); i++)
        ids.push_back(servers[i]->getId());

    if (ids == shownIds)        // nothing changed
        return;

    int selected = serverList->currentData().toInt();
    serverList->clear();
    for (int i = 0; i < (int)ids.size(); i++) {
        serverList->addItem("Server " + QString::number(ids[i]), ids[i]);
        if (ids[i] == selected)
            serverList->setCurrentIndex(i);
    }
    shownIds = ids;
}
