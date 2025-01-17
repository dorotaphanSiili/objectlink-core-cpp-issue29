#pragma once

#include "olink/remotenode.h"
#include "olink/remoteregistry.h"
#include "olink/iobjectsource.h"
#include <iostream>

using namespace ApiGear::ObjectLink;

class CalcSource: public IObjectSource {
public:
    CalcSource(RemoteRegistry& registry)
        : m_node(nullptr)
        , m_registry(&registry)
        , m_total(1)
    {
    }
    virtual ~CalcSource() override {
        m_registry->removeSource(olinkObjectName());
    }

    IRemoteNode* remoteNode() const {
        assert(m_node);
        return m_node;
    }

    int add(int value) {
        m_total += value;
        remoteNode()->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/add", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total >= 10) {
            remoteNode()->notifySignal("demo.Calc/hitUpper", { 10 });
            m_events.push_back({ "demo.Calc/hitUpper", 10 });
        }
        return m_total;
    }

    int sub(int value) {
        m_total -= value;
        remoteNode()->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/sub", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total <= 0) {
            remoteNode()->notifySignal("demo.Calc/hitLower", { 0 });
            m_events.push_back({ "demo.Calc/hitLower", 0 });
        }
        return m_total;
    }
    void notifyShutdown(int timeout) {
        remoteNode()->notifySignal("demo.Calc/timeout", { timeout });
    }
    // IServiceObjectListener interface
public:
    std::string olinkObjectName() override {
        return "demo.Calc";
    }
    nlohmann::json olinkInvoke(const std::string& name, const nlohmann::json& args) override {
        std::cout << "invoke" << name << args.dump();
        std::string path = Name::getMemberName(name);
        if(path == "add") {
            int a = args[0].get<int>();
            int result = add(a);
            return result;
        }
        return nlohmann::json();
    }
    void olinkSetProperty(const std::string& name, const nlohmann::json& value) override {
        std::cout << "setProperty" << name << value.dump();
        std::string path = Name::getMemberName(name);
        if(path == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
                remoteNode()->notifyPropertyChange(name, total);
            }
        }
    }
    void olinkLinked(const std::string& name, IRemoteNode *node) override {
        std::cout << "linked" << name;
        m_node = node;
    }
    void olinkUnlinked(const std::string& name) override
    {
        std::cout << "unlinked" << name;
        m_node = nullptr;
    }
    nlohmann::json olinkCollectProperties() override
    {
        return {{ "total", m_total }};
    }
private:
    IRemoteNode* m_node;
    RemoteRegistry* m_registry;
    int m_total;
    std::vector<nlohmann::json> m_events;
};
