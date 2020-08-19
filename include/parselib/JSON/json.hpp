#ifndef JSON_H
#define JSON_H

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <typeindex>

namespace ParseLib
{
    namespace JSON
    {
        bool checkNameC(char a);
        bool checkFC(char a);
        bool checkFPC(char a);
        class Value
        {
        public:
            typedef std::shared_ptr<Value> Type;

            virtual std::type_index type() const = 0;

            virtual std::string toString(bool json = false,unsigned offset = 0,bool compacted = false) const = 0;

            virtual bool toBoolean() const;
            virtual double toNumber() const;
            virtual std::vector<Type> toArray() const;
            virtual std::map<std::string,Type> toProperties() const;

            virtual ~Value() = default;
        };

        class Undefined: public Value
        {
        public:
            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual bool toBoolean() const;

            Undefined() = default;
            Undefined(const Undefined&) = default;
            Undefined(Undefined&&) noexcept = default;
        };

        class Number: public Value
        {
        public:
            static Value::Type from(double a);

            double value;

            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual double toNumber() const;

            Number& operator = (const Number&) = default;
            Number& operator = (Number&&) = default;

            Number& operator = (double a);

            Number();

            Number(double a);

            Number(const Number&) = default;
            Number(Number&&) noexcept = default;

        };

        class Boolean: public Value
        {
        public:
            bool value;

            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual bool toBoolean() const;

            Boolean();

            Boolean(bool a);

            Boolean(const Boolean&) = default;
            Boolean(Boolean&&) noexcept = default;
        };

        class String: public Value
        {
        public:
            static Value::Type from(const std::string&);

            std::string value;

            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual bool toBoolean() const;
            virtual double toNumber() const;

            String() = default;
            String(const std::string& a);

            String(const String&) = default;
            String(String&&) noexcept = default;
        };

        class Array: public Value
        {
        public:
            std::vector<std::shared_ptr<Value> > value;

            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual std::vector<Type> toArray() const;

            Array() = default;
            Array(const std::vector<Type>& a);

            Array(const Array&) = default;
            Array(Array&&) noexcept = default;
        };

        class Object: public Value
        {
        public:
            std::map<std::string,std::shared_ptr<Value> > value;

            virtual std::type_index type() const;

            virtual std::string toString(bool json = false,unsigned offset=0,bool compacted=false) const;

            virtual std::map<std::string,Type> toProperties() const;

            Object() = default;
            Object(const std::map<std::string,Type>& a);

            Object(const Object&) = default;
            Object(Object&&) noexcept = default;
        };

        std::string stringify(const Value::Type& source);
        Value::Type parse(const std::string& source);
    }
}

#endif // JSON_H
