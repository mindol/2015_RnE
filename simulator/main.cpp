#include <cstdio>

struct state {
    double temperature [2][180][360];
    double pressure    [2][180][360];
    int year, month, day;
    int hour, minute;
};

/// Transition; write the next state into 'next'.

void simulate(state before, state& next)
{
    
}

state current_state;

/// Read data into current_state.

void read_data()
{
    
}

int main()
{
    read_data();
    int i;
    state temp_state;
    for(i=0;i<100;++i){
        simulate(current_state,temp_state);
        current_state=temp_state;
    }
    return 0;
}
