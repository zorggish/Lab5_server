#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::pushButtonClicked);
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::connectUser);

    QString path = QCoreApplication::applicationDirPath() + "/messages.txt";
    QFile messagesFile(path);
    if(messagesFile.exists())
    {
        messagesFile.open(QIODevice::ReadOnly);
        QString fileData = messagesFile.readAll();
        QStringList msgs = fileData.split(static_cast<char>(7));
        qDebug() << msgs.size();
        for(int i = 0; i < 10; i++)
            messages.push_back(msgs[i]);
        messagesFile.close();
        updateTextBox();
    }
    else
        for(int i = 0; i<10; i++)
            messages.push_back("");
}

MainWindow::~MainWindow()
{
    QString path = QCoreApplication::applicationDirPath() + "/messages.txt";
    QFile messagesFile(path);
    messagesFile.open(QIODevice::WriteOnly);
    for(int i = 0; i < 10; i++)
        messagesFile.write(messages[i].toUtf8() + static_cast<char>(7));
    messagesFile.close();
    delete ui;
}

