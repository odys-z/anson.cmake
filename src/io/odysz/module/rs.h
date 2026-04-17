#pragma once

#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <chrono>

#include "io/odysz/common.h"
#include "io/odysz/anson.h"

namespace anson {

// class Column : public Anson {
// public:
//     inline static const string _type_ = "io.odysz.module.rs.AnResultset.Column";
//     int colx;
//     string col_id;

//     Column() = default;
//     Column(int cx, string nom) : colx(cx), col_id(nom) {}
// };


// using ValType = std::variant<std::monostate, int, double, std::string, std::chrono::system_clock::time_point>;
using Column = vector<LangExt::VarType>;
using Row = vector<LangExt::VarType>;

/**
 * @brief The AnResultset class
 *
 * All indecies start at 0.
 */
class AnResultset : public Anson {
public:
//    inline static const string _variantype_ = "json-value";

    inline static const string _type_ = "io.odysz.module.rs.AnResultset";

    vector<vector<LangExt::VarType>> rows;
    map<string, Column> colnames;

    /** start at 0 */
    int rowIdx;
    int rowCnt;
    int colCnt;
    long total;

    AnResultset() : Anson(_type_), rowIdx(-1), rowCnt(0), colCnt(0), total(0){}

    template<typename... RowTypes>
    AnResultset(const map<string, Column> &columns, RowTypes&&... row)
        : AnResultset() {

        this->colnames = columns;
        this->colCnt = columns.size();

        this->rows.reserve(sizeof...(row));

        ([&](auto& r) {
            Row variantRow;
            variantRow.reserve(r.size());

            for (const auto& cell : r) {
                variantRow.emplace_back(cell);
            }

            this->rows.push_back(std::move(variantRow));
        }(row), ...);

        this->rowCnt = rows.size();
        total += rowCnt;
    }

    bool next() {
        if (rowIdx < rowCnt)
            return ++rowIdx < rowCnt;
        else return false;
    }

    AnResultset& beforeFirst() {
        rowIdx = -1;
        return *this;
    }

protected:
    const LangExt::VarType* getCell(const string& col) const {
        if (rowIdx < 0 || rowIdx >= rowCnt) return nullptr;

        auto it = colnames.find(col);
        if (it != colnames.end()) {
            optional<int> colx = LangExt::var_int(it->second[0]);
            if (colx)
                return &rows[rowIdx][*colx];
        }

        string c = LangExt::upper_case(col);
        it = colnames.find(c);
        if (it != colnames.end()) {
            // return &rows[rowIdx][it->second.colx];
            optional<int> colx = LangExt::var_int(it->second[0]);
            if (colx)
                return &rows[rowIdx][*colx];
        }

        return nullptr;
    }

public:

    std::optional<int> getInt(const string& col) {
        if (auto* cell = getCell(col)) {
            if (auto* val = std::get_if<int>(cell)) return *val;
        }
        return std::nullopt;
    }

    std::optional<double> getDouble(const string& col) {
        if (auto* cell = getCell(col)) {
            if (auto* val = std::get_if<double>(cell)) return *val;
        }
        return std::nullopt;
    }

    std::optional<string_view> getString(const string& col) {
        if (auto* cell = getCell(col)) {
            if (auto* val = std::get_if<std::string>(cell)) return *val;
        }
        return std::nullopt;
    }

    using DateTime = std::chrono::system_clock::time_point;
    std::optional<DateTime> getDate(const string& col) {
        if (auto* cell = getCell(col)) {
            if (auto* val = std::get_if<DateTime>(cell)) return *val;
        }
        return std::nullopt;
    }};
}
