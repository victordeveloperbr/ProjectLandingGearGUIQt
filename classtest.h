
#ifndef CLASSTEST_H
#define CLASSTEST_H


#include <QObject>


class classtest : public QObject
{
    Q_OBJECT
public:
    explicit classtest(QObject *parent = nullptr);

signals:

};

#endif // CLASSTEST_H
