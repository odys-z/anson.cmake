#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "anson.h"

namespace anson {
public class Device : public anson::Anson {
public:
    string id;
    string synode0;
    string devname;
    string tofolder;
};

public class PageInf : public anson::Anson {
public:
    string page;
    string size;
    string total;
    List<String[] arrCondts;
    Map<String, ? mapCondts;
};

public class PathsPage : public anson::Anson {
public:
    string device;
    long start;
    long end;
    Map<String, String[] clientPaths;
};

public class DocsReq : public anson::AnsonBody {
public:
    struct A {
        inline static const string syncdocs = "r/syncs";
        inline static const string orgNodes = "r/synodes";
        inline static const string and more = "...";
    };
    string synuri;
    string docTabl;
    ExpSyncDoc doc;
    PageInf pageInf;
    string stamp;
    Device device;
    string org;
    PathsPage syncingPage;
};

}