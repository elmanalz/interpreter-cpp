// tests.cpp -- test suite for the expression interpreter.
//
// Build (multi-file layout):
//     make run
// or directly:
//     g++ -std=c++17 -Wall -Wextra -pedantic tests.cpp lexer.cpp ast.cpp parser.cpp -o tests
//
// If you wrapped everything in `namespace calc`, add `using namespace calc;`
// below the includes.

#include "lexer.h"
#include "parser.h"

#include <cmath>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Harness
// ---------------------------------------------------------------------------

static int checks_run = 0;
static int checks_failed = 0;

static void section(const std::string& title)
{
    std::cout << "\n[" << title << "]\n";
}

static void pass(const std::string& what)
{
    std::cout << "  pass   " << what << '\n';
}

static void fail(const std::string& what)
{
    ++checks_failed;
    std::cout << "  FAIL   " << what << '\n';
}

// Parse `src` and assert the parenthesised tree matches `want`.
// This checks STRUCTURE -- precedence and associativity -- which a numeric
// result cannot: 2*3*4 is 24 under either associativity.
static void tree(const std::string& src, const std::string& want)
{
    ++checks_run;
    try {
        Parser p(calc::lex(src));
        const std::string got = p.parse()->to_string();
        if (got == want) pass("\"" + src + "\"  ->  " + got);
        else             fail("\"" + src + "\"\n            got:  " + got +
                              "\n           want:  " + want);
    } catch (const std::exception& e) {
        fail("\"" + src + "\"  threw: " + e.what());
    }
}

// Parse and evaluate `src`, assert the value is `want` within tolerance.
static void value(const std::string& src, double want)
{
    ++checks_run;
    try {
        Parser p(calc::lex(src));
        const double got = p.parse()->eval();
        if (std::fabs(got - want) < 1e-9)
            pass("\"" + src + "\" = " + std::to_string(got));
        else
            fail("\"" + src + "\" = " + std::to_string(got) +
                 ", want " + std::to_string(want));
    } catch (const std::exception& e) {
        fail("\"" + src + "\"  threw: " + e.what());
    }
}

// Assert that `src` is rejected -- by the lexer, the parser, or the evaluator.
// A parser that accepts everything looks fine until someone makes a typo.
static void rejects(const std::string& src)
{
    ++checks_run;
    try {
        Parser p(calc::lex(src));
        auto root = p.parse();
        root->eval();
        fail("\"" + src + "\" should have been rejected");
    } catch (const std::exception&) {
        pass("\"" + src + "\" rejected");
    }
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

int main()
{
    std::cout << "Running interpreter tests\n";

    // -----------------------------------------------------------------------
    section("single values");

    tree("42", "42");
    tree("3.5", "3.5");
    tree("0", "0");
    value("42", 42);
    value("3.5", 3.5);

    // -----------------------------------------------------------------------
    section("precedence");

    // These prove * binds tighter than +, in both orders.
    tree("1+2*3", "(1 + (2 * 3))");
    tree("2*3+4", "((2 * 3) + 4)");
    tree("1-2*3-4", "((1 - (2 * 3)) - 4)");
    tree("2^3*4", "((2 ^ 3) * 4)");        // ^ binds tighter than *
    tree("2*3^4", "(2 * (3 ^ 4))");
    value("1+2*3", 7);
    value("2*3+4", 10);

    // -----------------------------------------------------------------------
    section("left associativity");

    // The critical tests. With + and * alone these would pass under EITHER
    // associativity, so every case here uses - or / where order matters.
    tree("10-3-2", "((10 - 3) - 2)");
    tree("100/10/2", "((100 / 10) / 2)");
    tree("1-2-3-4", "(((1 - 2) - 3) - 4)");
    tree("2*3*4", "((2 * 3) * 4)");
    tree("1+2+3+4", "(((1 + 2) + 3) + 4)");
    value("10-3-2", 5);      // right-assoc would give 9
    value("100/10/2", 5);    // right-assoc would give 20

    // -----------------------------------------------------------------------
    section("right associativity of ^");

    tree("2^3^2", "(2 ^ (3 ^ 2))");
    value("2^3^2", 512);     // left-assoc would give 64
    value("2^10", 1024);

    // -----------------------------------------------------------------------
    section("parentheses");

    tree("(1+2)*3", "((1 + 2) * 3)");
    tree("2*(3+4)", "(2 * (3 + 4))");
    tree("((((1))))", "1");
    tree("(1+2)*(3+4)", "((1 + 2) * (3 + 4))");
    tree("2*(3+4)*5", "((2 * (3 + 4)) * 5)");
    value("(2+3)*4", 20);
    value("(1+2)*(3+4)", 21);

    // -----------------------------------------------------------------------
    section("unary");

    tree("-3", "(-3)");
    tree("+5", "(+5)");
    tree("--5", "(-(-5))");
    tree("-3*2", "((-3) * 2)");
    tree("2*-3", "(2 * (-3))");
    tree("1--2", "(1 - (-2))");        // binary minus then unary minus
    tree("2^-1", "(2 ^ (-1))");
    tree("-2^2", "(-(2 ^ 2))");        // unary above factor: -(2^2), not (-2)^2
    tree("(-2)^2", "((-2) ^ 2)");
    value("--5", 5);
    value("-3*2", -6);
    value("2^-1", 0.5);
    value("-2^2", -4);
    value("(-2)^2", 4);

    // -----------------------------------------------------------------------
    section("modulo and division");

    value("7%3", 1);
    value("7.5%2", 1.5);      // fmod, not integer truncation
    value("7/2", 3.5);
    value("-7%3", -1);

    // -----------------------------------------------------------------------
    section("whitespace is irrelevant");

    tree("  2  *  3  ", "(2 * 3)");
    tree("2\t*\t3", "(2 * 3)");
    tree("1 + 2 * 3", "(1 + (2 * 3))");

    // -----------------------------------------------------------------------
    section("a big one");

    tree("6^6*2*(8+4)^2/36", "((((6 ^ 6) * 2) * ((8 + 4) ^ 2)) / 36)");
    value("6^6*2*(8+4)^2/36", 373248);
    value("2+3*(4-1)^2^2/-6", -38.5);

    // -----------------------------------------------------------------------
    section("malformed input must be rejected");

    rejects("");            // nothing at all
    rejects("   ");         // whitespace only
    rejects("2*");          // operator with no right operand
    rejects("*3");          // operator with no left operand
    rejects("2**3");        // two binary operators in a row
    rejects("2^");
    rejects("1+");
    rejects("+");           // a lone operator
    rejects("*");
    rejects(")");
    rejects("42 43");       // trailing input
    rejects("1 2 3");
    rejects("(1+2");        // unclosed paren
    rejects("1+2)");        // unopened paren
    rejects("()");          // empty parens
    rejects("2*(");
    rejects("1 @ 2");       // character the lexer cannot start a token with
    rejects("1.2.3");       // two decimal points
    rejects("1/0");         // division by zero
    rejects("1%0");         // modulo by zero
    rejects("x+1");         // unknown identifier

    // -----------------------------------------------------------------------
    std::cout << "\n----------------------------------------\n"
              << (checks_run - checks_failed) << " / " << checks_run
              << " checks passed\n";

    if (checks_failed == 0) {
        std::cout << "ALL TESTS PASSED\n";
        return 0;
    }
    std::cout << checks_failed << " CHECK(S) FAILED\n";
    return 1;   // non-zero exit code: build systems and CI read this
}
