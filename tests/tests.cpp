#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "../tree/Node.h"
#include "../NFA/NFA.h"
#include "../DFA/DFA.h"
#include "../MDFA/MDFA.h"
#include "../Regex/Regex.h"
#include "../StateElimination//StateElim.h"
std::string getPostOrderStr(Node* root) {
    if (!root) return "";
    std::vector<Node*> nodes;
    postOrder(root, nodes);
    std::string res;
    for (auto n : nodes) {
        if (n->type == CONCAT) res += ".";
        else if (n->type == OR) res += "|";
        else if (n->type == STAR) res += "*";
        else res += n->name;
    }
    return res;
}

TEST_CASE("Parser basic symbols and concatenation") {
    SECTION("Single symbol") {
        auto res = Parser("a");
        REQUIRE(res.mainTree != nullptr);
        CHECK(res.mainTree->name == 'a');
        CHECK(res.mainTree->type == SYM);
    }

    SECTION("concatenation") {
        auto res = Parser("ab");
        CHECK(getPostOrderStr(res.mainTree.get()) == "ab.");
    }
}

TEST_CASE("Parser unary and binary operators") {
    SECTION("Kleene star") {
        auto res = Parser("a*");
        CHECK(getPostOrderStr(res.mainTree.get()) == "a*");
    }

    SECTION("a+") {
        auto res = Parser("a+");
        CHECK(getPostOrderStr(res.mainTree.get()) == "aa*.");
    }

    SECTION("") {
        auto res = Parser("a|b");
        CHECK(getPostOrderStr(res.mainTree.get()) == "ab|");
    }

    SECTION("Priority and parentheses") {
        auto res = Parser("a|bc*");
        CHECK(getPostOrderStr(res.mainTree.get()) == "abc*.|");
        auto res2 = Parser("(a|b)c");
        CHECK(getPostOrderStr(res2.mainTree.get()) == "ab|c.");
    }
}

TEST_CASE("Parser complex structures") {
    SECTION("Ranges [a-z]") {
        auto res = Parser("[abc]");
        CHECK(getPostOrderStr(res.mainTree.get()) == "ab|c|");
    }

    SECTION("Repetitions {m,n}") {
        auto res = Parser("a{1,2}");
        CHECK(getPostOrderStr(res.mainTree.get()) == "aaa.|");
    }

    SECTION("Repetitions {m,}") {
        auto res = Parser("a{2,}");
        CHECK(getPostOrderStr(res.mainTree.get()) == "aa.a*.");
    }
}

TEST_CASE("Parser Lookahead and Escaping") {
    SECTION("/") {
        auto res = Parser("abc/de");
        CHECK(res.hasLookahead == true);
        CHECK(res.lookahead == "de");
    }

    SECTION("%") {
        auto res = Parser("%*");
        CHECK(res.mainTree->name == '*');
        CHECK(res.mainTree->type == SYM);
    }
}
TEST_CASE("Error Handling") {
    CHECK_THROWS(Parser("(a|b"));
    CHECK_THROWS(Parser("a|"));
    CHECK_THROWS(Parser("*a"));
    CHECK_THROWS(Parser("[a-z"));
    CHECK_THROWS(Parser("a{3,1}"));
}
TEST_CASE("Parser Quantifiers {m,n}") {
    auto checkBounds = [](const std::string& input) {
        size_t i = 0;
        return parseBounds(input, i);
    };
    SECTION("Successful parsing") {
        auto res1 = checkBounds("{3}");
        CHECK(res1.first == 3);
        CHECK(res1.second == 3);
        auto res2 = checkBounds("{2,5}");
        CHECK(res2.first == 2);
        CHECK(res2.second == 5);
        auto res3 = checkBounds("{4,}");
        CHECK(res3.first == 4);
        CHECK(res3.second == -1);
        auto res4 = checkBounds("{,10}");
        CHECK(res4.first == 0);
        CHECK(res4.second == 10);
    }
    SECTION("Error handling") {
        CHECK_THROWS_WITH(checkBounds("{}"), "Invalid {");
        CHECK_THROWS_WITH(checkBounds("{a}"), "Invalid {");
        CHECK_THROWS_WITH(checkBounds("{,a}"), "Invalid {,}");
        CHECK_THROWS_WITH(checkBounds("{,}"), "Invalid {,}");
        CHECK_THROWS_WITH(checkBounds("{5a}"), "Invalid {m...}");
        CHECK_THROWS_WITH(checkBounds("{5,10"), "No }");
        CHECK_THROWS_WITH(checkBounds("{5,2}"), "Invalid range");
    }
    SECTION("Repeat from zero") {
        auto res = Parser("a{0,1}");
        CHECK(getPostOrderStr(res.mainTree.get()) == "$a|");
    }
    SECTION("Repeat with m=0") {
        auto res = Parser("a{0,}");
        CHECK(getPostOrderStr(res.mainTree.get()) == "a*");
    }
}
TEST_CASE("NFA") {
    NFA nfa;
    SECTION("Single symbol compilation") {
        auto root = std::make_unique<Node>('a');
        root->type = SYM;
        REQUIRE_NOTHROW(nfa.compile(root.get()));
    }

    SECTION("ab") {
        auto a = std::make_unique<Node>('a'); a->type = SYM;
        auto b = std::make_unique<Node>('b'); b->type = SYM;
        auto concat = std::make_unique<Node>('.', std::move(a), std::move(b));
        concat->type = CONCAT;
        REQUIRE_NOTHROW(nfa.compile(concat.get()));
    }
    SECTION("a|b") {
        auto a = std::make_unique<Node>('a'); a->type = SYM;
        auto b = std::make_unique<Node>('b'); b->type = SYM;
        auto orNode = std::make_unique<Node>('|', std::move(a), std::move(b));
        orNode->type = OR;
        REQUIRE_NOTHROW(nfa.compile(orNode.get()));
    }
    SECTION("a*") {
        auto a = std::make_unique<Node>('a'); a->type = SYM;
        auto star = std::make_unique<Node>('*', std::move(a), nullptr);
        star->type = STAR;
        REQUIRE_NOTHROW(nfa.compile(star.get()));
    }
    SECTION("Error handling") {
        CHECK_THROWS_AS(nfa.compile(nullptr), std::invalid_argument);
        auto badOr = std::make_unique<Node>('|');
        badOr->type = OR;
        CHECK_THROWS(nfa.compile(badOr.get()));
    }
}
TEST_CASE("DFA") {
    NFA nfa;
    State* s0 = nfa.create();
    State* s1 = nfa.create();
    State* s2 = nfa.create();
    nfa.addTransition(s0, s1, '$');
    nfa.addTransition(s1, s2, 'a');
    nfa.entry = s0;
    DFA dfa;
    SECTION("Epsilon Closure") {
        auto closure = dfa.epsClosure(s0, nfa);
        CHECK(closure.count(s0) == 1);
        CHECK(closure.count(s1) == 1);
        CHECK(closure.count(s2) == 0);
    }

    SECTION("Tmove") {
        std::set<State*> group = {s1};
        auto moves = dfa.Tmove(group, nfa, 'a');
        CHECK(moves.size() == 1);
        CHECK(moves.count(s2) == 1);
    }
}

TEST_CASE("DFA2") {
    SECTION("Simple Regex a|b") {
        auto res = Parser("a|b");
        NFA nfa;
        nfa.compile(res.mainTree.get());
        nfa.alphabet = {'a', 'b'};
        DFA dfa;
        dfa.process(nfa);
        CHECK(dfa.startDFA == 0);
        CHECK(!dfa.finalDFA.empty());
    }
    SECTION("Complete DFA") {
        auto res = Parser("a");
        NFA nfa;
        nfa.compile(res.mainTree.get());
        nfa.alphabet = {'a', 'b'};
        DFA dfa;
        dfa.process(nfa);
        int startId = dfa.startDFA;
        if (dfa.transitionTable[startId].contains('b')) {
            int trapId = dfa.transitionTable[startId]['b'];
            CHECK(dfa.transitionTable[trapId]['b'] == trapId);
        }
    }
}
TEST_CASE("MDFA Minimization") {
    DFA dfa;
    dfa.alphabet = {'a'};
    dfa.transitionTable[0]['a'] = 1;
    dfa.transitionTable[1]['a'] = 2;
    dfa.transitionTable[2]['a'] = 2;
    dfa.finalDFA = {0, 1, 2};
    dfa.startDFA = 0;
    MDFA mdfa;
    mdfa.minimize(dfa);
}
TEST_CASE("MDFA Set Operations") {
    MDFA mdfaA;
    MDFA mdfaB;
SECTION("Full match abc") {
    auto res = Parser("abc");
    NFA nfa;
    nfa.compile(res.mainTree.get());
    nfa.alphabet = {'a', 'b', 'c'};
    DFA dfa;
    dfa.process(nfa);
    MDFA mdfa;
    mdfa.minimize(dfa);
    CHECK(mdfa.match("abc") == true);
    CHECK(mdfa.match("ab") == false);
    CHECK(mdfa.match("abcd") == false);
    CHECK(mdfa.search("dsadasdabc")==true);
}

    SECTION("Get All Final Indices") {
        auto res = Parser("aa*");
        NFA nfa; nfa.compile(res.mainTree.get());
        nfa.alphabet = {'a'};
        DFA dfa; dfa.process(nfa);
        MDFA mdfa; mdfa.minimize(dfa);
        std::vector expected = {1, 2, 3};
        CHECK(mdfa.getAllFinInd("aaa") == expected);
        std::vector expectedShort = {1, 2};
        CHECK(mdfa.getAllFinInd("aab") == expectedShort);
    }
}
TEST_CASE("Regex and Lookahead") {
    SECTION("Basic Regex (no lookahead)") {
        Regex re("abc");
        CHECK(re.match("abc") == true);
        CHECK(re.match("ab") == false);
        CHECK(re.match("abcd") == false);
    }
    SECTION("Lookahead ") {
        Regex re("a/b");
        CHECK(re.match("ab") == true);
        CHECK(re.match("ac") == false);
    }
    SECTION("Lookahead tails") {
        Regex re("a/bc");
        CHECK(re.match("abc") == true);
        CHECK(re.match("ab") == false);
    }
    SECTION("Compilation") {
        Regex re("a");
        REQUIRE_NOTHROW(re.match("a"));
    }
}
TEST_CASE("Regex Search") {
    SECTION("Subline searching") {
        Regex re("abc");
        CHECK(re.search("xyz") == false);
    }
}
TEST_CASE("State Elimination") {
    StateElim eliminator;
    SECTION("Simple transition: a") {
       Regex re("a");
       re.comp();
       std::string s = eliminator.getRegex(re.mainAutomata);
       Regex re2(s);
       re2.comp();
       CHECK(MDFA::equal(re.mainAutomata,re2.mainAutomata));
    }
    SECTION("Parallel edges: a|b") {
        Regex re("a|b");
        re.comp();
        std::string s = eliminator.getRegex(re.mainAutomata);
        Regex re2(s);
        re2.comp();
        CHECK(MDFA::equal(re.mainAutomata,re2.mainAutomata));
    }
    SECTION("Self-loop: a*") {
        Regex re("a*");
        re.comp();
        std::string s = eliminator.getRegex(re.mainAutomata);
        Regex re2(s);
        re2.comp();
        CHECK(MDFA::equal(re.mainAutomata,re2.mainAutomata));
    }
    SECTION("Complex path: ab*c") {
        Regex re("ab*c");
        re.comp();
        std::string s = eliminator.getRegex(re.mainAutomata);
        Regex re2(s);
        re2.comp();
        CHECK(MDFA::equal(re.mainAutomata,re2.mainAutomata));
    }
    SECTION("Complex path: a{1,5}b+|a*[f-z]") {
        Regex re("ab*c");
        re.comp();
        std::string s = eliminator.getRegex(re.mainAutomata);
        Regex re2(s);
        re2.comp();
        CHECK(MDFA::equal(re.mainAutomata,re2.mainAutomata));
    }
}

TEST_CASE("Empty") {
    SECTION("Matching empty") {
        Regex re("");
        CHECK(re.match("") == false);
        CHECK(re.match("a") == false);
    }
}
TEST_CASE("Empty Language") {
    SECTION("MDFA") {
        MDFA mdfa;
        mdfa.setStart(0);
        mdfa.addFinal(1);
        CHECK(mdfa.isEmpty() == true);
        CHECK(mdfa.match("") == false);
        CHECK(mdfa.match("a") == false);
    }
    SECTION("StateElim for Empty") {
        Regex r("ab|cd*f+");
        r.comp();
        StateElim elim;
        MDFA resEmpty = MDFA::diff(r.mainAutomata,r.mainAutomata,true);
        std::string res = elim.getRegex(resEmpty);
        CHECK(res.empty());
    }
}
TEST_CASE("MDFA Product") {
    SECTION("Intersection") {
        Regex re("ab|bc");
        Regex r1("ab|cd");
        re.comp(); r1.comp();
        MDFA res = MDFA::diff(re.mainAutomata, r1.mainAutomata, false);
        CHECK(res.match("ab") == true);
        CHECK(res.match("b") == false);
        CHECK(res.isEmpty() == false);
    }

    SECTION("Intersection with Empty") {
        Regex re("ab|cd");
        re.comp();
        MDFA res = MDFA::diff(re.mainAutomata,re.mainAutomata, true);

        MDFA resEmpty = MDFA::diff(re.mainAutomata, res, false);
        CHECK(resEmpty.isEmpty() == true);
        CHECK(resEmpty.match("ab") == false);
    }

    SECTION("Difference") {
        Regex re("ab|cd");
        re.comp();
        Regex r1("ab|fg");
        MDFA res = MDFA::diff(re.mainAutomata,r1.mainAutomata, true);
        CHECK(re.mainAutomata.isEmpty() == false);
        CHECK(res.match("cd") == true);
    }

    SECTION("Difference with Empty - main") {
        Regex re("ab|cd");
        re.comp();
        MDFA res = MDFA::diff(re.mainAutomata,re.mainAutomata, true);
        MDFA resEmpty = MDFA::diff(res, re.mainAutomata, true);
        CHECK(resEmpty.isEmpty() == true);
        CHECK(resEmpty.match("ab") == false);
    }
    SECTION("Equality fail") {
        Regex re("ab|c");
        Regex r1("a*");
        re.comp(); r1.comp();
        bool res = MDFA::equal(re.mainAutomata,r1.mainAutomata);
        CHECK(res == false);
    }
}