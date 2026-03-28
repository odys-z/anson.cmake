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
        ;
}
}


TEST(ENTT, TLIST) {
    using namespace anson;
    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;
    anson::register_asts(asts, enttypes);

    // // 1. Create the instance via reflection
    // auto type = entt::resolve<anson::T_List>();
    // ASSERT_TRUE(type);

    // entt::meta_any instance = type.construct();
    // ASSERT_TRUE(instance);

    // // 2. Manipulate 'val' (vector<string>) using the meta system
    // // We look up the data member by the hashed string "val"_hs you registered
    // auto data = type.data("val"_hs);
    // ASSERT_TRUE(data);

    // // Get a reference to the actual vector inside the meta_any
    // if (auto* list_ptr = instance.try_cast<anson::T_List>()) {
    //     // Direct manipulation
    //     list_ptr->val.push_back("Hello");
    //     list_ptr->val.push_back("EnTT");
    //     list_ptr->val.push_back("Anson");
    // }

    // // 3. Verify the data
    // auto &list_ref = instance.cast<anson::T_List&>();
    // EXPECT_EQ(list_ref.val.size(), 3);
    // EXPECT_EQ(list_ref.val[1], "EnTT");

    // // 4. (Optional) Testing via meta_any specifically
    // // This simulates how a JSON deserializer would use your registration
    // auto val_any = data.get(instance);
    // auto &vec = val_any.cast<std::vector<std::string>&>();
    // EXPECT_EQ(vec.back(), "Anson");

    // T_List &anlist = instance.cast<anson::T_List&>();
    // andebug(anlist.val);
    // EXPECT_EQ(anlist.val.back(), "Anson");
}

TEST(ENTT, T_LIST_GENERIC_SEQUENCE) {
    using namespace entt::literals;
    using namespace anson;

    auto type = entt::resolve<anson::T_List>();
    entt::meta_any instance = type.construct();
    T_List &tlist = instance.cast<T_List&>();
    auto data_member = type.data("val"_hs);

    // CRITICAL FIX: Get the field as a REFERENCE
    // .as_ref() ensures we are pointing to the vector inside 'instance'
    entt::meta_any val_any = data_member.get(instance).as_ref();

    auto view = val_any.as_sequence_container();

    if (view) {
        entt::meta_any newValue = std::string("Generic Insert");

        // This now modifies the vector inside 'instance'
        view.insert(view.end(), std::move(newValue));
        andebug(string_view(std::format("{}, {}", view.size(), tlist.val.size())));
    }

    // Verification
    auto& final_vec = instance.cast<anson::T_List&>().val;
    EXPECT_EQ(1, final_vec.size());
    vector<string>& val = final_vec;
    // EXPECT_EQ("Generic Insert", final_vec[0]);
    EXPECT_EQ("Generic Insert", val[0]);
}

// TEST(ENTT, T_LIST_PARSE) {
//     aninfo(string_view(filesystem::current_path().string()));

//     map<string, AnsonAst> asts;
//     map<string, meta_type> enttypes;
//     JsonOpt contxt{&asts, &enttypes};
//     IJsonable::contxt_ptr = &contxt;

//     register_asts(asts, enttypes);


//     T_List anlist;
//     std::string json_input = std::format(R"({{"type": "{}", "val": ["a", "b"]}})", T_List::_type_);
//     EnTTSaxParser handler(anlist, IJsonable::contxt_ptr);

//     cout << "[0] " << json_input << endl;
//     anlist.type = "";
//     bool result = nlohmann::json::sax_parse(json_input, &handler);

//     ASSERT_TRUE(result);
//     ASSERT_EQ(T_List::_type_, anlist.anclass) << "anlist.anclass";
//     ASSERT_EQ(T_List::_type_, anlist.type) << "anlist.type";

//     ASSERT_EQ((vector<string>{"a", "b"}), anlist.val) << "anlist.val";
// }
