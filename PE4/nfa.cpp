    #include "nfa.hpp"
    
    // private:
    bool NFA::has_state(const std::string& state) const{
    
        for(std::string curstate : all_states){
            if(curstate == state) return true;
        }
        return false;
    }
    
    bool NFA::is_final_state(const std::string& state) const{
        for(std::string curstate : final_states){
            if(curstate == state) return true;
        }
        return false;
    
    }
    
    // public:
    
    bool NFA::is_DFA() const{
        for (const auto& state : all_states) {
            for (char symbol : alphabet.get_symbols()) {
                if (transitions(state, symbol).size() != 1) { // Check if there is not exactly one transition for the symbol
                    return false;
                }
            }
            // Checking empty transitions
            if(transitions(state,'e').size() != 0) return false;
        }
        return true;
    }
    
    void NFA::update_state_name(const std::string& old_name, const std::string& new_name){
        if (starting_state == old_name) starting_state= new_name;
        if (all_states.find(old_name) != all_states.end()){
            all_states.erase(all_states.find(old_name));
        }
        all_states.insert(new_name);
        
        if (final_states.find(old_name) != final_states.end()){
            final_states.erase(final_states.find(old_name));
        }
        final_states.insert(new_name);
        transitions.update_state_name(old_name,new_name);
        return;
    }
    

    #include "nfa.hpp"

    bool NFA::process(std::string input) {
        // Check if the input contains only symbols from the alphabet
        if (!alphabet.is_valid(input)) {
            std::cout << "Invalid string\n";
            return false;
        }
    
        // Initialize the queue with the starting state and the entire input
        std::queue<ComputationBranch> Q;
        ComputationBranch initialBranch(starting_state, input);
        Q.push(initialBranch);
    
        bool foundAccepting = false;
        ComputationBranch lastBranch;
    
        // Process the queue while it is not empty
        while (!Q.empty()) {
            ComputationBranch branch = Q.front();
            Q.pop();
            lastBranch = branch;  // Store the last processed branch for possible rejection output
    
            std::string curr_state = branch.get_last_config().first;
            std::string remaining_input = branch.get_last_config().second;
    
            // Handle ε-transitions from the current state
            auto epsilon_transitions = transitions(curr_state, 'e');
            for (const std::string& next_state : epsilon_transitions) {
                ComputationBranch newBranch(branch); // Create a new branch from the current
                newBranch.push_config(next_state, remaining_input); // No input consumed
                Q.push(newBranch);
            }
    
            // If no remaining input, check if current state is a final state
            if (remaining_input.empty() && is_final_state(curr_state)) {
                std::cout << branch << std::endl;
                std::cout << "Accept\n";
                foundAccepting = true;
                break; // If we find an accepting state, we stop processing further branches
            }
    
            // Handle symbol transitions if there is remaining input
            if (!remaining_input.empty()) {
                char symbol = remaining_input[0];
                std::string next_input = remaining_input.substr(1);
                auto symbol_transitions = transitions(curr_state, symbol);
                for (const std::string& next_state : symbol_transitions) {
                    ComputationBranch newBranch(branch); // Create a new branch from the current
                    newBranch.push_config(next_state, next_input); // Consume one character of input
                    Q.push(newBranch);
                }
            }
        }
    
        // If no accepting branch was found and we finished processing the queue
        if (!foundAccepting) {
            std::cout << lastBranch << std::endl;  // Print the last checked branch
            std::cout << "Reject\n";
        }
    
        return foundAccepting;
    }

    
    
        
    NFA NFA::operator!() const{
        if (!is_DFA()) {
            std::cout << "Not a DFA" << std::endl;
            // Return an NFA equivalent to the current object
            return *this;
        }
        NFA complement = *this;
        std::set<std::string> all_states_copy = all_states;
        std::set<std::string> final_states_copy;
        for (const std::string& state : all_states_copy) {
            if (final_states.find(state) == final_states.end()) {
                // If the state is not in the final states, add it to the final states copy
                final_states_copy.insert(state);
            }
        }
        complement.final_states = final_states_copy;
    
        return complement;
    
    }
    NFA NFA::operator+(const NFA& other) const {
        // Create a new NFA instance for the union
        NFA result = *this;
        NFA backup = other;
        // Merge the alphabet
        result.alphabet = this->alphabet;
        result.alphabet += other.alphabet;
        
        // Merge the states
        result.all_states = this->all_states;
        std::set<std::string> other_states = other.all_states;
    
        // Update state names in other NFA to avoid conflicts and add them to result's states
        for (std::string state : other_states) {
            std::string new_state_name = state;
            // Check for conflicts with existing state names in result
            while (result.has_state(new_state_name)) {
                // Prepend "q" to the state name
                new_state_name = "q" + new_state_name;
                
            }
            // Add the updated state name to result's states
            if(other.is_final_state(state)) result.final_states.insert(new_state_name);
            result.all_states.insert(new_state_name);
            
            // Update state name in backup NFA
        
            backup.update_state_name(state, new_state_name);
        }
    
        // Merge the final states
        
    
        result.transitions = backup.transitions;
        result.transitions += transitions;
        
        // Add the new starting state and transitions
        std::string new_starting_state = "s";
        // Check for conflicts with existing state names in result
        while (result.has_state(new_starting_state)) {
            // Prepend "q" to the new starting state name
            new_starting_state = "s" + new_starting_state;
        }
        // Add the new starting state to result's states
        result.all_states.insert(new_starting_state);
        // Add epsilon transitions to the starting state of both NFAs
        result.transitions.add_rule(new_starting_state, 'e', backup.starting_state);
        result.transitions.add_rule(new_starting_state, 'e', this->starting_state);
        
        
        
    
    
        // Set the new starting state
        result.starting_state = new_starting_state;
    
        // Merge the transitions
        
    
        return result;
    }
    
    
    
