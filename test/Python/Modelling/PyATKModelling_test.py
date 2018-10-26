#!/usr/bin/env python

from ATK.Core import DoubleInPointerFilter, DoubleOutPointerFilter
from ATK.Modelling import AST

def createAST_test():
    ast = AST()

def parseSimpleAST_test():
    ast = AST()
    ast.parse_string("R0 out ref1 200e3")
