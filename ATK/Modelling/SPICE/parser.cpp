/**
 * \file parser.cpp
 */

#include <algorithm>
#include <fstream>

#include <boost/algorithm/string/case_conv.hpp>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/SPICE/parser.h>

namespace ATK
{
namespace
{
  auto tolower = [](auto& ctx){ _val(ctx) += std::tolower(_attr(ctx)); };

  // convert a SPICE number to an actual number
  double convert_suffix(const std::string& suffix)
  {
    if(suffix.empty())
    {
      return 1;
    }
    if(suffix.size() > 2)
    {
      std::string cpy(suffix.substr(0, 3));
      boost::algorithm::to_lower(cpy);
      if(cpy == "mil")
      {
        return 2.54e-6;
      }
      if(cpy == "meg")
      {
        return 1e6;
      }
    }
    switch(std::tolower(suffix[0]))
    {
      case 'f': return 1.e-15;
      case 'p': return 1.e-12;
      case 'n': return 1.e-9;
      case 'u': return 1.e-6;
      case 'm': return 1.e-3;
      case 'k': return 1.e3;
      case 'g': return 1.e9;
      case 't': return 1.e12;
      default:
        return 1;
    }
  }
}
namespace parser
{
  namespace x3 = boost::spirit::x3;
  namespace ascii = boost::spirit::x3::ascii;

// Rule to bypass comments, we don't bypass spaces between
auto const space_comment = x3::lexeme[ '*' >> *(x3::char_ - x3::eol) >> x3::eol];

const auto component_name = x3::rule<class name, std::string>()
  = (x3::char_(
               "cC"
               "lL"
               "rR"
               "vV"
               )[tolower] >> *(x3::alnum[tolower] | x3::punct));

const auto component_value = x3::rule<class component_value, ast::SPICENumber>()
  = x3::double_ >> *(x3::char_ - x3::space); // to lower is done in the transofrmation function

const auto pin = x3::rule<class pin, std::string>()
  = (x3::alpha[tolower] >> *(x3::alnum[tolower] | x3::punct));
  
const auto component_arg = x3::rule<class component_arg, ast::SPICEArg>()
  = component_value | pin;

const auto component_arguments = x3::rule<class component_arguments, std::vector<ast::SPICEArg>>()
  = component_arg % +(x3::lit(' ') | x3::lit('\n'));

const auto component = x3::rule<class component, ast::Component>()
  = component_name >> +(x3::lit(' ') | x3::lit('\n')) >> component_arguments >> *(x3::lit(' ') | x3::lit('\n'));

const auto entry = x3::rule<class entry, ast::SPICEEntry>()
  = component;

}

/// to replace the apply visit that is missing in boost::variant comapred to std
namespace
{
  template <typename ReturnT, typename... Lambdas>
  struct lambda_visitor;
  
  template <typename ReturnT, typename L1, typename... Lambdas>
  struct lambda_visitor< ReturnT, L1 , Lambdas...> : public L1, public lambda_visitor<ReturnT, Lambdas...>
  {
    using L1::operator();
    using lambda_visitor< ReturnT , Lambdas...>::operator();
    lambda_visitor(L1 l1, Lambdas... lambdas)
    : L1(l1), lambda_visitor< ReturnT , Lambdas...> (lambdas...)
    {}
  };
  
  template <typename ReturnT, typename L1>
  struct lambda_visitor<ReturnT, L1> : public L1, public boost::static_visitor<ReturnT>
  {
    using L1::operator();
    lambda_visitor(L1 l1)
    :  L1(l1), boost::static_visitor<ReturnT>()
    {}
  };
  
  template <typename ReturnT>
  struct lambda_visitor<ReturnT> : public boost::static_visitor<ReturnT>
  {
    lambda_visitor() : boost::static_visitor<ReturnT>() {}
  };
  
  template <typename ReturnT, typename... Lambdas>
  lambda_visitor<ReturnT, Lambdas...> make_lambda_visitor(Lambdas... lambdas)
  {
    return { lambdas... };
  }
  
  /// Converts a parsed entry to an AST entry
  void populate_entry(ast::SPICEAST& currentAST, ast::SPICEEntry entry)
  {
    auto visitor = make_lambda_visitor<void>(
                                             [&](ast::Component& arg) { currentAST.components.insert(std::move(arg)); },
                                             [&](ast::Model& arg) { }
                                             );
    boost::apply_visitor(visitor, std::move(entry));
  }
}
  
double convert_component_value(const ast::SPICENumber& value)
{
  return value.first * convert_suffix(value.second);
}

double parse_component_value(const std::string_view str)
{
  auto iter = str.begin();
  auto end = str.end();
  ast::SPICENumber value;
  bool r = phrase_parse(iter, end, parser::component_value, parser::space_comment, value);
  if(!r)
  {
    throw ATK::RuntimeError("Failed to parse value");
  }
  if(iter != end)
  {
    throw ATK::RuntimeError("Failed to parse line, reminder is " + std::string(iter, end));
  }
  return convert_component_value(value);
}

void parse_string(ast::SPICEAST& currentAST, const std::string_view str)
{
  auto iter = str.begin();
  auto end = str.end();
  ast::SPICEEntry entry;
  bool r = phrase_parse(iter, end, parser::entry, parser::space_comment, entry);
  if(!r)
  {
    throw ATK::RuntimeError("Failed to parse line");
  }
  if(iter != end)
  {
    throw ATK::RuntimeError("Failed to parse line, reminder is " + std::string(iter, end));
  }
  populate_entry(currentAST, entry);
}
}
