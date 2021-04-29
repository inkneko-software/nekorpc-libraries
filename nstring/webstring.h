#include <string>
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>

namespace webstring
{
	std::string strip(const std::string & str, const std::string chr = " ");
	std::string lstrip(const std::string& str, const std::string chr = " ");
	std::string rstrip(const std::string& str, const std::string chr = " ");
	std::string tolower(const std::string& str);
	std::string toupper(const std::string& str);

	std::string urldecode(std::string text);
	std::string urlencode(std::string text);

	/*!
	统计指定的UTF8编码的字符串中有多少个字符
	参数：utf8String | UTF8编码的字符串
	返回：字符个数
	*/
	std::size_t UTF8Strlen(const std::string& utf8String);

	/*!
	substr函数，utf8编码*/
	std::string UTF8Substr(const std::string& str, size_t start, size_t leng);

	/*!
	解析HTTP请求参数(&key=value&key=value的参数格式)
	参数：param | 要解析的字符串
	参数：assginChar | 键与值之间的关系符号，默认为等号
	参数：splitChar  | 键值对之间分割的符号
	返回：一个关联容器，包含着键值对的参数
	*/
	std::multimap<std::string, std::string> ParseKeyValue(std::string param, char assginChar = '=', char splitChar = '&');

	/*!
	使用MD5算法计算指定字符串的数据摘要
	参数：text | 要计算的字符串
	返回：长度为16字节(128位)的数据摘要的十六进制表示，每个字节长度为两位。输入"helloworld"输出"fc5e038d38a57032085441e7fe7010b0"
	*/
	std::string md5(std::vector<uint8_t> content);

	/*!
	使用SHA1算法计算指定字符串的数据摘要
	参数：text | 要计算的字符串
	返回：长度为20字节(160位)的数据摘要的十六进制表示，每个字节长度为两位。输入"helloworld"输出"6adfb183a4a2c94a2f92dab5ade762a47889a5a1"
	*/
	std::string sha1(std::vector<uint8_t> content);

	/*!
	获取一个UUID
	返回：一个字符串类型的UUID，例如f05c8857-d456-494d-a988-3904899a7aea
	*/
	std::string genUUID();

	/*!
	生成当前的Unix时间戳。如果time(nullptr)返回-1则本函数返回"0"
	*/
	std::string genTimestamp();

	/*!
	将string类型的map转化为JSON字符串
	*/
	std::string JsonStringify(std::map<std::string, std::string> object);

	/*!
	将ptree对象转换为字符串
	*/
	std::string JsonStringify(boost::property_tree::ptree propertyTree);

	/*!
	base64编解码, 自动进行补全操作
	*/
	std::string b64encode(std::string input);

	std::string b64decode(std::string input);
}