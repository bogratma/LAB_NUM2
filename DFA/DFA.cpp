//
// Created by vlad on 3/17/26.
//

#include "DFA.h"
#include <iostream>
#include <queue>

void DFA::process(NFA& nfa) {
      std::queue<std::set<State*>> q;
      int id=0;
      std::set<State*> start = epsClosure(nfa.entry,nfa);
      tableDFA[start] = id++;
      this->startDFA=0;
      q.push(start);
      for (auto s : start) {
            if (s->isAcceptable) {
                  finalDFA.insert(0);
            }
      }
      while (!q.empty()) {
            std::set<State*> s = q.front();
            q.pop();
            int gId = tableDFA[s];
            for (const char c : nfa.alphabet) {
                  if (c=='$') continue;
                  std::set<State*> nexts;
                  for (auto f: s) {
                        auto range = nfa.table.equal_range(std::make_pair(f,c));
                        for (auto it = range.first; it != range.second; ++it) {
                              nexts.insert(it->second);
                        }
                  }
                  if (nexts.empty()) continue;
                  std::set<State*> closureGroup;
                  for (const auto cl: nexts) {
                        DFS(cl,nfa,closureGroup);
                  }
                  if (!tableDFA.contains(closureGroup)) {
                        int v = id++;
                        tableDFA[closureGroup] = v;
                        q.push(closureGroup);
                        for (auto s : closureGroup) {
                              if (s->isAcceptable) {
                                    finalDFA.insert(v);
                              }
                        }
                  }
                  transitionTable[gId][c] = tableDFA[closureGroup];
            }
      }
}
std::set<State*> DFA::epsClosure(State* p, NFA& nfa) {
      std::set<State*> epsClose;
      DFS(p,nfa,epsClose);
      return epsClose;
}

std::set<State*> DFA::Tmove(const std::set<State*>& group, NFA& nfa, char c) {
      std::set<State*> next;
      for (auto s : group) {
            auto range = nfa.table.equal_range(std::make_pair(s,c));
            for (auto it = range.first; it != range.second; ++it) {
                  next.insert(it->second);
            }
      }
            return next;
}

void DFA::DFS(State* cur, NFA& nfa, std::set<State*>& states) {
      if (states.contains(cur)) return;
      states.insert(cur);
      auto it = nfa.table.equal_range(std::make_pair(cur,'$'));
      for (auto t = it.first; t != it.second; ++t) {
            DFS(t->second,nfa,states);
      }
}



void DFA::dumpDot(const std::string& filename) {
      std::ofstream out(filename);
      out << "digraph DFA {" << std::endl;
      out << "    rankdir=LR;" << std::endl;
      out << "    node [shape = circle];" << std::endl;
      for (int final : finalDFA) {
            out << "    " << final << " [shape = doublecircle];" << std::endl;
      }
      out << "    node [shape = none]; \"\" -> " << startDFA << ";" << std::endl;
      out << "    node [shape = circle];" << std::endl;
      for (auto const& [fromId, charMap] : transitionTable) {
            for (auto const& [symbol, toId] : charMap) {
                  out << "    " << fromId << " -> " << toId
                      << " [label = \"" << symbol << "\"];" << std::endl;
            }
      }
      out << "}" << std::endl;
      out.close();
}