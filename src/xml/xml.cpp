#include <xml.hpp>
#include <core.hpp>
namespace ParseLib
{
	namespace XML
	{
		bool checkNameC(char a)
		{
			return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || a == '-' || a == '_' || a == '.' || a == ':';
		}
		bool checkAttrC(char a)
		{
			return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || a == '.' || a == ':';
		}
		bool checkWhite(char a)
		{
			return a == ' ' || a == '\n' || a == '\t' || a == '\r';
		}

		std::string Node::toString(bool minified)const
		{
			return format(minified, 0);
		}

		TextNode::TextNode(const std::string& t)
		{
			text = t;
		}
		bool TextNode::isTextNode() const noexcept
		{
			return true;
		}
		std::string TextNode::format(bool minified, unsigned pos)const
		{
			return (minified ? "" : std::string(pos, '\t')) + text;
		}

		bool Tag::isTextNode() const noexcept
		{
			return false;
		}

		Value Tag::getAttribute(const std::string attribute) const noexcept
		{
			auto it = attributes.find(attribute);
			if (it != attributes.end())
			{
				return it->second;
			}
			return Value();
		}

		void Tag::setAttribute(const std::string attribute, const std::string value)
		{
			attributes[attribute] = value;
		}

		std::string Tag::format(bool minified, unsigned pos)const
		{
			std::string ret;
			if (minified)
			{
				ret = "<" + name;
				for (const auto& i : attributes)
				{
					ret += " " + i.first;
					if (ret.size())
						ret += "=\"" + i.second.val + "\"";
				}
				if (collapsable && children.empty())
					ret += "/";
				ret += ">";
				for (const auto& i : children)
				{
					ret += i->format(true, 0);
				}
				if (!collapsable || !children.empty())
					ret += "<" + name + "/>";
			}
			else
			{
				ret = std::string(pos, '\t') + "<" + name;
				for (const auto& i : attributes)
				{
					ret += " " + i.first;
					if (ret.size())
						ret += " = \"" + i.second.val + "\"";
				}
				if (collapsable && children.empty())
					ret += "/";
				ret += ">\n";
				for (const auto& i : children)
				{
					ret += i->format(false, pos + 1) + "\n";
				}
				if (!collapsable || !children.empty())
					ret += std::string(pos, '\t') + "<" + name + "/>";
			}
			return ret;
		}

		std::string Tag::formatContents(bool minified, unsigned pos) const
		{
			std::string ret;
			if (minified)
			{
				for (const auto& i : children)
				{
					ret += i->format(true, 0);
				}
			}
			else
			{
				for (const auto& i : children)
				{
					ret += i->format(false, pos + 1) + "\n";
				}
			}
			return ret;
		}

		bool Value::exists() const noexcept
		{
			return present;
		}

		std::unique_ptr<Tag> parse(const std::string& source,bool inl)
		{
			std::unique_ptr<Tag> root(new Tag());

			if (source.size() == 0)
				return root;

			bool tags = inl, strange = false;
			size_t pos = 0, pos1 = 0, pos2 = 0, pos3 = 0;
			Tag* current = root.get(), * tmp;
			std::vector<Tag*>parents;
			while (pos < source.size() && (pos = source.find('<', pos)) != std::string::npos)
			{
				strange = false;
				++pos;
				// "<" character found!
				// first let's check if its is doctype  or ?xml tag
				if (!tags)
				{
					//check if there is enough space for !DOCTYPE substring
					if (pos + 7 < source.size())
					{
						bool s = true;
						const char* str = "!DOCTYPE";
						for (unsigned i = 0; i < 8; ++i, ++pos)
							if (str[i] != source[pos])
							{
								s = false;
								break;
							}
						if (s)
						{
							strange = true;
							//yup, there definitely is !DOCTYPE substring
							if (pos == std::string::npos)
								throw ParseException("syntax error: ending of tag not found", source.size());
							if (!checkWhite(source[pos]))
								throw ParseException("syntax error: expected whitespace character", pos);
							pos = source.find_first_not_of(" \t\r\n", pos);

							if (pos == std::string::npos)
								throw ParseException("syntax error: ending of doctype not found", source.size());
							//find name specified by doctype
							pos1 = pos;
							while (pos < source.size() && checkNameC(source[pos]))
								++pos;
							//there it is
							root->name = source.substr(pos1, pos - pos1);
							if (pos >= source.size())
								throw ParseException("syntax error: ending of doctype not found", source.size());
							if (!checkWhite(source[pos]) && source[pos] != '>' && source[pos] != '/')
								throw ParseException("syntax error: expected whitespace character", pos);
							//skip to possible format specification
							pos = source.find_first_not_of(" \t\r\n", pos);
							if (pos == std::string::npos)
								throw ParseException("syntax error: ending of doctype not found", source.size());
							if (source[pos] == '"')
							{
								pos1 = pos;
								pos = source.find('"', pos + 1);
								if (pos == std::string::npos)
									throw ParseException("syntax error: ending of doctype not found", source.size());
								root->attributes["format"] = source.substr(pos1 + 1, pos - pos1 - 2);
								pos = source.find_first_not_of(" \t\r\n", pos + 1);
							}
							if (pos == std::string::npos || pos == source.size())
								throw ParseException("syntax error: ending of doctype not found", source.size());
							if (source[pos] == '/')
								++pos;
							if (pos == source.size())
								throw ParseException("syntax error: ending of doctype not found", source.size());
							if (source[pos] != '>')
								throw ParseException("syntax error: unexpected character", pos);
							continue;
						}
					}
					//otherwise check if there is enough space for ?xml
					if (pos + 3 < source.size())
					{
						bool s = true;
						const char* str = "?xml";
						for (unsigned i = 0; i < 4; ++i, ++pos)
							if (str[i] != source[pos])
							{
								s = false;
								break;
							}
						if (s)
						{
							strange = true;
							//there you are
							if (pos == source.size())
								throw ParseException("syntax error: ending of tag not found", source.size());
							std::string prefix;
							if (source[pos] == '-')
							{
								//we have xlm-smth syntax!
								pos1 = pos;
								while (pos < source.size() && checkNameC(source[pos]))
									++pos;
								if (pos == source.size())
									throw ParseException("syntax error: ending of tag not found", source.size());
								prefix = source.substr(pos1, pos - pos1);
							}
							if (!checkWhite(source[pos]) && source[pos] != '?')
								throw ParseException("syntax error: expected whitespace character", pos);
							///parse attributes
							if (source[pos] == '?')
							{
								//todo better error handling
								++pos;
								if (source[pos] != '>')
									throw ParseException("syntax error: expected '>' character", pos);
								//
								continue;
							}
							pos = source.find_first_not_of(" \t\r\n", pos);
							if (pos == std::string::npos)
							{
								throw ParseException("syntax error: ending of tag not found", source.size());
							}
							while (pos < source.size() && source[pos] != '?')
							{
								pos2 = pos;
								while (pos < source.size() && checkNameC(source[pos]))
									++pos;
								if (pos2 == pos)
								{
									throw ParseException("syntax error: unexpected character", source.size());
								}
								pos1 = source.find_first_not_of(" \t\r\n", pos);
								if (pos1 == std::string::npos)
								{
									throw ParseException("syntax error: ending of tag not found", source.size());
								}
								if (source[pos1] == '=')
								{
									// attr = val clouse
									pos1 = source.find_first_not_of(" \t\r\n", pos1 + 1);
									if (source[pos1] == '"')
									{
										//string
										pos3 = source.find('"', pos1 + 1);
										root->attributes[prefix + source.substr(pos2, pos - pos2)] = source.substr(pos1 + 1, pos3 - pos1 - 1);
										pos1 = pos3;
									}
									else
									{
										//value

										pos3 = pos1;
										while (pos1 < source.size() && checkNameC(source[pos1]))
											++pos1;
										if (pos1 == source.size())
										{
											throw ParseException("syntax error: ending of tag not found", source.size());
										}
										root->attributes[prefix + source.substr(pos2, pos - pos2)] = source.substr(pos3, pos1 - pos3);
										--pos1;
									}
									pos = pos1 + 1;
								}
								else if (pos != source.size() && (checkWhite(source[pos]) || source[pos] == '/' || source[pos] == '>'))
								{
									root->attributes[prefix + source.substr(pos2, pos - pos2)] = std::string();
								}
								else
								{
									if (pos == source.size())
										throw ParseException("syntax error: ending of tag not found", source.size());
									throw ParseException("syntax error: unexpected character", pos);
								}
								pos = source.find_first_not_of(" \t\r\n", pos);
								if (pos == std::string::npos)
								{
									throw ParseException("syntax error: ending of tag not found", source.size());
								}
							}
							///end of attributes
							if (source[pos] != '?')
							{
								throw ParseException("syntax error: unexpected character", pos);
							}
							++pos;
							if (pos >= source.size())
							{
								throw ParseException("syntax error: ending of tag not found", source.size());
							}
							if (source[pos] != '>')
							{
								throw ParseException("syntax error: unexpected character", pos);
							}
							++pos;
						}
					}
					else
					{
						//err
					}
				}

				//check if it is closing tag
				if (!strange)
				{
					if (pos < source.size() && source[pos] == '/')
					{
						pos += 1;
						pos = source.find_first_not_of(" \t\r\n", pos);
						pos1 = pos;
						if (pos == std::string::npos || pos + current->name.size() >= source.size())
						{
							throw ParseException("syntax error: unfinished closing tag", pos1);
						}
						bool s = true;
						if (pos + current->name.size() > source.size())
						{
							s = false;
						}
						else
							for (unsigned int i = 0; i < current->name.size(); ++i, ++pos)
								if (current->name[i] != source[pos])
								{
									s = false;
									break;
								}
						if (!s)
						{
							throw ParseException("syntax error: closing tag does not match", pos1);
						}
						if (pos >= source.size())
						{
							throw ParseException("syntax error: unfinished closing tag", pos1);
						}
						pos = source.find_first_not_of(" \t\r\n", pos);
						if (pos == std::string::npos || source[pos] != '>')
						{
							throw ParseException("syntax error: unfinished closing tag", pos1);
						}
						if (parents.size() == 0)
						{
							throw ParseException("syntax error: closing tag without beginning", pos1);
						}
						current = parents.back();
						parents.pop_back();
						++pos;
					}
					else
					{
						pos = source.find_first_not_of(" \t\r\n", pos);//start of name
						if (pos == std::string::npos)
						{
							throw ParseException("syntax error: ending of tag not found", source.size());
						}
						//new tag
						tmp = new Tag();

						pos1 = pos;

						while (pos < source.size() && checkNameC(source[pos]))
							++pos;
						if ((pos == source.size() || (!checkWhite(source[pos]) && (source[pos] != '>' && source[pos] != '/'))))
						{
							delete tmp;
							if (pos == source.size())
								throw ParseException("syntax error: ending of tag not found", source.size());
							throw ParseException("syntax error: invalid tag name \"" + source.substr(pos1, pos - pos1) + "\"", pos1);
						}
						tmp->name = source.substr(pos1, pos - pos1);
						pos = source.find_first_not_of(" \t\r\n", pos);
						if (pos == std::string::npos)
						{
							delete tmp;
							throw ParseException("syntax error: ending of tag not found", source.size());
						}
						//now attribs

						while (pos < source.size() && source[pos] != '>' && source[pos] != '/')
						{
							pos2 = pos;
							while (pos < source.size() && checkNameC(source[pos]))
								++pos;
							pos1 = source.find_first_not_of(" \t\r\n", (pos2 == pos) ? pos + 1 : pos);
							if (pos1 == std::string::npos)
							{
								delete tmp;
								throw ParseException("syntax error: ending of tag not found", source.size());
							}
							if (source[pos1] == '=')
							{
								// attr = val clouse
								pos1 = source.find_first_not_of(" \t\r\n", pos1 + 1);
								if (source[pos1] == '"')
								{
									//string
									pos3 = source.find('"', pos1 + 1);
									tmp->attributes[source.substr(pos2, pos - pos2)] = source.substr(pos1 + 1, pos3 - pos1 - 1);
									pos1 = pos3;
								}
								else
								{
									//value

									pos3 = pos1;
									while (pos1 < source.size() && checkNameC(source[pos1]))
										++pos1;
									if (pos1 == source.size())
									{
										delete tmp;
										throw ParseException("syntax error: ending of tag not found", source.size());
									}
									tmp->attributes[source.substr(pos2, pos - pos2)] = source.substr(pos3, pos1 - pos3);
									--pos1;
								}
								pos = pos1 + 1;
							}
							else if (pos != source.size() && (checkWhite(source[pos]) || source[pos] == '/' || source[pos] == '>'))
							{
								tmp->attributes[source.substr(pos2, pos - pos2)] = std::string();
							}
							else
							{
								delete tmp;
								if (pos == source.size())
									throw ParseException("syntax error: ending of tag not found", source.size());
								throw ParseException("syntax error: unexpected character: " + source[pos], pos);
							}
							pos1 = pos;
							pos = source.find_first_not_of(" \t\r\n", pos1);
							if (pos == std::string::npos)
							{
								delete tmp;
								throw ParseException("syntax error: ending of tag not found", pos1);
							}
						}
						tags = true;
						current->children.emplace_back(tmp);
						if (source[pos] == '/')
						{
							pos += 1;
						}
						else
						{
							parents.push_back(current);
							current = tmp;
						}
						if (source[pos] != '>')
						{
							throw ParseException("syntax error: ending of tag not found", pos);
						}
						++pos;
					}
					if (parents.size())
					{
						pos1 = source.find('<', pos);
						if (pos1 != std::string::npos && pos != pos1)
						{
							pos2 = source.find_first_not_of(" \t\r\n", pos);
							if (pos2 != pos1)
								current->children.emplace_back(new TextNode(source.substr(pos, pos1 - pos)));
						}
					}
				}
			}
			if (parents.size())
				throw ParseException("syntax error: closing tag not found", source.size());
			if (inl)
			{
				for (auto& i : root->children)
				{
					if (!i->isTextNode())
					{
						return std::unique_ptr<XML::Tag>(static_cast<XML::Tag*>(i.release()));
					}
				}
				return std::unique_ptr<XML::Tag>();
			}
			return std::move(root);
		}
	}
}
