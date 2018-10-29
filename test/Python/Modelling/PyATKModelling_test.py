#!/usr/bin/env python

from ATK.Core import DoubleInPointerFilter, DoubleOutPointerFilter
from ATK.Modelling import AST, DoubleModellerFilter

def createAST_test():
    ast = AST()

def parseSimpleAST_test():
    ast = AST()
    ast.parse_string("R0 out ref1 200e3")

def createDynamic_test():
    ast = AST()
    filter = DoubleModellerFilter.create_dynamic_filter(ast)
