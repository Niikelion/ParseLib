#ifndef XML_HPP_INCLUDED
#define XML_HPP_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <sstream>

namespace ParseLib
{
    namespace XML
    {
        class Value
        {
        private:
            bool present;
        public:
            std::string val;

            bool exists() const noexcept;

            Value& operator = (const Value&) = default;

            Value(): present(false) {}
            Value(const std::string&);
            Value(const Value&) = default;
            Value(Value&&) noexcept = default;
            ~Value() = default;
        };
        class Node
        {
        public:
            virtual bool isTextNode() const noexcept = 0;

            virtual std::string toString(bool minified = false) const;
            virtual std::string format(bool minified,unsigned pos) const = 0;

            virtual ~Node() = default;
        };

        struct TextNode: public Node
        {
            std::string text;

            virtual bool isTextNode() const noexcept;

            virtual std::string format(bool minified,unsigned pos)const;

            TextNode(const std::string&);
        };

        struct Tag: public Node
        {
            std::string name;
            std::unordered_map<std::string,Value> attributes;
            std::vector<std::unique_ptr<Node> > children;

            Value getAttribute(const std::string attribute) const noexcept;

            virtual std::string format(bool minified,unsigned pos) const;

            virtual bool isTextNode() const noexcept;
        };

        std::unique_ptr<Tag> parse(const std::string&);
    }
}
#endif // XML_HPP_INCLUDED
