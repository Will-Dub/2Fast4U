#include "consolereader.h"
#include <unistd.h>
#include <stdio.h>

ConsoleReader::ConsoleReader(QObject *parent)
    : QThread{parent}
{
}

ConsoleReader::~ConsoleReader()
{
    resetTermios();

    if(isRunning()) {
        terminate(); // Note: terminate() is dangerous, but necessary here because getchar() blocks indefinitely.
        wait();
    }
}

void ConsoleReader::initTermios(bool echo) {
    tcgetattr(STDIN_FILENO, &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~ICANON;

    if (echo) {
        newSettings.c_lflag |= ECHO;
    } else {
        newSettings.c_lflag &= ~ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void ConsoleReader::resetTermios() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
}

void ConsoleReader::run() {
    initTermios(false);

    while (!isInterruptionRequested()) {
        int c = getchar();

        if (c == EOF) break;

        char key = static_cast<char>(c);

        // Lock the mutex to safely add the key to our buffer
        {
            QMutexLocker locker(&m_mutex);
            m_keyBuffer.append(key);

            // Optional: Prevent the buffer from growing infinitely if
            // the main thread stops calling isKeyPressed()
            if (m_keyBuffer.size() > 50) {
                m_keyBuffer.removeFirst();
            }
        }

        // Still emit the signal for event-driven parts of your app
        emit KeyPressed(key);
    }

    resetTermios();
}

// Check if a specific key is in our queue
bool ConsoleReader::isKeyPressed(char targetKey) {
    QMutexLocker locker(&m_mutex); // Protect the buffer

    if (m_keyBuffer.contains(targetKey)) {
        // We found the key! Remove it so it acts as "consumed"
        // Without this, the key would be permanently stuck as "pressed"
        m_keyBuffer.removeOne(targetKey);
        return true;
    }

    return false;
}
