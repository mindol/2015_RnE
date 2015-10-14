#include <cstdio>
#include <cmath>

struct state {
    double temperature [2][180][360];
    //double pressure    [2][180][360]; /* Not implemented */
    int year, month, day;
    int hour;
};

/// Transition; write the next state into 'next'.

struct vector3{
    double x,y,z;
    vector3(){ x=y=z=0; }
    vector3(double a,double b,double c){ x=a; y=b; z=c; }
    double size(){ return sqrt(x*x+y*y+z*z); }
    vector3 operator*(double k)
    {
        return vector3(k*x,k*y,k*z);
    }
    vector3 operator/(double k)
    {
        return vector3(x/k,y/k,z/k);
    }
    vector3 inv()
    {
        double my_size = size();
        return operator/(my_size)/my_size;
    }
};
double dot(vector3 a,vector3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }

int dx[26]={1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int dy[26]={-1,-1,-1,0,0,0,1,1,1,-1,-1,-1,0,0,1,1,1,-1,-1,-1,0,0,0,1,1,1};
int dz[26]={-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1};
vector3 dr[26];

const double total_flux;

void simulate(state before, state& next)
{
    vector3 wind_map[2][180][360];
    double volume_flux[2][180][360];
    double temp_sum[2][180][360];
    int i,j,k;
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k){
        wind_map    [i][j][k]=vector3(0,0,0);
        volume_flux [i][j][k]=0;
        temp_sum    [i][j][k]=0;
    }
    int dir;
    int next_i, next_j, next_k;
    double my_out, my_left;
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k){
        my_out=0;
        for(dir=0;dir<26;++dir)
        {
            next_i = i + dx[dir];
            next_j = j + dy[dir];
            next_k = k + dz[dir];
            if(next_i>=0 && next_j>=0  && next_k>=0 &&
               next_i<2  && next_j<180 && next_k<360){
                double cur_volume = dot(wind_map[i][j][k],dr[dir].inv());
                if(cur_volume<0) continue;
                volume_flux[next_i][next_j][next_k]+=cur_volume;
                temp_sum   [next_i][next_j][next_k]+=cur_volume*before.temperature[i][j][k];
                my_out+=cur_volume;
            }
        }
        my_left = total_flux - my_out;
        volume_flux[i][j][k] += my_left;
        temp_sum   [i][j][k] += my_left*before.temperature[i][j][k];
    }
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k) next.temperature[i][j][k]=temp_sum[i][j][k]/volume_flux[i][j][k];

}

state current_state;

/// Read data into current_state.

void read_data()
{
    //
}

void init()
{
    for(int dir=0;dir<26;++dir) dr[dir]=vector3(dx[dir],dy[dir],dz[dir]);
}

int main()
{
    init();
    read_data();
    int i;
    state temp_state;
    for(i=0;i<100;++i){
        simulate(current_state,temp_state);
        current_state=temp_state;
    }
    return 0;
}
