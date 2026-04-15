#include <gtest/gtest.h>
#include "io/odysz/json.h"
#include "t02_entt_basic.h"

TEST(ENTT, Serialize_T_List) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_T_List(asts);
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
    register_T_List(asts);
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
