#include <iostream>

using namespace std;

#include <parselib/JSON/json.h>
#include <parselib/CORE/core.h>

int main()
{
    string a=R"XD({
    "type":"text",
    content:
    [
        "1",
        "2"
    ],
    :t
})XD";
    cout << a << endl << endl;
    try
    {
        ParseLib::JSON::Value::Type b = ParseLib::JSON::parse(a);
        if (b)
        {
            cout << b->toString(true,0,true);
        }
    }
    catch(const ParseLib::ParseException& c)
    {
        cout << c.what() << endl;
    }
    return 0;
}
