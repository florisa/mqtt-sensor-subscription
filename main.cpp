#include <QCoreApplication>
#include <QtMqtt/QtMqtt>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const QString hostname{"localhost"};
    const quint16 port = 1883;
    const QMqttTopicName topic{"qtdemosensors/"};
    const QMqttTopicFilter filter{"qtdemosensors/#"};

    QMqttClient client;
    client.setHostname(hostname);
    client.setPort(port);

    QObject::connect(&client, &QMqttClient::stateChanged, [](QMqttClient::ClientState state){
            if(state == QMqttClient::Disconnected)
                qDebug() << " State: Disconnected";
            else if(state == QMqttClient::Connecting)
                qDebug() << " State: Connecting";
            else if(state == QMqttClient::Connected)
                qDebug() << " State: Connected";
        });

    QObject::connect(&client, &QMqttClient::errorChanged, [](QMqttClient::ClientError error){
           qDebug() << "not connected" << error;

       });

    QObject::connect(&client, &QMqttClient::messageReceived, [](const QByteArray &message, const QMqttTopicName &topic){
           qDebug() << " Received Topic:" << topic.name() << " Message: " << message;
       });

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&client, &topic](){
           if(client.publish(topic,  QDateTime::currentDateTime().toString().toUtf8()) == -1)
               qDebug() << "Error: Could not publish message";
       });

    QObject::connect(&client, &QMqttClient::connected, [&client, &timer, &filter](){
            QMqttSubscription *subscription = client.subscribe(filter);
            if(!subscription)
                qDebug() << "Could not subscribe";
            timer.start(1000);
        });

        client.connectToHost();

    return a.exec();
}
