#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <fstream>
#include <tuple>

namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
namespace ascii = boost::spirit::ascii;

namespace data {
	typedef std::vector<fusion::vector<std::string, int>> path_t;

	struct import
	{
		std::vector<std::string> names;
		std::string from;
	};

	struct path
	{
		std::string startNode;
		path_t path;
		int destId;
	};

	struct oid
	{
		std::string name;
		struct path path;
	};

	struct obj
	{
		std::string name;
		std::string syntax;
		std::string access;
		std::string status;
		std::string desc;
		struct path path;
	};

	struct type
	{
		std::string name;
		std::string visibility;
		int typeId;
		std::string typeName;
		std::string constraints;
	};
}

BOOST_FUSION_ADAPT_STRUCT(
	data::type,
	(std::string, name),
	(std::string, visibility),
	(int, typeId),
	(std::string, typeName),
	(std::string, constraints)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::import,
	(std::vector<std::string>, names),
	(std::string, from)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::path,
	(std::string, startNode),
	(data::path_t, path),
	(int, destId)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::oid,
	(std::string, name),
	(data::path, path)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::obj,
	(std::string, name),
	(std::string, syntax),
	(std::string, access),
	(std::string, status),
	(std::string, desc),
	(data::path, path)
)

void add_oid(data::oid const& oid){
	Object *object = new Object(oid.name);
	ObjectPath path;
	path.path.push_back(std::make_tuple(oid.path.startNode, -1));
	for(auto const& val : oid.path.path){
		path.path.push_back(std::make_tuple(fusion::at_c<0>(val), fusion::at_c<1>(val)));
	}
	path.path.push_back(std::make_tuple("", oid.path.destId));
	tree->addObject(object, path);
}

void add_obj(data::obj const& obj){
	Object *object = new Object(obj.name, obj.syntax, obj.access, obj.status, obj.desc);
	ObjectPath path;
	path.path.push_back(std::make_tuple(obj.path.startNode, -1));
	for(auto const& val : obj.path.path){
		path.path.push_back(std::make_tuple(fusion::at_c<0>(val), fusion::at_c<1>(val)));
	}
	path.path.push_back(std::make_tuple("", obj.path.destId));
	tree->addObject(object, path);
}


template <typename Iterator>
struct mib_parser : qi::grammar<Iterator, void(), ascii::space_type>
{
	mib_parser() : mib_parser::base_type(start)
	{
		name %= qi::lexeme[+(ascii::alnum | qi::char_("-"))];
		names %= name % ',';

		import = names >> qi::lit("FROM") >> name;
		imports = qi::lit("IMPORTS") >> +import >> ';';

		path %= qi::lit("::=") >> '{' >> name >> *(name >> '(' >> qi::int_ >> ')') >> qi::int_ >> '}';

		desc %= '"' >> qi::lexeme[+(qi::char_ - '"')] >> '"';

		oid %= name >> qi::lit("OBJECT IDENTIFIER") >> path;
		oids = +oid[&add_oid];

		obj %= name >> qi::lit("OBJECT-TYPE") >> qi::lit("SYNTAX") >> qi::lexeme[+(qi::char_ - qi::char_("\n"))] >> qi::lit("ACCESS") >> name >> qi::lit("STATUS") >> name >> qi::lit("DESCRIPTION") >> desc >> path;
		objs = +obj[&add_obj];

		//type %= name >> qi::lit("::=") >> -('[' >> name >> qi::int_ >> ']') >> -(qi::lit("EXPLICIT") | qi::lit("IMPLICIT")) >> name;// >> -('(' >> qi::lexeme[+(qi::char_ - ')')] >> ')');

		start = imports >> oids >> objs;
	}

	qi::rule<Iterator, std::string(), ascii::space_type> name;
	qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> names;
	qi::rule<Iterator, data::path(), ascii::space_type> path;
	qi::rule<Iterator, std::string(), ascii::space_type> desc;

	qi::rule<Iterator, void(), ascii::space_type> import;
	qi::rule<Iterator, void(), ascii::space_type> imports;

	qi::rule<Iterator, data::oid(), ascii::space_type> oid;
	qi::rule<Iterator, void(), ascii::space_type> oids;

	qi::rule<Iterator, data::obj(), ascii::space_type> obj;
	qi::rule<Iterator, void(), ascii::space_type> objs;

	qi::rule<Iterator, data::type(), ascii::space_type> type;

	qi::rule<Iterator, void(), ascii::space_type> start;
};

bool parser::load(std::string filename){
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

	mib_parser<std::string::const_iterator> g;
	bool r = qi::phrase_parse(iter, end, g, ascii::space);

	return r;
}

bool parser::load(std::string filename, std::vector<std::string> imports) {
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

	mib_parser<std::string::const_iterator> g;
	bool r = qi::phrase_parse(iter, end, g, ascii::space);

	return r;
}
