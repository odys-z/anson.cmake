#include <gtest/gtest.h>
// #include <entt/meta/meta.hpp>
// #include <entt/meta/factory.hpp>
// #include <nlohmann/json.hpp>
// #include "io/odysz/anson.h"
#include "t02_entt_basic.h"

// namespace anson {

// class T_List : public Anson {
// public:
//     inline static const string _type_ = "io.odysz.anson.T_List";

//     vector<string> val;

//     string txt;

//     T_List() : Anson(_type_), txt("By Ctor") {  anlog(string_view("T_List Constructor")); }
// };

// class T_List2DPtr : public Anson {
// public:
//     inline static const string _type_ = "io.odysz.anson.T_List2DPtr";

//     // Parsing pointer vector, to be continued.
//     vector<shared_ptr<T_List>> vpp;

//     T_List2DPtr() : Anson(_type_) {}
// };

// void register_asts(AstMap &asts) {
//     hashed_string enttype;
//     string anclass;
//     //
//     enttype = hashed_string{"io.odysz.anson.IJasonable"};
//     entt::meta_factory<anson::IJsonable>()
//         // .type(enttype)
//         .type(IJsonable::_anclass_.c_str())
//         .data<&anson::IJsonable::anclass>("anclass")
//         ;

//     //
//     enttype = hashed_string{Anson::_type_.c_str()};
//     entt::meta_factory<anson::Anson>()
//         // .type(enttype)
//         .type(Anson::_type_.c_str())
//         .base<IJsonable>()
//         .ctor<>()
//         .ctor<const std::string&>()
//         .data<&anson::Anson::type>("type")
//         ;

//     // using T_List_shared_ptr = shared_ptr<anson::T_List>;
//     enttype = hashed_string{T_List::_type_.c_str()};
//     entt::meta_factory<anson::T_List>()
//         // .type(enttype)
//         .type(T_List::_type_.c_str())
//         .base<Anson>()
//         .ctor<>()
//         .func<+[](const T_List &inst) -> std::shared_ptr<anson::T_List> {
//             anlog(std::format("T_List.func<create_ptr>(const inst)"));
//             return std::make_shared<anson::T_List>(inst);
//         }>("create_ptr")

//         // works with EnttSAXParse.end_object() is_list:
//         // success = obj_type.func("create_ptr"_hs).invoke({});
//         //
//         // .func<+[]() -> std::shared_ptr<anson::T_List> {
//         //     anlog(string_view("T_List.func<create_ptr>()"));
//         //     return std::make_shared<anson::T_List>();
//         // }>("create_ptr")

//         .data<&anson::T_List::val>("val")
//         .data<&anson::T_List::txt>("txt")
//         ;

//     anclass = T_List().anclass;
//     AnsonAst *ast = new AnsonAst{anclass, false};
//     ast->dataAnclass = T_List::_type_;
//     ast->base = Anson::_type_;
//     ast->enttypeid = enttype;

//     // ast.fields is only used for serialization?
//     ast->fields = map<string, AnsonField>{
//         {"txt",   {.fieldname="txt", .dataAnclass = "string"}},
//         {"val",   {.fieldname="val", .dataAnclass = "list<string"}}
//     };

//     ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
//         if (!ast->fields.contains(fieldname))
//             return meta_any{false};

//         auto& concrete = static_cast<const T_List&>(ans);

//         if ("txt" == fieldname)
//             // return {concrete.txt}; -> auto *s = return-meta.try_cast<stirng>(); os << *s;
//             return entt::forward_as_meta(concrete.txt);
//         if ("val" == fieldname)
//             return entt::forward_as_meta(concrete.val);

//         anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
//         return {};
//     };

//     asts[anclass] = unique_ptr<AnsonAst>(ast);
// }

// void register_2DPtr_asts_callback(AstMap &asts) {
//     string anclass;
//     hashed_string enttype;

//     entt::meta_factory<std::shared_ptr<anson::T_List>>()
//         .type(("shared_ptr<"s + T_List::_type_).c_str());

//     entt::meta_factory<std::vector<std::shared_ptr<anson::T_List>>>()
//         .type(("vector<shared_ptr<"s + T_List::_type_).c_str());

//     entt::meta_factory<anson::T_List2DPtr>()
//         .type(T_List2DPtr::_type_.c_str())
//         .base<Anson>()
//         .ctor<>()
//         .data<&anson::T_List2DPtr::vpp>("vpp")
//         ;

//     anclass = T_List2DPtr().anclass;
//     AnsonAst *ast = new AnsonAst{anclass, false};
//     ast->dataAnclass = T_List2DPtr::_type_;
//     ast->dataBaseAst = Anson::_type_;
//     ast->base = Anson::_type_;
//     ast->enttypeid = hashed_string{T_List2DPtr::_type_.c_str()};

//     ast->fields = map<string, AnsonField>{
//       {"vpp",   {.fieldname="vpp", .dataAnclass = "list<shared_ptr<"s + T_List::_type_}},
//     };

//     /* Returned something. Depends on entt reflection or Anson semantics generation?
//     ast->get_entt_instance = [](IJsonable& inst, id_type key) -> meta_any {
//         auto& concrete = static_cast<T_List2DPtr&>(inst);

//         auto type = entt::resolve<T_List2DPtr>();

//         if (auto data = find_field_recursive(type, key); data) {
//             if (!entt::resolve(data.type().id())) {
//                 anerror("Field type is not registered in EnTT meta system!");
//             }
//             meta_any d = data.get(concrete);
//             if (d)
//                 return d;
//         }
//         anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
//         return {};
//     };
//     */

//     ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
//         if (!ast->fields.contains(fieldname))
//             return meta_any{false};

//         auto& concrete = static_cast<const T_List2DPtr&>(ans);

//         if ("vpp" == fieldname)
//             return entt::forward_as_meta(concrete.vpp);

//         anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
//         return {};
//     };

//     asts[anclass] = unique_ptr<AnsonAst>(ast);
// }
// }

TEST(ENTT, Serialize_T_List) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_2DPtr_asts_callback(asts);

    T_List inst_list;
    T_List &anlist = inst_list;

    anlist.val = vector<string>{"x.x", "y.y"};
    string json = inst_list.toBlock(contxt);
    cout << json << endl;
    ASSERT_EQ(R"({"type": "io.odysz.anson.T_List","txt": "By Ctor","val": ["x.x","y.y"]})", json);
}

TEST(ENTT, Serialize_2DPtr) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_2DPtr_asts_callback(asts);

    T_List2DPtr inst_list;
    T_List2DPtr &anlist = inst_list;

    T_List tl0;
    tl0.txt = "t-0";
    tl0.val = vector<string>{"0.0", "0.1"};
    T_List tl1;
    tl1.txt = "t-1";
    tl1.val = vector<string>{"1.0", "1.1"};

    inst_list.vpp = vector<shared_ptr<T_List>>{
        std::make_shared<T_List>(tl0),
        std::make_shared<T_List>(tl1)
    };

    string json = inst_list.toBlock(contxt);

    cout << json << endl;

    ASSERT_EQ(R"({"type": "io.odysz.anson.T_List2DPtr",)"
              R"("vpp": [{"type": "io.odysz.anson.T_List","txt": "t-0","val": ["0.0","0.1"]},)"
                      R"({"type": "io.odysz.anson.T_List","txt": "t-1","val": ["1.0","1.1"]}]})",
              json);
}
