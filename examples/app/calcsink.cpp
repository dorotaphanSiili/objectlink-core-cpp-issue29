#include "calcsink.h"

#include "olink/core/types.h"

using namespace ApiGear::ObjectLink;

CalcSink::CalcSink(QObject *parent)
    : QObject(parent)
    , m_node(nullptr)
{
}

CalcSink::~CalcSink() {
    QClientRegistry::getInstance().removeSink(olinkObjectName());
    m_node = nullptr;
}

void CalcSink::add(int a)
{
    assert(m_node);
    m_node->invokeRemote("demo.Calc/add", { a });
}

void CalcSink::sub(int a)
{
    assert(m_node);
    m_node->invokeRemote("demo.Calc/sub", { a });
}

void CalcSink::clear()
{
    assert(m_node);
    m_node->invokeRemote("demo.Calc/clear");
}

int CalcSink::total() const {
    return m_total;
}

void CalcSink::setTotal(int total)
{
    assert(m_node);
    m_node->setRemoteProperty("demo.Calc/total", total);
}

bool CalcSink::isReady() const {
    return m_ready;
}

std::string CalcSink::olinkObjectName() {
    return "demo.Calc";
}

void CalcSink::olinkOnSignal(const std::string& name, const json& args)
{
    std::string path = Name::getMemberName(name);
    if(path == "maxReached") {
        int value = args[0].get<int>();
        emit maxReached(value);
    }
    else if(path == "minReached") {
        int value = args[0].get<int>();
        emit minReached(value);
    }
}
void CalcSink::olinkOnPropertyChanged(const std::string& name, const json& value)
{
    qDebug() << "property changed: " << QString::fromStdString(name);
    std::string path = Name::getMemberName(name);
    if(path == "total") {
        int total = value.get<int>();
        if(m_total != total) {
            m_total = total;
            emit totalChanged(total);
        }
    }
}
void CalcSink::olinkOnInit(const std::string& name, const json& props, IClientNode *node)
{
    qDebug() << Q_FUNC_INFO << QString::fromStdString(name);
    m_name = name;
    m_ready = true;
    m_node = node;
    if(props.contains("total")) {
        int total = props["total"].get<int>();
        if(m_total != total) {
            m_total = total;
            emit totalChanged(total);
        }
    }
    assert(m_node);
}

void CalcSink::olinkOnRelease()
{
    m_ready = false;
    m_node = nullptr;
}
