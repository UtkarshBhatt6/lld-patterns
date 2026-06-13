#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <ctime>

using namespace std;

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

string logLevelToString(LogLevel level) {
    switch(level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
    }
    return "";
}

class LogMessage {
private:
    string timestamp;
    LogLevel level;
    string message;

public:
    LogMessage(
        string timestamp,
        LogLevel level,
        string message
    ) {
        this->timestamp = timestamp;
        this->level = level;
        this->message = message;
    }

    string getTimestamp() const {
        return timestamp;
    }

    string getMessage() const {
        return message;
    }

    LogLevel getLogLevel() const {
        return level;
    }
};

class LogAppender {
public:
    virtual void append(const LogMessage& logMessage) = 0;
};

class ConsoleAppender : public LogAppender {
public:
    void append(const LogMessage& logMessage) override {

        cout
            << "["
            << logMessage.getTimestamp()
            << "] "
            << logLevelToString(logMessage.getLogLevel())
            << " : "
            << logMessage.getMessage()
            << endl;
    }
};

class FileAppender : public LogAppender {
public:
    void append(const LogMessage& logMessage) override {

        // For interview purpose.
        // In real implementation write to file.

        cout
            << "[FILE] "
            << "["
            << logMessage.getTimestamp()
            << "] "
            << logLevelToString(logMessage.getLogLevel())
            << " : "
            << logMessage.getMessage()
            << endl;
    }
};

class Logger {
private:
    LogLevel currentLevel;
    vector<LogAppender*> appenders;

    static Logger* instance;

    mutable mutex mtx;

    Logger() {
        currentLevel = LogLevel::INFO;
    }

    string getCurrentTimestamp() {

        time_t now = time(0);

        string ts = ctime(&now);

        if(!ts.empty() && ts.back() == '\n') {
            ts.pop_back();
        }
        
        return ts;
    }

public:

    static Logger* getInstance() {

        if(instance == nullptr) {
            instance = new Logger();
        }

        return instance;
    }

    void setLogLevel(LogLevel level) {
        currentLevel = level;
    }

    void addAppender(LogAppender* appender) {
        appenders.push_back(appender);
    }

    void log(
        LogLevel level,
        string message
    ) {

        lock_guard<mutex> lock(mtx);

        if(level < currentLevel) {
            return;
        }

        LogMessage logMessage(
            getCurrentTimestamp(),
            level,
            message
        );

        for(auto appender : appenders) {
            appender->append(logMessage);
        }
    }
};

Logger* Logger::instance = nullptr;

int main() {

    Logger* logger = Logger::getInstance();

    logger->setLogLevel(LogLevel::INFO);

    ConsoleAppender consoleAppender;
    FileAppender fileAppender;

    logger->addAppender(&consoleAppender);
    logger->addAppender(&fileAppender);

    logger->log(
        LogLevel::INFO,
        "Application Started"
    );

    logger->log(
        LogLevel::DEBUG,
        "This debug log will be filtered"
    );

    logger->log(
        LogLevel::ERROR,
        "Database Connection Failed"
    );

    return 0;
}