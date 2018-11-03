#include <parser.hpp>
#include <tree.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>
#include <fstream>

namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
namespace ascii = boost::spirit::ascii;

namespace data {
	struct import
	{
		std::vector<std::string> names;
		std::string from;
	};

	struct oid
	{
		std::string name;
		std::vector<std::string> path;
	};

	struct object
	{
		std::string name;
		std::string syntax;
		std::string access;
		std::string status;
		std::string desc;
		std::vector<std::string> path;
	};
}

BOOST_FUSION_ADAPT_STRUCT(
	data::import,
	(std::vector<std::string>, names),
	(std::string, from)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::oid,
	(std::string, name),
	(std::vector<std::string>, path)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::object,
	(std::string, name),
	(std::string, syntax),
	(std::string, access),
	(std::string, status),
	(std::string, desc),
	(std::vector<std::string>, path)
)

template <typename Iterator>
struct mib_parser : qi::grammar<Iterator, fusion::vector< std::vector<data::import>, std::vector<data::oid>, std::vector<data::object> >(), ascii::space_type>
{
	mib_parser() : mib_parser::base_type(start)
	{
		name %= qi::lexeme[+(ascii::alnum | qi::char_("-"))];
		names %= name % ',';

		import %= names >> qi::lit("FROM") >> name;
		imports %= qi::lit("IMPORTS") >> +import >> ';';

		path %= qi::lit("::=") >> '{' >> +qi::lexeme[+(ascii::alnum | qi::char_("()-"))] >> '}';

		desc %= '"' >> qi::lexeme[+(qi::char_ - '"')] >> '"';

		oid %= name >> qi::lit("OBJECT IDENTIFIER") >> path;
		oids %= +oid;

		obj %= name >> qi::lit("OBJECT-TYPE") >> qi::lit("SYNTAX") >> name >> qi::lit("ACCESS") >> name >> qi::lit("STATUS") >> name >> qi::lit("DESCRIPTION") >> desc >> path;
		objs %= +obj;


		start %= imports >> oids >> objs;
	}

	qi::rule<Iterator, std::string(), ascii::space_type> name;
	qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> names;
	qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> path;
	qi::rule<Iterator, std::string(), ascii::space_type> desc;

	qi::rule<Iterator, data::import(), ascii::space_type> import;
	qi::rule<Iterator, std::vector<data::import>(), ascii::space_type> imports;

	qi::rule<Iterator, data::oid(), ascii::space_type> oid;
	qi::rule<Iterator, std::vector<data::oid>(), ascii::space_type> oids;

	qi::rule<Iterator, data::object(), ascii::space_type> obj;
	qi::rule<Iterator, std::vector<data::object>(), ascii::space_type> objs;

	qi::rule<Iterator, fusion::vector< std::vector<data::import>, std::vector<data::oid>, std::vector<data::object> >(), ascii::space_type> start;
};

parser::parser() {

}

bool parser::load(std::string filename, std::vector<std::string> imp) {
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

	fusion::vector<std::vector<data::import>, std::vector<data::oid>, std::vector<data::object> > res;

	mib_parser<std::string::const_iterator> g;
	bool r = qi::phrase_parse(iter, end, g, ascii::space, res);

	auto imports = fusion::at_c<0>(res);
	auto oids = fusion::at_c<1>(res);
	auto objs = fusion::at_c<2>(res);

	for(auto const& value : imports){
		std::cout<<"Names: ";
		for(auto const& val : value.names)
			std::cout<<val<<" ";
		std::cout<<"From: "<<value.from<<std::endl;
	}

	for(auto const& value : oids){
		std::cout<<"Name: "<<value.name<<std::endl<<"Path: ";
		for(auto const& val : value.path)
			std::cout<<val<<", ";
		std::cout<<std::endl;
	}

	for(auto const& value : objs){
		std::cout<<value.name<<std::endl;
		std::cout<<value.syntax<<std::endl;
		std::cout<<value.access<<std::endl;
		std::cout<<value.status<<std::endl;
		std::cout<<value.desc<<std::endl;
		for(auto const& val : value.path)
			std::cout<<val<<", ";
		std::cout<<std::endl;
	}



	return r;
}
