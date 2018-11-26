#include <parser.hpp>
#include <object.hpp>
#include <tree.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <fstream>
#include <tuple>

namespace qi = boost::spirit::qi;
namespace fusion = boost::fusion;
namespace ascii = boost::spirit::ascii;

parser p;

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

	struct seq_member
	{
		std::string name;
		std::string type;
		std::string cons;
	};

	struct obj
	{
		std::string name;
		std::string typeName;
		std::vector<data::seq_member> sequence;
		std::string constraints;
		std::string access;
		std::string status;
		std::string desc;
		struct path path;
	};

	struct type
	{
		std::string name;
		std::string visibility;
		std::string mode;
		std::string typeName;
		std::vector<data::seq_member> sequence;
		std::string constraints;
	};
}

BOOST_FUSION_ADAPT_STRUCT(
	data::seq_member,
	(std::string, name),
	(std::string, type),
	(std::string, cons)
)

BOOST_FUSION_ADAPT_STRUCT(
	data::type,
	(std::string, name),
	(std::string, visibility),
	(std::string, mode),
	(std::string, typeName),
	(std::vector<data::seq_member>, sequence),
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
	(std::string, typeName),
	(std::vector<data::seq_member>, sequence),
	(std::string, constraints),
	(std::string, access),
	(std::string, status),
	(std::string, desc),
	(data::path, path)
)

void import_file(data::import const& import){
	std::vector<std::string> imports;
	for(auto const& val : import.names){
		if(val.compare("OBJECT-TYPE") != 0)
			imports.push_back(val);
	}

	if(imports.size() > 0){
		std::cout<<"Importing file: "<<import.from<<std::endl;
		p.load(import.from);
	}
}

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
	std::vector<seq_type> seq_types;
	for(auto const& val : obj.sequence){
		seq_type seq;
		seq.name = val.name;

		std::string::const_iterator iter = val.cons.begin();
		std::string::const_iterator end = val.cons.end();

		std::vector<unsigned int> constr;

		namespace qi = boost::spirit::qi;
		bool r = qi::parse(iter, end, qi::uint_ % qi::lit(".."), constr);
		seq.cons = constr;

		seq.type = val.type;
		seq_types.push_back(seq);
	}
	Object *object = new Object(obj.name, obj.typeName, obj.constraints, seq_types, obj.access, obj.status, obj.desc);
	ObjectPath path;
	path.path.push_back(std::make_tuple(obj.path.startNode, -1));
	for(auto const& val : obj.path.path){
		path.path.push_back(std::make_tuple(fusion::at_c<0>(val), fusion::at_c<1>(val)));
	}
	path.path.push_back(std::make_tuple("", obj.path.destId));
	tree->addObject(object, path);
}

void add_type(data::type const& type){
	std::vector<seq_type> seq_types;
	for(auto const& val : type.sequence){
		seq_type seq;
		seq.name = val.name;

		std::string::const_iterator iter = val.cons.begin();
		std::string::const_iterator end = val.cons.end();

		std::vector<unsigned int> constr;

		namespace qi = boost::spirit::qi;
		bool r = qi::parse(iter, end, qi::uint_ % qi::lit(".."), constr);
		seq.cons = constr;

		seq.type = val.type;
		seq_types.push_back(seq);
	}
	Type t(type.name, type.visibility, type.typeName, type.constraints, seq_types);
	types.push_back(t);
}

template <typename Iterator>
struct skip_grammar : qi::grammar<Iterator, void()>
{
	skip_grammar() : skip_grammar::base_type(skip, "SKIP")
	{
	name = +(qi::alnum | qi::char_("-"));
	comment = qi::lit("--") >> *(qi::char_ - qi::eol) >> qi::eol;
	exports = qi::lit("EXPORTS") >> +(qi::char_ - ';') >> ';';
	definitions = name >> qi::lit(" DEFINITIONS ::= BEGIN");

	skip = comment | exports | definitions | ascii::space;
	}

	qi::rule<std::string::const_iterator, void()> comment;
	qi::rule<std::string::const_iterator, void()> exports;
	qi::rule<std::string::const_iterator, void()> definitions;
	qi::rule<std::string::const_iterator, void()> name;
	qi::rule<std::string::const_iterator, void()> skip;
};

template <typename Iterator, typename SkipType>
struct mib_parser : qi::grammar<Iterator, void(), SkipType>
{
	mib_parser() : mib_parser::base_type(start)
	{
		name %= qi::lexeme[+(ascii::alnum | qi::char_("-"))];
		names %= name % ',';

		syntax_part %= qi::lexeme[+(qi::char_ - ascii::space)];
		syntax %= *(syntax_part >> !&qi::lit("ACCESS")) >> syntax_part;

		import = names >> qi::lit("FROM") >> name;
		imports = qi::lit("IMPORTS") >> +import[&import_file] >> ';';

		path %= qi::lit("::=") >> '{' >> name >> *(name >> '(' >> qi::int_ >> ')') >> qi::int_ >> '}';

		desc %= '"' >> qi::lexeme[+(qi::char_ - '"')] >> '"';

		oid %= name >> qi::lit("OBJECT IDENTIFIER") >> path;
		oids = +oid[&add_oid];

		obj %= name >> qi::lit("OBJECT-TYPE") >> qi::lit("SYNTAX") >> type_name >> -('{' >> seq_member % ',' >> '}') >> -(range | size) >> qi::lit("ACCESS") >> name >> qi::lit("STATUS") >> name >> qi::lit("DESCRIPTION") >> desc >> path;
		objs = +obj[&add_obj];

		visibility %= '[' >> qi::lexeme[+(qi::char_ - ']')] >> ']';

		mode %= qi::string("EXPLICIT") | qi::string("IMPLICIT");

		//constr %= '(' >> qi::lexeme[+(qi::char_ - ')')] >> ')';

		range %= '(' >> qi::lexeme[+(ascii::digit)] >> qi::string("..") >> qi::lexeme[+(ascii::digit)] >> ')';

		size %= '(' >> qi::lit("SIZE") >> '(' >> qi::lexeme[+(ascii::digit)] >> ')' >> ')';

		type_name %= qi::string("INTEGER") | qi::string("OCTET STRING") | qi::string("OBJECT IDENTIFIER") | qi::string("NULL") | (qi::string("SEQUENCE OF ") >> type_name) | name;

		//type_syntax %= type_name >> -(qi::string("{") >> qi::lexeme[+(qi::char_ - '}')]>> qi::string("}"));

		seq_member %= name >> type_name >> -(range|size);

		type %= name >> qi::lit("::=") >> -visibility >> -mode >> type_name >> -('{' >> seq_member % ',' >> '}') >> -(range | size);
		types = +type[&add_type];

		macro = name >> qi::lit("MACRO") >> qi::lit("::=") >> qi::lit("BEGIN") >> *(syntax_part >> !&qi::lit("END")) >> syntax_part >> qi::lit("END");

		hack = qi::lexeme[qi::char_ - qi::eol];

		start = -imports >> *(oids | objs | types | macro | hack);
	}

	qi::rule<Iterator, std::string(), SkipType> name;
	qi::rule<Iterator, std::vector<std::string>(), SkipType> names;
	qi::rule<Iterator, data::path(), SkipType> path;
	qi::rule<Iterator, std::string(), SkipType> desc;
	qi::rule<Iterator, std::string(), SkipType> syntax_part;
	qi::rule<Iterator, std::vector<std::string>(), SkipType> syntax;

	qi::rule<Iterator, std::string(), SkipType> visibility;
	qi::rule<Iterator, std::string(), SkipType> mode;
	qi::rule<Iterator, data::seq_member(), SkipType> seq_member;
	qi::rule<Iterator, std::string(), SkipType> type_syntax;
	qi::rule<Iterator, std::string(), SkipType> type_name;
	qi::rule<Iterator, std::string(), SkipType> range;
	qi::rule<Iterator, std::string(), SkipType> size;

	qi::rule<Iterator, data::import(), SkipType> import;
	qi::rule<Iterator, void(), SkipType> imports;

	qi::rule<Iterator, data::oid(), SkipType> oid;
	qi::rule<Iterator, void(), SkipType> oids;

	qi::rule<Iterator, data::obj(), SkipType> obj;
	qi::rule<Iterator, void(), SkipType> objs;

	qi::rule<Iterator, data::type(), SkipType> type;
	qi::rule<Iterator, void(), SkipType> types;

	qi::rule<Iterator, void(), SkipType> macro;

	qi::rule<Iterator, void(), SkipType> hack;

	qi::rule<Iterator, void(), SkipType> start;
};

bool parser::load(std::string filename){
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

	skip_grammar<std::string::const_iterator> skip;
	mib_parser<std::string::const_iterator, BOOST_TYPEOF(skip)> g;
	bool r = qi::phrase_parse(iter, end, g, skip);

	return r;
}

/*
bool parser::load(std::string filename, std::vector<std::string> imports) {
	std::ifstream ifs(filename);
	std::string str((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

	std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();

	qi::rule<std::string::const_iterator, void()> comment;
	comment = (qi::lit("--") >> *(qi::char_ - qi::eol) >> qi::eol) | ascii::space;

	mib_parser<std::string::const_iterator, BOOST_TYPEOF(comment)> g;
	bool r = qi::phrase_parse(iter, end, g, comment);

	return r;
}
*/