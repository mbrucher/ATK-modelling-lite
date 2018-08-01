/**
 * \file SPICEHandler.cpp
 */

#include <algorithm>
#include <fstream>

#include <ATK/Core/Utilities.h>

#include <ATK/Modelling/SPICE/SPICEHandler.h>
#include <ATK/Modelling/SPICE/parser.h>

namespace ATK
{
  SPICEHandler::SPICEHandler(const ast::SPICEAST& tree)
  :tree(tree)
  {
    
  }

  std::tuple<gsl::index, gsl::index, gsl::index> SPICEHandler::get_pins() const
  {
    return std::make_tuple(0, 0, 0);
  }
}
