#include <iostream>
#include <fstream>
#include <set>

#include <parselib/XML/xml.hpp>

using namespace std;
using namespace ParseLib;

void printSTree(const XML::Node& node,string off="",bool root = false)
{
    if (node.isTextNode())
        cout << off << "\"" << dynamic_cast<const XML::TextNode&>(node).text << "\"" << endl;
    else
    {
        const XML::Tag& tag = dynamic_cast<const XML::Tag&>(node);
        if (root)
        {
            cout << off << ">root";
            if (tag.name.size())
                cout << "[" << tag.name << "]";
        }
        else
            cout << off << tag.name;
        if (tag.attributes.size())
        {
            cout << " (";
            bool first = true;
            for (const auto& i:tag.attributes)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    cout << ",";
                }
                cout << i.first;
                if (i.second.val.size())
                {
                    cout << "=\"" << i.second.val << "\"";
                }
            }
            cout << ")";
        }
        cout << endl;
        for (unsigned int i=0; i < tag.children.size(); ++i)
            printSTree(*tag.children[i].get(),off+"  ");
    }
}

void printTree(const XML::Tag& root)
{
    printSTree(root,"",true);
}

string loadFile(const string& source)
{
    string tmp;
    string out;
    ifstream in(source.c_str());
    if (in.good())
    {
        while (getline(in,tmp))
        {
            out += tmp;
        }
        in.close();
    }
    else
    {
        cout << "Not found." << endl;
    }
    return std::move(out);
}

int main(int argc,char* argv[])
{
    if (argc > 1)
    {
        try
        {
            set<string> flags;
            vector<string> args;
            string a;
            if (argc == 2)
            {
                a = loadFile(argv[1]);
            }
            else if (argc > 2)
            {
                for (int i = 1; i < argc; ++i)
                {
                    a = argv[i];
                    if (a.size())
                    {
                        if (a[0] == '-' || a[0] == '/')
                        {
                            a = std::move(a.substr(1));
                            if (flags.find(a) == flags.end())
                                flags.insert(a);
                        }
                        else
                        {
                            args.push_back(a);
                        }
                    }
                }
                if (flags.find("f") != flags.end())
                {
                    if (flags.size() > 1)
                    {
                        cout << "1 or more flags ingored(-f used instead)." << endl;
                    }
                    if (args.size() != 1)
                    {
                        throw "Invalid argument count.";
                    }
                    cout << args[0] << endl;
                    a = loadFile(args[0]);
                }
                else if (flags.find("s") != flags.end())
                {
                    if (flags.size() > 1)
                    {
                        cout << "1 or more flags ingored(-s used instead)." << endl;
                    }
                    if (args.size() != 1)
                    {
                        throw "Invalid argument count.";
                    }
                    a = args[0];
                }
            }
            else
                return 0;
            XML::Tag t = XML::parse(a);
            cout << t.toString(false) << endl;
        }
        catch(const char* err)
        {
            cout << err << endl;
        }
    }
    return 0;
}
