#include "webstring.h"
#include <regex>
#include <sstream>
#include <iomanip>
#include <ctime>

//compile with option -lcrypto
#include <openssl/md5.h>
#include <openssl/sha.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <algorithm>

namespace webstring
{
	std::string strip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto start = result.find_first_not_of(*c);

				if ((start != result.npos) && (start != 0))
				{
					/*如果从左开始能找到非目标字符，且位置不为0*/
					result = result.substr(start);
					found = true;
				}


				auto stop = result.find_last_not_of(*c);
				/*如果从右边开始能找到非目标字符，且位置不为最后*/
				if ((stop != result.npos) && (stop != result.length() - 1))
				{
					result = result.substr(0, stop + 1);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string lstrip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto start = result.find_first_not_of(*c);

				if ((start != result.npos) && (start != 0))
				{
					/*如果从左开始能找到非目标字符，且位置不为0*/
					result = result.substr(start);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string rstrip(const std::string& str, const std::string chr)
	{
		std::string result(str);
		bool found = false;
		while (true)
		{
			found = false;
			for (auto c = chr.cbegin(); (c != chr.cend()) && (found == false); ++c)
			{
				auto stop = result.find_last_not_of(*c);
				/*如果从右边开始能找到非目标字符，且位置不为最后*/
				if ((stop != result.npos) && (stop != result.length() - 1))
				{
					result = result.substr(0, stop + 1);
					found = true;
				}
			}

			if (found == false)
			{
				break;
			}
		}

		return result;
	}

	std::string tolower(const std::string& str)
	{
		std::string ret;
		std::transform(str.cbegin(), str.cend(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
		return ret;
	}

	std::string toupper(const std::string& str)
	{
		std::string ret;
		std::transform(str.cbegin(), str.cend(), ret.begin(), [](unsigned char c) { return std::toupper(c); });
		return ret;
	}


	std::string urldecode(std::string text)
	{
		//这vector用的..凑合着用吧，还能离不成
		std::vector<char> convertedBytes;
		std::stringstream stream;

		std::regex format("%([0-9a-fA-F]{2})");

		for (auto index = text.cbegin(); index != text.cend(); )
		{
			if (*index == '%')
			{
				std::string assumedEncodedString(index, index + 3);
				std::smatch result;
				if (std::regex_match(assumedEncodedString, result, format))
				{
					unsigned int byteValue;
					stream << std::hex << result[1];
					stream >> byteValue;
					stream.clear();

					convertedBytes.push_back(static_cast<char>(byteValue));
					index += 3;
				}
				else
				{
					convertedBytes.push_back(*index);
					index++;
				}
			}
			else
			{
				convertedBytes.push_back(*index);
				index++;
			}
		}

		return std::string(convertedBytes.data(), convertedBytes.size());
	}

	std::string urlencode(std::string text)
	{
		std::string convertedBytes;
		std::string temp;
		std::stringstream stream;
		for (auto chr : text)
		{

			if (!((chr >= 0x41 && chr <= 0x5A) || (chr >= 0x61 && chr <= 0x7A) || (chr >= 0x30 && chr <= 0x39)) && (chr != '-' && chr != '_' && chr != '.' && chr != '`'))
			{
				stream << std::setfill('0') << std::setw(2) << std::hex << (static_cast<unsigned int>(chr) & 0xFF);
				stream >> temp;
				convertedBytes += '%' + temp;
				stream.clear();
			}
			else
			{
				convertedBytes += chr;
			}
		}

		return convertedBytes;
	}

	std::size_t UTF8Strlen(const std::string &utf8String)
	{
		//来源：https://blog.csdn.net/yangxudong/article/details/24267155
		//修正了潜在的越位读取造成的死循环
		size_t length = 0;
		for (size_t i = 0, len = 0; i < utf8String.length(); i += len) {
			unsigned char byte = utf8String[i];
			if (byte >= 0xFC)
				len = 6;
			else if (byte >= 0xF8)
				len = 5;
			else if (byte >= 0xF0)
				len = 4;
			else if (byte >= 0xE0)
				len = 3;
			else if (byte >= 0xC0)
				len = 2;
			else
				len = 1;
			length++;
		}
		return length;
	}

	//http://www.zedwood.com/article/cpp-utf-8-mb_substr-function
	//没有进行代码审计
	std::string UTF8Substr(const std::string& str, size_t start, size_t leng)
	{
		using std::string;
		if (leng == 0) { return ""; }
		std::size_t c, i, ix, q, min = string::npos, max = string::npos;
		for (q = 0, i = 0, ix = str.length(); i < ix; i++, q++)
		{
			if (q == start) { min = i; }
			if (q <= start + leng || leng == string::npos) { max = i; }

			c = static_cast<unsigned char>(str[i]);
			if (c <= 127) i += 0;
			else if ((c & 0xE0) == 0xC0) i += 1;
			else if ((c & 0xF0) == 0xE0) i += 2;
			else if ((c & 0xF8) == 0xF0) i += 3;
			//else if (($c & 0xFC) == 0xF8) i+=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
			//else if (($c & 0xFE) == 0xFC) i+=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
			else return "";//invalid utf8
		}
		if (q <= start + leng || leng == string::npos) { max = i; }
		if (min == string::npos || max == string::npos) { return ""; }
		return str.substr(min, max);
	}

	std::multimap<std::string, std::string> ParseKeyValue(std::string param, char assginChar, char splitChar)
	{
		using namespace std;

		std::multimap<std::string, std::string> result;
		if (assginChar == splitChar)
		{
			return result;
		}

		std::string key;
		std::string value;
		std::string temp_str;

		size_t begin_pos = 0;
		size_t str_length = param.length();
		size_t split_pos;
		size_t equal_pos;

		while (begin_pos < str_length)
		{
			split_pos = param.find(splitChar, begin_pos);
			if (split_pos == string::npos)
			{
				//匹配到最后一个键值对
				equal_pos = param.find(assginChar, begin_pos);
				if (equal_pos != string::npos)
				{
					key = strip(param.substr(begin_pos, equal_pos - begin_pos));
					value = strip(param.substr(equal_pos + 1));
					result.insert({ key, value });
				}

				break;
			}
			else
			{
				temp_str = param.substr(begin_pos, split_pos - begin_pos);
				equal_pos = temp_str.find(assginChar);
				if (equal_pos != string::npos)
				{
					key = strip(temp_str.substr(0, equal_pos));
					value = strip(temp_str.substr(equal_pos + 1));
					result.insert({ key, value });
				}

				begin_pos = split_pos + 1;
				continue;
			}
		}
		
		return result;
	}

	std::string md5(std::vector<uint8_t> content)
	{
		std::unique_ptr<unsigned char> digest(new unsigned char[MD5_DIGEST_LENGTH]());
		MD5(content.data(), content.size(), digest.get());

		std::stringstream hexdigest;
		hexdigest << std::hex;
		for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
		{
			hexdigest << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest.get()[i]);
		}

		return hexdigest.str();
	}

	std::string sha1(std::vector<uint8_t> content)
	{
		std::unique_ptr<unsigned char> digest(new unsigned char[SHA_DIGEST_LENGTH]());
		SHA1(content.data(), content.size(), digest.get());

		std::stringstream hexdigest;
		hexdigest << std::hex;
		for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
		{
			hexdigest << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(digest.get()[i]);
		}

		return hexdigest.str();
	}

	std::string genUUID()
	{
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		return boost::uuids::to_string(uuid);
	}

	std::string genTimestamp()
	{
		time_t currentTime = time(nullptr);
		return (currentTime == -1) ? "0" : std::to_string(currentTime);
	}

	std::string JsonStringify(std::map<std::string, std::string> object)
	{
		boost::property_tree::ptree jsonTree;
		for (auto pair : object)
		{
			jsonTree.put(pair.first, pair.second);
		}

		std::ostringstream buffer;
		boost::property_tree::write_json(buffer, jsonTree, false);
		return buffer.str();
	}

	std::string JsonStringify(boost::property_tree::ptree propertyTree) noexcept(false)
	{
		std::ostringstream stream;
		boost::property_tree::write_json(stream, propertyTree);

		return stream.str();
	}

	//copy from https://stackoverflow.com/questions/46349697/decode-base64-string-using-boost
	//without code review
	std::string b64decode(std::string input)
	{
		using namespace boost::archive::iterators;
		typedef transform_width<binary_from_base64<remove_whitespace<std::string::const_iterator> >, 8, 6> ItBinaryT;

		try
		{
			// If the input isn't a multiple of 4, pad with =
			size_t num_pad_chars((4 - input.size() % 4) % 4);
			input.append(num_pad_chars, '=');

			size_t pad_chars(std::count(input.begin(), input.end(), '='));
			std::replace(input.begin(), input.end(), '=', 'A');
			std::string output(ItBinaryT(input.begin()), ItBinaryT(input.end()));
			output.erase(output.end() - pad_chars, output.end());
			return output;
		}
		catch (std::exception const&)
		{
			return std::string("");
		}
	}

	//copy from https://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception
	std::string b64encode(std::string input)
	{
		using namespace boost::archive::iterators;
		typedef base64_from_binary<transform_width<std::string::const_iterator
			, 6, 8> > base64_text;

		size_t writePaddChars = (3 - input.length() % 3) % 3;
		std::string base64(base64_text(input.begin()), base64_text(input.end()));

		base64.append(writePaddChars, '=');
		return base64;
	}
}