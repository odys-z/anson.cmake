
class AnsonBody : public anson::Anson {
public:
    inline static const string _type_ = "io.odysz.jprotocol.AnsonBody";

    string a;

    AnsonBody() : Anson(_type_) {}

    AnsonBody(string a) : Anson(_type_) , a(a) {}

    AnsonBody(string a, string type) : Anson(type), a(a) {}
};

class T03Req : public AnsonBody {
    int t03
};

template <
    typename T //anson::AnsonBody
    >
class AnsonMsg: public Anson {
public:
    inline static const std::string _type_ = "io.odysz.jprotocol.AnsonMsg";

    vector<shared_ptr<T>> body;
}

