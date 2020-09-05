#ifndef XML_HPP_INCLUDED
#define XML_HPP_INCLUDED

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <sstream>
#include <type_traits>

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

			Value() : present(false) {}
			Value(const std::string& v) : val(v), present(true) {}
			Value(const Value&) = default;
			Value(Value&&) noexcept = default;
			~Value() = default;
		};
		class Node
		{
		public:
			virtual bool isTextNode() const noexcept = 0;

			virtual std::string toString(bool minified = false) const;
			virtual std::string format(bool minified, unsigned pos) const = 0;

			virtual ~Node() = default;
		};

		struct TextNode : public Node
		{
			std::string text;

			virtual bool isTextNode() const noexcept;

			virtual std::string format(bool minified, unsigned pos) const;

			TextNode(const std::string&);
		};

		struct Tag : public Node
		{
			std::string name;
			bool collapsable;
			std::unordered_map<std::string, Value> attributes;
			std::vector<std::unique_ptr<Node> > children;

			Value getAttribute(const std::string attribute) const noexcept;
			void setAttribute(const std::string attribute, const std::string value = "");

			template<typename T> typename std::enable_if<std::is_base_of<Node, T>::value>::type addChild(std::unique_ptr<T>&& c)
			{
				children.emplace_back(std::move(c));
			}

			virtual std::string format(bool minified, unsigned pos = 0) const;
			std::string formatContents(bool minified, unsigned pos = 0) const;

			virtual bool isTextNode() const noexcept;

			Tag() : Node(), collapsable(false) {}
			Tag(const std::string& n) : name(n), collapsable(false) {}
		};

		std::unique_ptr<Tag> parse(const std::string& source, bool inl = false);
	}
}
#endif // XML_HPP_INCLUDED
