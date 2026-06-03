#pragma once

#include <io/odysz/anson.h>

namespace anson {
class TableMeta {

public:
    string tbl;
    string pk;
    string conn;

    TableMeta(const string &tbl, const string &conn = "") : tbl(tbl), conn(conn) {}
};

class SemanticTableMeta : public TableMeta {
public:
    SemanticTableMeta(const string &tbl, const string &conn = "") : TableMeta(tbl, conn) {}
};

class SynEntityMeta : public SemanticTableMeta {
public:
    SynEntityMeta(const string &tab, const string &conn = "") : SemanticTableMeta(tab, conn) {}

    SynEntityMeta() : anson::SynEntityMeta("", "") {}

    SynEntityMeta(const string &tab, const string &pk, const string &deviceid, const string & conn)
        : SemanticTableMeta(tab, conn) {
        this->pk = pk;
    }
};

class IFileDescriptor {

public:
    string device;

    string fullpath() const {
        return "TODO";
    }
};
}
