#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include <QThread>
#include <QMutex>
#include <QList>
#include <termios.h>

class ConsoleReader : public QThread
{
    Q_OBJECT
public:
    explicit ConsoleReader(QObject *parent = nullptr);
    ~ConsoleReader() override;

    bool isKeyPressed(char key);

signals:
    void KeyPressed(char key);

protected:
    void run() override;

private:
    void initTermios(bool echo);
    void resetTermios();

    struct termios oldSettings;
    struct termios newSettings;

    QMutex m_mutex;
    QList<char> m_keyBuffer;
};

#endif // CONSOLEREADER_H
