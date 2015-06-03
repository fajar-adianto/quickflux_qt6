#include <QtCore>
#include <QtQml>
#include <QVariant>
#include <QJSValue>
#include <QPointer>
#include "qfappdispatcher.h"

QFAppDispatcher::QFAppDispatcher(QObject *parent) : QObject(parent)
{
    m_dispatching = false;

}

QFAppDispatcher::~QFAppDispatcher()
{

}

void QFAppDispatcher::dispatch(QString name, QJSValue message)
{
    if (m_dispatching) {
        m_queue.enqueue(QPair<QString,QJSValue> (name,message) );
        return;
    }

    m_dispatching = true;
    emit received(name,message);

    while (m_queue.size() > 0) {
        QPair<QString,QJSValue> pair = m_queue.dequeue();
        emit received(pair.first,pair.second);
    }
    m_dispatching = false;
}

static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    QFAppDispatcher* object = new QFAppDispatcher();

    return object;
}

class QFActionDispatcherRegisterHelper {

public:
    QFActionDispatcherRegisterHelper() {
        qmlRegisterSingletonType<QFAppDispatcher>("QuickFlux", 1, 0, "AppDispatcher", provider);
    }
};

static QFActionDispatcherRegisterHelper registerHelper;