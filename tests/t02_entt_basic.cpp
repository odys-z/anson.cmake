#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include "io/odysz/anson.h"


namespace anson {

class T_List : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List";

    vector<string> val;

    string txt;

    T_List() : Anson(_type_) {}
};

void register_asts(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {
    hashed_string enttype;
    string anclass;
    //
    enttype = hashed_string{"io.odysz.anson.IJasonable"};
    entt::meta_factory<anson::IJsonable>()
        .type(enttype)
        .data<&anson::IJsonable::anclass>("anclass")
        ;

    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    //
    enttype = hashed_string{T_List::_type_.c_str()};
    entt::meta_factory<anson::T_List>()
        .type(enttype)
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List::val>("val")
        .data<&anson::T_List::txt>("txt")
        ;
}
}

TEST(ENTT, T_LIST_GENERIC_SEQUENCE) {
    using namespace entt::literals;
    using namespace anson;

    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;
    anson::register_asts(asts, enttypes);

    auto type = entt::resolve<anson::T_List>();
    entt::meta_any instance = type.construct();
    T_List &tlist = instance.cast<T_List&>();
    auto data_member = type.data("val"_hs);

    entt::meta_any val_any = data_member.get(instance);

    andebug(tlist.val);

    auto view = val_any.as_sequence_container();

    if (view) {
        entt::meta_any newValue = std::string("Generic Insert");

        // This now modifies the vector inside 'instance'
        view.insert(view.end(), std::move(newValue));
        andebug(string_view(std::format("{}, {}", view.size(), tlist.val.size())));
    }

    // Verification
    auto& final_vec = instance.cast<anson::T_List&>().val;
    EXPECT_EQ(1, view.size());
    EXPECT_EQ(0, final_vec.size());
    vector<string>& val = final_vec;

    vector<string> vec{"New Insert"};
    data_member.set(instance, vec);

    EXPECT_EQ("New Insert", val[0]);

    meta_any inst = forward_as_meta(tlist);
    vec.push_back("A");
    data_member.set(inst, vec);
    EXPECT_EQ("A", tlist.val[1]);
}

TEST(ENTT, T_LIST_PARSE) {
    using namespace entt::literals;
    using namespace anson;

    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts, enttypes);

    T_List anlist;
    T_List &l2 = anlist;
    std::string json_input = std::format(R"({{"type": "{}", "txt": "text", "val": ["a", "b"]}})", T_List::_type_);
    // EnTTSaxParser handler(l2, IJsonable::contxt_ptr);
    EnTTSaxParser handler(anlist, IJsonable::contxt_ptr);

    cout << "[0] " << json_input << endl;
    anlist.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ(T_List::_type_, anlist.anclass) << "anlist.anclass";
    // ASSERT_EQ(T_List::_type_, anlist.type) << "anlist.type";

    ASSERT_EQ("text", anlist.txt) << "anlist.txt";
    ASSERT_EQ((vector<string>{"a", "b"}), l2.val) << "anlist.val";
    ASSERT_EQ((vector<string>{"a", "b"}), anlist.val) << "anlist.val";
}
