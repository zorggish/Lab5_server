#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpServer* tcpServer;
    QVector<QTcpSocket*> clients;
    QQueue<QString> messages;
    enum connectStatusType{disconnected, connected};
    connectStatusType connectStatus = disconnected;

public slots:
    void pushButtonClicked()
    {
        /**
        * @brief Обработчик нажатия кнопки подключения
        */
        if(connectStatus == disconnected){
            if (tcpServer->listen(QHostAddress::Any, 1337)){
                ui->statusLabel->setText("Статус: включён");
                ui->pushButton->setText("Выключить");
            }
            connectStatus = connected;
        } else {
            ui->statusLabel->setText("Статус: выключен");
            ui->pushButton->setText("Включить");
            while(clients.size()) {
                clients.last()->disconnectFromHost();
                clients.pop_back();
            }
            tcpServer->close();
            connectStatus = disconnected;
        }
    }

    void connectUser()
    {
        /**
        * @brief Обработать подключение нового пользователя
        */
        QTcpSocket* client = tcpServer->nextPendingConnection();
        connect(client, &QTcpSocket::readyRead, this, &MainWindow::runClientAction);
        clients.push_back(client);

        for(int i = 0; i < messages.size(); i++){
            client->write(messages[i].toUtf8() + static_cast<char>(7));
            client->flush();
        }
    }

    void runClientAction()
    {
        /**
        * @brief Обработать сообщение от пользователя
        */
        QTcpSocket* clientSocket = (QTcpSocket*) sender();
        QTextStream messageStream (clientSocket);
        QString message = messageStream.readAll();

        if(message == "disconnect")
        {
            clientSocket->close();
            clients.removeOne(clientSocket);
        }
        else pushMessage(message);
    }

    void pushMessage(QString message)
    {
        /**
        * @brief Добавить новое сообщение
        * @param message Сообщение
        */
        messages.pop_front();
        messages.push_back(message);
        updateTextBox();
        for(int i = 0; i < clients.size(); i++)
            clients[i]->write(message.toUtf8() + static_cast<char>(7));
    }

    void updateTextBox()
    {
        /**
        * @brief Обновление текста на форме
        */
        QString text;
        for(int i = 0; i < 10; i++)
            text += messages[i] + "\n";
        ui->textEdit->setText(text);
    }
};
#endif // MAINWINDOW_H
