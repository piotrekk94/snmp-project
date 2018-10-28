#include <parser.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <fstream>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct import
{
	std::vector<std::string> names;
	std::string from;
};

BOOST_FUSION_ADAPT_STRUCT(
	import,
	(std::vector<std::string>, names),
	(std::string, from)
)

template <typename Iterator>
struct mib_parser : qi::grammar<Iterator, import(), ascii::space_type>
{
	mib_parser() : mib_parser::base_type(start)
	{

		name %= qi::lexeme[+(ascii::alnum)];
		names %= name >> *(',' >> name);
		start %=
			qi::lit("IMPORTS")
			>> names
			>> qi::lit("FROM")
			>> name
			>> ';'
			;
	}

	qi::rule<Iterator, std::string(), ascii::space_type> name;
	qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> names;
	qi::rule<Iterator, import(), ascii::space_type> start;
};

parser::parser() {
	std::cout<<"Parser init"<<std::endl;
}

bool parser::load(std::string filename, std::vector<std::string> imports) {
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
	import imp;
	mib_parser<std::string::const_iterator> g;
	bool r = qi::phrase_parse(iter, end, g, ascii::space, imp);
	std::cout<<"Names: ";
	for(auto const& value : imp.names){
		std::cout<<value<<", ";
	}
	std::cout<<std::endl<<"From: "<<imp.from<<std::endl;
	return r;
}
