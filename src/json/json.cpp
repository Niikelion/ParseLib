#include <json.hpp>
#include <core.hpp>

namespace ParseLib
{
    namespace JSON
    {
        bool checkNameC(char a)
        {
            return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || a == '_' || a=='.';
        }
        bool checkFC(char a)
        {
            return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || a == '_';
        }
        bool checkFPC(char a)
        {
            return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || a == '_';
        }

        bool Value::toBoolean() const
        {
            return true;
        }
        double Value::toNumber() const
        {
            return nan("");
        }
        std::vector<Value::Type> Value::toArray() const
        {
            return std::move(std::vector<Value::Type>());
        }
        std::map<std::string,Value::Type> Value::toProperties() const
        {
            return std::move(std::map<std::string,Value::Type>());
        }

        std::string Undefined::toString(bool json,unsigned offset,bool compacted) const
        {
            return "undefined";
        }

        std::type_index Undefined::type() const
        {
            return typeid(Undefined);
        }

        bool Undefined::toBoolean() const
        {
            return false;
        }

        Value::Type Number::from(double a)
        {
            return std::static_pointer_cast<Value>(std::make_shared<Number>(a));
        }

        std::string Number::toString(bool json,unsigned offset,bool compacted) const
        {
            return std::to_string(value);
        }

        std::type_index Number::type() const
        {
            return typeid(Number);
        }

        double Number::toNumber() const
        {
            return value;
        }

        Number::Number()
        {
            value = 0.0;
        }

        Number::Number(double a)
        {
            value = a;
        }

        std::string Boolean::toString(bool json,unsigned offset,bool compacted) const
        {
            return (value?"true":"false");
        }

        std::type_index Boolean::type() const
        {
            return typeid(Boolean);
        }

        bool Boolean::toBoolean() const
        {
            return value;
        }

        Boolean::Boolean()
        {
            value = false;
        }

        Boolean::Boolean(bool a)
        {
            value = a;
        }

        Value::Type String::from(const std::string& s)
        {
            return std::static_pointer_cast<Value>(std::make_shared<String>(s));
        }
        std::string String::toString(bool json,unsigned offset,bool compacted) const
        {
            return json?("\""+value+"\""):value;
        }

        std::type_index String::type() const
        {
            return typeid(String);
        }

        bool String::toBoolean() const
        {
            return value == "true";
        }

        double String::toNumber() const
        {
            char* end;
            return std::strtod(value.c_str(),&end);
        }

        String::String(const std::string& a)
        {
            value = a;
        }

        std::string Array::toString(bool json,unsigned offset,bool compacted) const
        {
            std::string ret = "[";

            if (compacted)
            {
                bool f = false;
                for (const auto& i: value)
                {
                    if (f) ret += ',';
                    else f = true;
                    ret += i -> toString(json,0,true);
                }
                ret += ']';
            }
            else
            {
                bool f=false;
                for (const auto& i: value)
                {
                    if (f) ret += ',';
                    else f = true;
                    ret += '\n';
                    ret += std::string(static_cast<size_t>(offset)+1,'\t')+(i -> toString(json,offset+1,false));
                }
                ret += "\n"+std::string(offset,'\t')+']';
            }

            return ret;
        }

        std::type_index Array::type() const
        {
            return typeid(Array);
        }

        std::vector<Value::Type> Array::toArray() const
        {
            return value;
        }

        std::string Object::toString(bool json,unsigned offset,bool compacted) const
        {
            std::string ret = "{";

            if (compacted)
            {
                bool f=false;
                for (const auto& i: value)
                {
                    if (f) ret += ',';
                    else f = true;
                    ret += '"'+i.first+"\":"+i.second -> toString(json,0,true);
                }
                ret += '}';
            }
            else
            {
                bool f=false;
                for (const auto& i: value)
                {
                    if (f) ret+=',';
                    else f=true;
                    ret += '\n';
                    ret += std::string(static_cast<size_t>(offset)+1,'\t')+'"'+i.first+"\":"+(i.second -> toString(json,offset+1,false));
                }
                ret += "\n"+std::string(offset,'\t')+'}';
            }

            return ret;
        }

        std::type_index Object::type() const
        {
            return typeid(Object);
        }

        std::map<std::string,Value::Type> Object::toProperties() const
        {
            return value;
        }

        Object::Object(const std::map<std::string,Value::Type>& a)
        {
            value = a;
        }

        std::string stringify(const Value::Type& source)
        {
            return source -> toString();
        }

        Value::Type parse(const std::string& source)
        {
            if (!source.size())
                return std::make_shared<Undefined>();
            std::string tmps;
            size_t pos = source.find_first_not_of(" \t\r\n"),pos1=0,pos2=0;
            if (pos == std::string::npos)
            {
                throw "syntax error:unexpected string end";
            }

            Value::Type root,tmp;

            std::vector<std::shared_ptr<Value>> parents;


            while (pos < source.size())
            {
                if (parents.size())
                {
                    if (parents.back() -> type() == typeid(Object))
                    {
                        //get property name and skip that part of string
                        if (static_cast<Object*>(parents.back().get())->value.size() && source[pos]!=',' && source[pos]!='}')
                        {
                            //error
                        }
                        pos = source.find_first_not_of(" \t\r\n",pos+(source[pos]==','));
                        if (pos >= source.size() || pos == std::string::npos)
                        {
                            //error
                        }

                        char c = 0;
                        if (source[pos] == '"' || source[pos]=='\'')
                        {
                            c = source[pos];
                            ++pos;
                        }
                        if (pos == source.size())
                        {
                            //error
                        }
                        if (checkFPC(source[pos]))
                        {
                            pos1 = pos;
                            while (pos < source.size() && checkFC(source[pos]))
                                ++pos;
                            pos2 = pos;
                            if (pos < source.size() && source[pos] == c)
                            {
                                ++pos;
                            }
                            pos = source.find_first_not_of(" \t\r\n",pos);
                            if (pos == std::string::npos || pos >= source.size() || source[pos]!=':')
                            {
                                throw "syntax error: expected ':'";
                            }
                            tmps = source.substr(pos1,pos2-pos1);
                            pos = source.find_first_not_of(" \t\r\n",pos+1);
                            if (pos == std::string::npos || pos >= source.size())
                            {
                                //error
                            }
                        }
                        else
                        {
                            //error
                        }
                    }
                    else if (parents.back() -> type() == typeid(Array))
                    {
                        //get
                        if (source[pos] != ',' && source[pos] != ']' && static_cast<Array*>(parents.back().get())->value.size())
                        {
                            throw ParseException("syntax error: undexpected ','",pos);
                        }
                        pos1 = pos+1;
                        while (pos < source.size() && source[pos] == ',')
                        {
                            pos = source.find_first_not_of(" \t\r\n",pos+1);
                            if (pos == std::string::npos || pos >= source.size())
                            {
                                throw ParseException("syntax error: closing ] parenthesis not found",source.size());
                            }
                            if (source[pos]==']')
                                static_cast<Array*>(parents.back().get())->value.emplace_back(std::make_shared<Undefined>());
                        }
                    }
                }
                switch (source[pos])
                {
                case '"':
                    {
                        pos1 = pos+1;
                        while (pos1 < source.size() && source[pos1] != '"')
                        {
                            if (source[pos1] == '\\')
                                ++pos1;
                            ++pos1;
                        }
                        if (pos1 >= source.size())
                        {
                            throw ParseException("syntax error: unclosed string",pos);
                        }

                        tmp = String::from(source.substr(pos+1,pos1-pos-1));

                        pos = pos1;
                        break;
                    }
                case '\'':
                    {
                        pos1 = pos+1;
                        while (pos1 < source.size() && source[pos1] != '\'')
                        {
                            if (source[pos1] == '\\')
                                ++pos1;
                            ++pos1;
                        }
                        if (pos1 >= source.size())
                        {
                            throw ParseException("syntax error: unclosed string",pos);
                        }

                        tmp = String::from(source.substr(pos+1,pos1-pos-1));

                        pos = pos1;
                        break;
                    }
                case '[':
                    {
                        tmp = std::static_pointer_cast<Value>(std::make_shared<Array>());
                        break;
                    }
                case ']':
                    {
                        if (parents.back().get()->type() != typeid(Array))
                        {
                            throw ParseException("syntax error: unexpected ]",pos);
                        }
                        parents.pop_back();
                        break;
                    }
                case '{':
                    {
                        tmp = std::static_pointer_cast<Value>(std::make_shared<Object>());
                        break;
                    }
                case '}':
                    {
                        if (parents.back().get()->type() != typeid(Object))
                        {
                            throw ParseException("syntax error: unexpected '}'",pos);
                        }
                        parents.pop_back();
                        break;
                    }
                default:
                    {
                        if (pos < source.size())
                        {
                            if (source[pos] >= '0' && source[pos] <='9')
                            {
                                pos1 = pos;
                                size_t pf = 0;
                                while (pos < source.size() && ((source[pos]>='0' && source[pos]<='9') || source[pos]=='.'))
                                {
                                    if (source[pos]=='.' && pf==0)
                                    {
                                        pf = pos;
                                    }
                                    else if (source[pos]=='.')
                                    {
                                        //error
                                    }
                                    ++pos;
                                }
                                tmp = Number::from(stod(source.substr(pos1,pos-pos1)));
                                if (pos!=pos1)
                                    --pos;
                            }
                            else
                                throw ParseException("syntax error: unexpected character",pos);
                        }
                    }
                }
                if (tmp && parents.size())
                {
                    if (parents.back() -> type() == typeid(Array))
                    {
                        static_cast<Array*>(parents.back().get())->value.push_back(tmp);
                    }
                    else
                    {
                        static_cast<Object*>(parents.back().get())->value[tmps]=tmp;
                    }
                }
                else if (tmp) root = tmp;
                if (tmp && (tmp->type() == typeid(Array) || tmp->type() == typeid(Object)))
                {
                    parents.push_back(tmp);
                }

                tmp.reset();

                pos = source.find_first_not_of(" \t\r\n",pos+1);
                if (pos == std::string::npos)
                    pos=source.size();
            }
            if (!root)
                return std::make_shared<Undefined>();
            return root;
        }
    }
}
