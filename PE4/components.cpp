#include "components.hpp"

/* operator<< overloads */
std::ostream& operator<<(std::ostream& os, Alphabet& a){
    for (char symbol : a.symbols) {
            os << symbol << " ";
        }

    return os;
}
std::ostream& operator<<(std::ostream& os, const Rule& r){
    os << r.initial_state << " " << r.symbol << " " << r.final_state;
    return os;
}
std::ostream& operator<<(std::ostream& os, TransitionTable& t){
    for(const Rule& rule : t.rules) {
        os << rule << std::endl; 
    }
    return os;

}
std::ostream& operator<<(std::ostream& os, const ComputationBranch& c){
    for (size_t i = 0; i < c.config_history.size(); ++i) {
        os << "(" << c.config_history[i].first << ", ";
        if (c.config_history[i].second.empty())
            os << "e";
        else
            os << c.config_history[i].second;
        os << ")";
        if (i != c.config_history.size() - 1) // Print :- only if it's not the last configuration
            os << " :- ";
    }
    return os;
}


/* Alphabet */
bool Alphabet::is_valid(const std::string& input) const{
    for(int i = 0; i < input.length() ; i++) {
        auto it = symbols.find(input[i]);
        
        if(it != symbols.end()) continue;
        else return false;
    }
    return true;
}

const std::set<char> Alphabet::get_symbols() const { 
    std::set<char> a;
    a = symbols;
    return a; 
}

Alphabet& Alphabet::operator+=(const Alphabet& other){
    for (char symbol : other.symbols) {
            symbols.insert(symbol);
        }
    return *this;
}


/* Rule */
Rule::Rule(const std::string& init_s, char symbol, const std::string& final_s){
    initial_state = init_s;
    this->symbol = symbol;
    final_state = final_s;
}

const std::string Rule::get_final_state() const {
    
    return final_state;
}

void Rule::update_state_name(const std::string& old_name, const std::string& new_name){
    if(old_name == initial_state) initial_state = new_name;
    if(old_name == final_state) final_state = new_name;
    return;
}

bool Rule::applies_to(const std::string& c, char s) const{
    if(c == initial_state && s == symbol) return true;
    return false;
}


/* TransitionTable */
void TransitionTable::add_rule(const std::string& initial_state, char symbol, const std::string& final_state){
    Rule newrule(initial_state, symbol, final_state);
    rules.push_back(newrule);
    return;
}

void TransitionTable::update_state_name(const std::string& old_name, const std::string& new_name){
    for(Rule& rule : rules){
        rule.update_state_name(old_name, new_name);
    }
    return;
}

const std::set<std::string> TransitionTable::operator()(const std::string& curr_state, char symbol) const{
    std::set<std::string> next_states;
    for(const Rule& rule : rules){
        if (rule.applies_to(curr_state, symbol)) next_states.insert(rule.get_final_state());
    }
    return next_states;
}

TransitionTable& TransitionTable::operator+=(const TransitionTable& other){
    // Iterate over the rules of the other TransitionTable and append them to the current object's rules
    for(const Rule& rule : other.rules){
        rules.push_back(rule);
    }
    return *this;
}



/* ComputationBranch */
void ComputationBranch::push_config(const std::string& state, const std::string& input){
    config_history.push_back(std::make_pair(state, input));
}

const std::pair<std::string,std::string> ComputationBranch::get_last_config() const{
    return config_history.back();
}