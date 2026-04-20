#pragma once

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include <iostream>

#include "anson.h"

namespace anson {

using namespace entt;

inline static entt::meta_data find_field_recursive(entt::meta_type type, id_type key) {
    if (auto data = type.data(key); data) {
        return data;
    }

    // base classes
    for (auto [id, base] : type.base()) {
        if (auto data = find_field_recursive(base, key); data) {
            return data;
        }
    }
    return {};
}

inline static ostream& serialize_prim_value(ostream &os, meta_any &inst,
                       const vector<string> &valtype, const JsonOpt &opts) {

    if (!opts.primtypes.contains(valtype[0]))
        return os << "\"Cannot serialize " << " [" << valtype[0] << "]\"";

    if ("string" == opts.primtypes.at(valtype[0])) {
        if (inst) {
            auto *s = inst.try_cast<const std::string>();
            if (s) return os << '"' << *s << '"';
            else // return os << '"' << std::to_string(inst) << '"';
                ;
        }
        else os << "null";
    }

    if ("int" == opts.primtypes.at(valtype[0])) {
        if (inst) {
            auto *s = inst.try_cast<const std::string>();
            return os << *s;
        }
    }

    if ("VarType" == opts.primtypes.at(valtype[0])) {
        return LangExt::serialize_var(os, inst);
    }

    return os << "\"serialize error: " << valtype[0] << '"';
}

inline static ostream& serialize_jsonable(ostream &os, meta_any& val,
              const AnsonAst &val_ast, const JsonOpt &opts) {

    if (val_ast.isList) {
        anerror("Simple value is not a list, and shouldn't reach here!");
        return os;
    }

    if (val_ast.isMap) {
        anerror("Simple value is not a map, and shouldn't reach here!");
        return os; // serialize_map(os, val, val_ast.dataAnclass);
    }

    if (val_ast.isEnum) {
        const AnsonJavaEnumAst * east = static_cast<const AnsonJavaEnumAst*>(&val_ast);
        string res = east->name_of(val);
        return os << '"' << res << '"';
    }

    if (val_ast.isPortEnum) {
        if (auto* je = val.try_cast<const JavaEnum>()) {
            return os << *je;
        } else {
            return os << "null";
        }
    }

    if (val_ast.istring) {
        auto s = val.try_cast<std::string>();
        return os << '\"' << *s << '\"';
    }

    if (val_ast.isJsonable) {
        IJsonable *jsonval = val.try_cast<IJsonable>();
        const IJsonable *anson = val.try_cast<Anson>();
        if(jsonval)
            jsonval->toBlock(os, opts);
        else
            anerror(std::format("serialize_jsonable(): Connot convert from meta_any to IJasonalbe: {}",
                                val_ast.dataAnclass));
        return os;
    }

    if (val_ast.isVar) {

    }

    // if (val_ast.isInt) {
    //     auto s = val.try_cast<int>();
    //     return os << '\"' << s << '\"';
    // }

    // if (val_ast.isDouble) {
    //     auto s = val.try_cast<std::double_t>();
    //     return os << '\"' << s << '\"';
    // }

    return os << R"("Cannot handle value of )" << val_ast.anclass << '\"';
}

inline static ostream& serialize_list(ostream& os, const meta_any &list_any,
                                      const vector<string> &val_type, const JsonOpt &opts) ;

inline static ostream& serialize_map(ostream& os, const meta_any &map_any,
                                     const vector<string> &val_type, const JsonOpt &opts) ;

inline static ostream& serialize_val(ostream& os, meta_any &inst,
                       const vector<string> &val_type, const JsonOpt &opts) {
    if (val_type[0].starts_with("list<")) {
        // AnsonAst *ast = opts.ast<AnsonAst>(val_type[0]);
        // if (ast->enttypeid != hashed_string{anson.anclass.c_str()})
        //     anerror(std::format("serialize_fields(): entt type_id mismatch: {} {}",
        //                         std::to_string(ast->enttypeid), anson.anclass));

        if (inst) {
            vector<string> valtype  = Regex::parseListValtype(val_type[0]);
            serialize_list(os, inst, valtype, opts);
        }
        else
            os << "null";
    }

    else if (val_type[0].starts_with("map<")) {
        vector<string> valtype  = Regex::parseMapValtype(val_type[0]);
        if (inst)
            serialize_map(os, inst, valtype, opts);
        else
            os << "null";
    }
    else {
        AnsonAst* fd_ast = opts.ast<AnsonAst>(val_type[0]);
        if (fd_ast) {
            // meta_type enttype = entt::resolve(ast->enttypeid);
            // hashed_string data_key{fn.c_str()};
            // meta_data data = find_field_recursive(enttype, data_key);

            // entt::meta_any obj = enttype.from_void(&anson);
            // meta_any val = data.get(obj);
            serialize_jsonable(os, inst, *fd_ast, opts);
        }
        else {
            // vector<string> valtype  = Regex::parse_val_type(val_type[0]);
            // entt::meta_any val = ast->get_field_instance(anson, fn);
            serialize_prim_value(os, inst, val_type, opts);
        }
    }

    return os;
}

inline static ostream& serialize_anson_prim_ptr(ostream& os, const meta_any &inst,
                       const vector<string> &val_type, const JsonOpt &opts) {

    return os;
}

inline static ostream& serialize_list(ostream& os, const meta_any &list_any,
                       const vector<string> &val_type, const JsonOpt &opts) {
    os << '[';
    bool first = true;

    if (auto view = list_any.as_sequence_container(); view) {
        for (auto e : view) {
            if (first) first = false; else os << ',';
            entt::meta_any element_obj = *e;
            if (element_obj) {
                serialize_val(os, element_obj, val_type, opts);
            }
            else // anerror("Element in list does not inherit from Anson!");
                serialize_val(os, e, val_type, opts);
        }
    }

    // if (AnsonAst * ast = IJsonable::contxt_ptr->ast<AnsonAst>(val_type[0]); ast) {
    //     if (!ast->isJsonable)
    //         anerror(string_view(std::format("Ast {} is not jsonable? ", ast->anclass)));
    //     else {
    //         if ("true" == val_type[1]) {
    //             if (auto view = list_any.as_sequence_container(); view) {
    //                 for (auto e : view) {
    //                     if (first) first = false; else os << ',';
    //                     entt::meta_any element_obj = *e;
    //                     if (element_obj) {
    //                         if (auto* anson_inst = element_obj.try_cast<anson::Anson>()) {
    //                             anson_inst->toBlock(os, opts);
    //                         } else {
    //                             anerror("Element in list does not inherit from Anson!");
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //         else {
    //             if (auto view = list_any.as_sequence_container(); view) {
    //                 for (auto e : view) {
    //                     if (first) first = false; else os << ',';
    //                     if (auto* p = e.try_cast<IJsonable>()) {
    //                         p->toBlock(os, opts);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    // else if (auto view = list_any.as_sequence_container(); view) {
    //         bool first = true;
    //         for (auto e : view) {
    //             if (first) first = false; else os << ',';
    //             serialize_prim_value(os, e, val_type, opts);
    //         }
    //     }
    // else
    //     anerror("Todo: list of "s + val_type[0] + ", ptr " + val_type[1]);

    return os << ']';
}

inline static ostream& serialize_map(ostream& os, const meta_any &map_any,
                       const vector<string> &val_type, const JsonOpt &opts) {
    /*
    if ("map<string, string" != dataclass) {
        anerror("TODO: map of " + dataclass);
        return os;
    }

    if (auto view = val.as_associative_container(); view) {
        os << '{';
        bool first = true;
        for (auto [key, value] : view) {
            if (first) first = false;
            else os << ", ";

            std::string k = key.cast<std::string>();
            os << '"' << k << "\": ";

            if (value.type() == entt::resolve<std::string>()) {
                os << '"' << value.cast<const std::string&>() << '"';
            }
            // Maybe it's an int?
            else if (auto* int_ptr = value.try_cast<int>()) {
                os << *int_ptr;
            }
            // Fallback: Just print the type name
            else {
                os << "\"(unknown type): " << value.type().info().name() << '"';
            }
        }
        os << '}';
    }
    return os;
    */
    os << '{';
    bool first = true;

    if (AnsonAst * ast = IJsonable::contxt_ptr->ast<AnsonAst>(val_type[0]); ast) {
        if (!ast->isJsonable)
            anerror(string_view(std::format("Ast {} is not jsonable? ", ast->anclass)));
        else {
            if ("true" == val_type[1]) {
                if (auto view = map_any.as_associative_container(); view) {
                    for (auto [k, v] : view) {
                        if (first) first = false; else os << ',';
                        entt::meta_any element_obj = *v;
                        if (element_obj) {
                            if (auto* anson_inst = element_obj.try_cast<anson::Anson>()) {
                                anson_inst->toBlock(os, opts);
                            } else {
                                anerror("Element in map is not inherit from Anson!");
                            }
                        }
                    }
                }
            }
            else {
                if (auto view = map_any.as_associative_container(); view) {
                    for (auto [k, v] : view) {
                        if (first) first = false; else os << ',';
                        if (auto* p = v.try_cast<IJsonable>()) {
                            p->toBlock(os, opts);
                        }
                    }
                }
            }
        }
    }
    else if (auto view = map_any.as_associative_container(); view) {
        bool first = true;
        for (auto [k, v] : view) {
            if (first) first = false; else os << ',';
            os << '"' << k.cast<const string&>() << "\": ";
            // serialize_anson_prim_ptr(os, v, val_type, opts);
            // serialize_prim_value(os, v, val_type[0], k.cast<string>(), opts);
            serialize_val(os, v, val_type, opts);
        }
    }
    else
        anerror("Todo: list of "s + val_type[0] + ", ptr " + val_type[1]);

    return os << '}';
}

/**
 * @brief serialize_fields
 * @param os
 * @param fields
 * @param anson
 * @param first
 * @param opts
 * @return os
 *
 * Debug Notes:
 * 1. forward_as_meta() resolves type at compile time, will fail converting Anson to subclasses.
 *
 * entt::meta_any meta_inst = ast->get_instance(anson);
 * meta_any meta_list = data.get(meta_inst);
 * *  Gemini:
 * *  fail—is because of Type Erasure and Registration Mismatch.
 * * In EnTT, forward_as_meta is a template function. Its behavior depends entirely on
 * * the static type of the argument passed to it at the call site.
 * * In your serialize_fields function, the parameter anson is likely defined as a
 * * reference to the base class: anson::Anson& anson.
 * * When you call forward_as_meta(anson), the compiler resolves this to
 * * forward_as_meta<anson::Anson>(anson).
 *
 * 2. The debugger observer effect.
 * code: meta_any meta_list2 = ast->get_field_instance(anson, fn);
 * Now meta_list2.node can be '0x0' in debugger view.
 * To observe the spooky entanglement, use this code and see the instance address:
 * * if (meta_list2.type() == entt::resolve(ast->enttypeid)) {
 * *     andebug("vector<shared_ptr< ...        ...");
 * * }
 */
inline static ostream& serialize_fields(ostream &os,
                    const map<string, AnsonField> &fields, anson::Anson& anson, const JsonOpt &opts) {

     if (fields.size() > 0) {

        AnsonAst *ast = opts.ast<AnsonAst>(anson.anclass);

        bool first = true;
        for (auto[fn, f] : fields) {
            if (opts.serialize_type && "type" == fn)
                continue;

            entt::meta_any meta_val = ast->get_field_instance(anson, fn);

            if (first) first = false; else os << ",";

            os << '\"' << fn << R"(": )";

            // vector<string> valtype = Regex::parseMapValtype(f.dataAnclass);
            serialize_val(os, meta_val, vector<string>{f.dataAnclass, f.isptr}, opts);

            // if (f.dataAnclass.starts_with("list<")) {
            //     if (ast->enttypeid != hashed_string{anson.anclass.c_str()})
            //         anerror(std::format("serialize_fields(): entt type_id mismatch: {} {}",
            //                             std::to_string(ast->enttypeid), anson.anclass));

            //     if (meta_val)
            //         serialize_list(os, meta_val, valtype, opts);
            //     else
            //         os << "null";
            // }

            // else if (f.dataAnclass.starts_with("map<")) {
            //     vector<string> valtype  = Regex::parseMapValtype(f.dataAnclass);
            //     if (meta_val)
            //         serialize_map(os, meta_val, valtype, opts);
            //     else
            //         os << "null";
            // }
            // else {
            //     AnsonAst* fd_ast = opts.ast<AnsonAst>(f.dataAnclass);
            //     if (fd_ast) {
            //         meta_type enttype = entt::resolve(ast->enttypeid);
            //         hashed_string data_key{fn.c_str()};
            //         meta_data data = find_field_recursive(enttype, data_key);

            //         entt::meta_any obj = enttype.from_void(&anson);
            //         meta_any val = data.get(obj);
            //         // IJsonable *jsonval = val.try_cast<Anson>();
            //         // if(jsonval)
            //         //     jsonval->toBlock(os, opts);
            //         // else
            //         //     anerror(std::format("Connot convert from meta_any to IJasonalbe: {}.{}",
            //         //             ast->dataAnclass, data.name()));
            //         serialize_jsonable(os, val, *fd_ast, opts);
            //     }
            //     else {
            //         vector<string> valtype  = Regex::parse_val_type(f.dataAnclass);
            //         entt::meta_any val = ast->get_field_instance(anson, fn);
            //         serialize_prim_value(os, val, valtype, opts);
            //     }
            // }
        }
    }
    return os;
}

inline static ostream& serialize_kvs(ostream &os, Anson& anson, const JsonOpt &opts) {

    AnsonAst *ast = opts.ast<AnsonAst>(anson.anclass);
    bool has_basefields = false;
    if (opts.has_ast(ast->dataBaseAst)) {
        AnsonAst *base_ast = opts.asts->at(ast->dataBaseAst).get();
        if (opts.has_ast(base_ast->dataAnclass)) {
            auto base_fields = opts.asts->at(base_ast->dataAnclass)->fields;
            serialize_fields(os, base_fields, anson, opts);

            has_basefields = base_fields.size() > 0;
        }
    }

    auto fields = ast->fields;
    if (has_basefields && fields.size() > 0) os << ",";

    return serialize_fields(os, fields, anson, opts);
}

inline static ostream& serialize_envelope(ostream &os, Anson& anson, const JsonOpt &opts) {
    os << R"({"type": ")" + anson.type + '"';

    AnsonAst *ast = opts.ast<AnsonAst>(anson.anclass);
    if (ast && ast->fields.size() > 0) os << ",";

    serialize_kvs(os, anson, opts);
    return  os << '}';
}

inline IJsonable* Anson::toBlock(ostream& os, const JsonOpt& opts) {
    serialize_envelope(os, *this, opts);
    return this;
}

struct ParseNode {
    meta_any instance;

    string val_astid;
    bool is_val_ptr = false;

    bool is_list = false;
    bool is_map = false;

    id_type activekey = 0;
    string map_key;

    std::function<meta_any()> nest_val_ctor;
};

template<typename T>
class EnTTSaxParser : public nlohmann::json_sax<nlohmann::json> {
private:
    const JsonOpt *contxt;

    id_type active_key{0};

    AnsonAst* find_field_ast(const AnsonAst *inst_ast, const std::string &fieldname) {
        AnsonAst *base_ast;
        if (inst_ast->fields.contains(fieldname))
            return contxt->ast<AnsonAst>(inst_ast->fields.at(fieldname).dataAnclass);
        else if (!LangExt::isblank(inst_ast->dataBaseAst) &&
                (base_ast = contxt->ast<AnsonAst>(inst_ast->dataBaseAst)))
            return find_field_ast(base_ast, fieldname);

        return nullptr;
    }

    template<typename V>
    bool insert_list(auto &list_v, V&& val) {
        using Tv = std::decay_t<V>;
        try {
            // Get the type the container actually expects
            auto expected_type = list_v.value_type();

            if (expected_type == entt::resolve<LangExt::VarType>()) {
                // The container is vector<VarType>.
                // We must explicitly create the variant so EnTT doesn't have to guess.
                LangExt::VarType variant_val;

                if constexpr (std::is_constructible_v<LangExt::VarType, Tv>) {
                    variant_val = std::forward<V>(val);
                } else {
                    variant_val = std::to_string(val);
                }

                // Wrap the variant in a meta_any and insert
                list_v.insert(list_v.end(), entt::meta_any{std::move(variant_val)});
            }
            else {
                // The container is vector<int>, vector<string>, etc.
                // Insert the raw value directly.
                list_v.insert(list_v.end(), std::forward<V>(val));
            }
            return true;
        } catch(...) {
            return false;
        }
    }

    template<typename V>
    void set_value(V&& val) {
        if (stack.empty()){
            anerror("set_value(): setting val to empty object");
            return;
        }

        auto& top = stack.back();

        if (top.is_list) {
            andebug(std::format("set_value(): setting value in list: {}", top.val_astid));
            auto view = top.instance.as_sequence_container();
            if (view) {
                // if ("ValType" == contxt->primtypes.at(top.val_astid))
                //     // view.insert(view.end(), LangExt::VarType{val});
                //     view.insert(view.end(), std::forward<V>(LangExt::VarType{to_string(val)}));
                // else
                //     view.insert(view.end(), std::forward<V>(val));
                insert_list(view, val);
                andebug("set_value(): List size: " + std::to_string(view.size()));
            }
            else
                anerror(std::format("set_value(): Failed to set list value: {}", val));
            return;
        }
        // ???
        else if (top.is_map) {
            andebug(std::format("set_value(): setting value in map: {}", top.map_key));
            auto view = top.instance.as_associative_container();
            if (view) {
                view.insert(top.map_key, std::forward<V>(val));
                andebug("set_value(): Map size: " + std::to_string(view.size()));
            }
            else
                anerror(std::format("set_value(): Failed to set a map pair: {}", val));
            return;
        }
        else if (active_key != 0) {
            auto& top = stack.back();

            if (contxt->primtypes.contains(top.val_astid)) {
                if (!top.is_map)
                    anerror("set_value(): Why here\n\n??????????\n\n");

                andebug("set_value(): set to supposed map");
                auto view = top.instance.as_associative_container();
                if (view) {
                    view.insert(top.map_key, std::forward<V>(val));
                }
                else
                    anerror("set_value(): Why cannot set value to map?");

                andebug("set_value(): Map size after insert: " + std::to_string(view.size()));
                return;
            }

            auto data = find_field_recursive(top.instance.type(), active_key);
            if (data) {
                std::string fieldname = data.name();
                andebug(std::format("set_value(): setting {}, active_key: {}",
                                fieldname, active_key));

                if (!contxt->has_ast(top.val_astid)) {
                    anerror(string_view("set_value(): Cannot find AST "s + top.val_astid));
                    return;
                }

                std::string string_val;
                if constexpr (std::is_same_v<std::decay_t<V>, std::string>)
                    string_val = std::forward<V>(val);
                else if constexpr (std::is_same_v<std::decay_t<V>, bool>)
                    string_val = val ? "true" : "false";
                else
                    string_val = std::to_string(val);

                AnsonAst* ast = contxt->ast<AnsonAst>(top.val_astid);
                AnsonAst *fd_ast = find_field_ast(ast, fieldname);

                if (fd_ast != nullptr) {
                    if (fd_ast->isPortEnum) {
                        auto v = data.type().construct(string_val);
                        JavaEnum *je = v.template try_cast<JavaEnum>();
                        bool res = data.set(top.instance, v);
                        return;
                    }
                    if (fd_ast->isEnum) {
                        AnsonJavaEnumAst *enum_ast = (AnsonJavaEnumAst*)fd_ast;
                        bool res = data.set(top.instance, enum_ast->get_field_instance(Anson(), string_val));
                        if (res) return;
                    }
                }
                else {
                    bool res = data.set(top.instance, std::forward<V>(val));
                    if (!res)
                        anerror("set_value(): failed to set "s + fieldname);
                }
            }
            else {
                // Debug Notes: to avoid error: SEH exception, don't call top.instance.type().name()
                anerror(std::format("set_value(): Cannot find field by key-id: {}",
                                    active_key));

                auto debug_agin = find_field_recursive(top.instance.type(), active_key);
            }
        }
    }

public:
    std::deque<ParseNode> stack;

    /**
     * Parser for AST loading.
     * When parsing AST strings, the ast_id can be different to obj.anclass.
     *
     * @brief EnTTSaxParser
     * @param obj
     * @param ast_id
     * @param opts
     */
    EnTTSaxParser(T& obj, std::string ast_id, const JsonOpt *opts = nullptr) : contxt(opts) {
        push(obj, ast_id);
        contxt = opts == nullptr ? IJsonable::contxt_ptr : opts;
        active_key = 0;
    }

    EnTTSaxParser(T& obj, const JsonOpt *opts = nullptr) : EnTTSaxParser(obj, obj.anclass, opts) {}

    bool start_object(std::size_t size) override {
        bool k0 = active_key != 0;
        bool es = !stack.empty();

        if (active_key != 0 && !stack.empty()) {
            ParseNode top = stack.back();
            meta_type type = top.instance.type();
            auto datafield = find_field_recursive(type, active_key);
            if (datafield) {
                std::string fieldname = datafield.name();
                andebug("start_obj(): Starting object, field name: "s + fieldname);

                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);
                    std::string fd_astid;

                    // Notes 26 Mar 2026:
                    // Fields is the definition of an AST, and must be merged back to a being loading AST.
                    // The loading of AST according to AST stop the recursive traversal here.
                    // As fields itself has no chances to register the type of a map.
                    if ("fields" == fieldname && IJsonable::contxt_ptr->is_ast(ast->type)) {
                        fd_astid = "map<string,"s + AnsonField_type;
                        meta_any inst = datafield.get(stack.back().instance);
                        AnsonField field_confg = ast->fields[fieldname];
                        field_confg.fieldname = fieldname;
                        push_map(inst, active_key, fd_astid, // ast->fields.at(fieldname).nest_val_ctor);
                                 [field_confg]() { return AnsonField{.fieldname=field_confg.fieldname, .dataAnclass=field_confg.dataAnclass};});
                    }

                    else {
                        if (!ast->fields.contains(fieldname))
                            anerror(std::format(
                                "start_obj(): AST {{anclass: {}, datatype: {}}} has no field {}.",
                                ast->anclass, ast->dataAnclass, fieldname));

                        else
                            fd_astid = ast->fields.at(fieldname).dataAnclass;

                        if (contxt->asts->contains(fd_astid))
                            stack.push_back({.instance = datafield.get(stack.back()),
                                 .val_astid=fd_astid});
                        else { // e.g. map<string, string
                            meta_any inst = datafield.get(stack.back().instance);
                            push_map(inst, active_key, fd_astid, ast->fields.at(fieldname).nest_val_ctor);
                        }
                    }
                }
            } else if (top.is_map) {
                // e.g. val_type = AnsonField in a Map, the top.instance. String in map won't reach here, starting obj.
                std::string fieldname = top.map_key;
                andebug(std::format("start_obj(): Starting object in map, field key: {}", active_key));

                std::string fd_astid;
                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);
                    fd_astid = top.val_astid;

                    meta_type type = resolve(hashed_string{fd_astid.c_str()});
                    if (type) {
                        meta_any inst = type.construct();
                        if (contxt->asts->contains(fd_astid))
                            stack.push_back({.instance = inst, .val_astid=fd_astid, .activekey=active_key});
                        else
                            push_map(inst, active_key, fd_astid);
                    }
                    else {
                        anerror("start_obj(): Primative types can not be here?");
                        anerror("start_obj(): Cannot start an object in a map with ast id:"s + fd_astid);
                    }
                }
            } else if (top.is_list) {
                // top.map_key should be empty;
                andebug(std::format("start_obj(): Starting object in list, field key: {}", active_key));

                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);

                    std::string fd_astid;
                    fd_astid = top.val_astid;
                    meta_type type = resolve(hashed_string{fd_astid.c_str()});
                    if (type) {
                        meta_any inst = type.construct();
                        if (contxt->asts->contains(fd_astid))
                            stack.push_back({.instance = inst, .val_astid=fd_astid, .activekey=active_key});
                        else
                            push_map(inst, active_key, fd_astid);
                    }
                    else {
                        anerror("start_obj(): Primative types can not be here?");
                        anerror("start_obj(): Cannot start an object in a list with ast id:"s + fd_astid);
                    }
                }
                else
                    anerror("start_obj(): Cannot find ast "s + top.val_astid);

            } else {
                anerror(std::format(
                    "start_obj(): Starting object, cannot find object field, key: {}, type: {}, top.map_key: {}",
                    std::to_string(active_key), top.val_astid, top.map_key));

                anerror(type.info().name());
                return true;
            }
        } else if (stack.empty()) {
            // This is the root object.
            anerror("start_obj(): Starting object with empty stack.");
            return true;
        }
        return true;
    }

    bool key(string_t& val) override {
        active_key = hashed_string{val.c_str()};
        andebug(std::format("key(): deserializing key {}, key-id: {}", val, active_key));

        if (!stack.empty()) {
            stack.back().map_key = val;
        }

        return true;
    }

    bool end_object() override {
        if (stack.size() > 1) {
            ParseNode top = stack.back();
            id_type key0 = top.activekey;
            stack.pop_back();

            if (!stack.empty() && key0 != 0) {
                auto data = find_field_recursive(stack.back().instance.type(), key0);
                if (data) {
                    data.set(stack.back().instance, top.instance);
                }
                else if (stack.back().is_map) {
                    meta_type val_type = resolve(hashed_string{stack.back().val_astid.c_str()});
                    meta_type obj_type = top.instance.type();
                    andebug(std::format("end_obj(): fd_type.id() {}, map_type.id() {}", val_type.id(), obj_type.id()));
                    if (val_type.id() == obj_type.id()) {
                        auto view = stack.back().instance.as_associative_container();
                        if (view) {
                            entt::meta_any key{std::move(stack.back().map_key)};

                            bool success = view.insert(std::move(key), top.instance);

                            if (!success) {
                                anerror("end_obj(): Handle failure (type mismatch or key already exists");
                            }
                        }
                    }
                }
                else if (stack.back().is_list) {
                    meta_type obj_type = top.instance.type();
                    std::string typname{obj_type.info().name()};
                    andebug(std::format("end_obj(): list-container.type-id() {}, info: {}", obj_type.id(), typname));
                        auto view = stack.back().instance.as_sequence_container();
                        if (view) {

                            meta_any success;
                            if (stack.back().is_val_ptr) {
                                auto func = obj_type.func("create_ptr"_hs);

                                if (!func) {
                                    anerror(std::format("end_obj(): function create_ptr({}) is not registered.", typname));
                                    return false;
                                }

                                andebug(std::format("end_obj(): Stack back instance's func<create_ptr> arity: {}",
                                                                func.arity()));

                                success = obj_type.func("create_ptr"_hs).invoke(top.instance);
                                if (success)
                                    success = view.insert(view.end(), success);
                            }
                            else
                                success = view.insert(view.end(), top.instance);

                            if (!success)
                                anerror("end_obj(): Setting back the list is failed!");
                            else
                                andebug(std::format("end_obj(): Ok! Size of the inserted list of obj: {}",
                                                                view.size()));
                        }
                }
            }
            active_key = key0;
        }
        // else: TODO needing a stack resetting for re-enter.
        return true;
    }

    // 2. Data Type Handling
    bool number_float(number_float_t val, const string_t&) override {
        set_value(static_cast<float>(val));
        return true;
    }

    bool number_integer(number_integer_t val) override {
        set_value(static_cast<int>(val));
        return true;
    }

    bool string(string_t& val) override {
        andebug(std::format("string(): {}, top.is_list: {}, top.is_map: {}, map_key: {}",
                val, stack.back().is_list, stack.back().is_map, stack.back().map_key));

        set_value(val);
        return true;
    }

    bool boolean(bool val) override {
        set_value(val);
        return true;
    }

    bool null() override { return true; }

    bool number_unsigned(number_unsigned_t val) override {
        set_value(static_cast<uint64_t>(val));
        return true;
    }

    bool binary(binary_t&) override { return true; }
    bool start_array(std::size_t) override {

        andebug(std::format("start_array(): [0] list container addr: {:P}",
                (void*)stack.front().instance.try_cast<Anson>()));

        if (active_key != 0 && !stack.empty()) {
            auto data = find_field_recursive(stack.back().instance.type(), active_key);
            if (data) {
                std::string fieldname = data.name();
                andebug(std::format("start_array(): starting, field key {}, name {}",
                                    active_key, fieldname));

                std::string val_astid;
                AnsonAst *ast = contxt->ast<AnsonAst>(stack.back().val_astid);
                if (!ast) {
                    anerror(std::format("start_array(): Cannot find list value type {}.",
                                        stack.back().val_astid));
                }
                else if (!ast->fields.contains(fieldname))
                    anerror(std::format(
                        "start_array(): AST {{anclass: {}, datatype: {}}} has no field {}.",
                        ast->anclass, ast->dataAnclass, fieldname));
                else
                    val_astid = ast->fields.at(fieldname).dataAnclass;

                meta_any list = data.get(stack.back().instance);
                push_list(list, active_key, val_astid, ast->fields.at(fieldname).nest_val_ctor);

                andebug(std::format("start_array(): [1] list container addr: {:P}",
                        (void*)stack.front().instance.try_cast<Anson>()));
            }
            else if (stack.back().is_map) { // map<string, list<...
                vector<std::string> valtype  = Regex::parseListValtype(stack.back().val_astid); // debug
                if (!stack.back().nest_val_ctor) {
                    anerror("start_array(): Not able to create list value without val_ctor in map of <"
                            + stack.back().val_astid);
                    return false;
                }
                else {
                    meta_any list = stack.back().nest_val_ctor();
                    // push_list(list, active_key, valtype[0]);
                    push_list(list, active_key, stack.back().val_astid);
                }
            }
            else if (stack.back().is_list) { // list<list<...
                vector<std::string> valtype  = Regex::parseListValtype(stack.back().val_astid); // debug
                if (!stack.back().nest_val_ctor) {
                    anerror("start_array(): Not able to create list value without val_ctor in list of <"
                            + stack.back().val_astid);
                    return false;
                }
                else {
                    meta_any list = stack.back().nest_val_ctor();
                    // push_list(list, active_key, valtype[0]);
                    push_list(list, active_key, stack.back().val_astid);
                }
            }
        }
        return true;
    }

    bool end_array() override {

        Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
        andebug(std::format("end_array(): Stack front addr: {:P}", (void*)stack_ptr));

        if (!stack.empty() && stack.back().is_list) {
            auto finished_list = stack.back().instance;
            id_type key_to_update = stack.back().activekey;
            std::string field_to_update = stack.back().map_key;
            id_type field_id = hashed_string(field_to_update.c_str());

            stack.pop_back();

            if (!stack.empty() && key_to_update != 0) {
                auto data = find_field_recursive(stack.back().instance.type(), key_to_update);
                if (data) {
                    andebug(std::format(
                        "end_array(): Setting back list {} : {}, size: {:#x} -> {}",
                        data.name(), key_to_update,
                        finished_list.as_sequence_container() ? finished_list.as_sequence_container().size() : -1,
                        data.get(stack.back().instance).as_sequence_container().size()));

                    bool res = data.set(stack.back().instance, finished_list);

                    Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
                    andebug(std::format("end_array(): Stack back addr: {:P}", (void*)stack_ptr));

                    if (res) {
                        andebug(std::format("end_array(): The reference of this list is found. size: {}, field: {}, key-id: {}",
                                            data.get(stack.back().instance).as_sequence_container().size(),
                                            data.name(), key_to_update));
                    }
                    else
                        anerror("end_array(): Failed to set back (copy) "s + data.name());

                    if (Anson* v = stack.back().instance.try_cast<anson::Anson>())
                        // Why serialize_jsonable(): Connot convert from meta_any to IJasonalbe: io.odysz.anson.T_List ?
                        andebug(v->toBlock(*IJsonable::contxt_ptr));
                }
                else if (stack.back().is_map) {
                    if (!stack.back().nest_val_ctor) {
                        anerror("end_array(): Not able to create map value without val_ctor in map of <"
                                + stack.back().val_astid);
                        return false;
                    }
                    else {
                        auto view = stack.back().instance.as_associative_container();
                        view.insert(stack.back().map_key, finished_list);
                        andebug(std::format("end_array(): map [{}] , size {}",
                                            stack.back().map_key, view.size()));
                    }
                }
                else if (stack.back().is_list) {
                    if (!stack.back().nest_val_ctor) {
                        anerror("end_array(): Not able to create map value without val_ctor in list of <"
                            + stack.back().val_astid);
                        return false;
                    }
                    else {
                        auto view = stack.back().instance.as_sequence_container();
                        view.insert(view.end(), finished_list);
                        andebug(std::format("end_array(): list {}, size {}", stack.back().map_key, view.size()));
                    }
                }
                else
                    anerror("Can't be here, field rather than list, map, or an Anson triggered ending arrays ...");
            }
        }
        return true;
    }


    /////////////////////////////////////////////////////////////////
    bool parse_error(std::size_t, const std::string&, const nlohmann::detail::exception&) override {
        return false;
    }

    void push(T &obj, std::string astid) {
        stack.push_back({.instance = forward_as_meta(obj),
                         .val_astid=astid, .is_list=false, .is_map=false,
                         .activekey=0});
    }

    void push_list(meta_any ref, id_type active_key, std::string list_type, function<meta_any()> nestval_ctor = nullptr) {
        vector<std::string> val_anclass = Regex::parseListValtype(list_type);
        andebug(std::format("List value data class: {}, ptr {}", val_anclass[0], val_anclass[1]));

        bool is_ptr = val_anclass[1] == "true";

        stack.push_back({.instance = ref,
                         .val_astid=val_anclass[0],
                         .is_val_ptr=is_ptr,
                         .is_list=true, .is_map=false,
                         .activekey=active_key, .nest_val_ctor=nestval_ctor});
    }

    void push_map(meta_any &map_inst, const id_type active_key, const std::string & map_type, function<meta_any()> nestval_ctor = nullptr) {

        vector<std::string> val_anclass = Regex::parseMapValtype(map_type);
        andebug(std::format("push_map(): Map value data class: {}, ptr {}", val_anclass[0], val_anclass[1]));

        stack.push_back({.instance = map_inst,
                         .val_astid=val_anclass[0],
                         .is_val_ptr=(val_anclass[1] == "true"),
                         .is_list=false, .is_map=true,
                         .activekey=active_key, .nest_val_ctor=nestval_ctor});
    }
};

template<typename T>
inline static bool parse(const string& json, T &an, const JsonOpt *opts) {
    EnTTSaxParser handler{an, opts};
    return nlohmann::json::sax_parse(json, &handler);
}

inline static vector<string> to_aststring(const AstMap &asts) {
    vector<string> sv;
    sv.push_back("astid: ast.dataAnclass -> ast.dataBaseAst");
    for (auto& [k, v] : asts) {
        string fields;
        for (auto& [fn, f] : v->fields)
            fields += std::format("\n\t{}: {}", f.fieldname, f.dataAnclass);
        sv.push_back(std::format("{}: {} -> {} [{}\n\t]", k, v->dataAnclass, v->dataBaseAst, fields));
    }
    return sv;
}
}
