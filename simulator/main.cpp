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

vector3 dr[26];

const double total_volume = 0.0;
const double wind_size    = 0.0;
const double PI = 2*acos(0);

int day_in_month(int year,int month)
{
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    else if (month == 2)
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) return 29;
        else return 28;
    } else return 31;
}

vector3 getWind(int i,int j)
{
    double pitch = (1-2*i)*sin((j/30.0 - 0.5)*PI);
    if(0<=j && j<30){
        double theta = 90-85*(30-j)/30.0;
        theta = theta/180*PI;
        return vector3(-sin(pitch), cos(pitch)*cos(theta) , -cos(pitch)*sin(theta));
    } else if(30<=j && j<60){
        double theta = 90-85*(j-30)/30.0;
        theta = theta/180*PI;        return vector3(-sin(pitch), -cos(pitch)*cos(theta) ,cos(pitch)*sin(theta));
    } else if(60<=j && j<90){
        double theta = 90-85*(90-j)/30.0;
        theta = theta/180*PI;
        return vector3(-sin(pitch), cos(pitch)*cos(theta) ,-cos(pitch)*sin(theta));
    } else if(90<=j && j<120){
        double theta = 90-85*(j-90)/30.0;
        theta = theta/180*PI;
        return vector3(-sin(pitch), -cos(pitch)*cos(theta) ,-cos(pitch)*sin(theta));
    } else if(120<=j && j<150){
        double theta = 90-85*(150-j)/30.0;
        theta = theta/180*PI;
        return vector3(-sin(pitch), cos(pitch)*cos(theta) ,cos(pitch)*sin(theta));
    } else if(150<=j && j<180){
        double theta = 90-85*(j-150)/30.0;
        theta = theta/180*PI;
        return vector3(-sin(pitch), -cos(pitch)*cos(theta) , -cos(pitch)*sin(theta));
    }
}

void simulate(state before, state& next)
{
    vector3 wind_map[2][180][360];
    double volume_flux[2][180][360];
    double temp_sum[2][180][360];
    int i,j,k;
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k){
        wind_map    [i][j][k]=getWind(i,j)*wind_size;
        volume_flux [i][j][k]=0;
        temp_sum    [i][j][k]=0;
    }
    int dir;
    int next_i, next_j, next_k;
    double my_out, my_left;
    int dx,dy,dz;
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k){
        my_out=0;
        for(dir=0;dir<26;++dir)
        {
            for(dx=-1; dx<=1; ++dx)
            for(dy=-1; dy<=1; ++dy)
            for(dz=-1; dz<=1; ++dz){
                if(dx==0 && dy==0 && dz==0) continue;
                next_i = i + dx;
                next_j = j + dy;
                next_k = k + dz;
                if(next_i>=0 && next_j>=0  && next_k>=0 &&
                   next_i<2  && next_j<180 && next_k<360){
                    double cur_volume = dot(wind_map[i][j][k],dr[dir].inv());
                    if(cur_volume<0) continue;
                    volume_flux[next_i][next_j][next_k]+=cur_volume;
                    temp_sum   [next_i][next_j][next_k]+=cur_volume*before.temperature[i][j][k];
                    my_out+=cur_volume;
                }
            }
        }
        my_left = total_volume - my_out;
        volume_flux[i][j][k] += my_left;
        temp_sum   [i][j][k] += my_left*before.temperature[i][j][k];
    }
    for(i=0;i<2;++i) for(j=0;j<180;++j) for(k=0;k<360;++k) next.temperature[i][j][k]=temp_sum[i][j][k]/volume_flux[i][j][k];
    next.year  = before.year;
    next.month = before.month;
    next.day   = before.day;
    next.hour  = before.hour + 1;
    if(next.hour == 24) {
        ++next.day;
        next.hour=0;
        if(next.day > day_in_month(next.year,next.month)){
            next.day=1; ++next.month;
            if(next.month==13){
                next.month=1; ++next.year;
            }
        }
    }
}

state current_state;

/// Read data into current_state.

void read_data(char* filename)
{
    FILE *f = fopen(filename,"r");
    int a,b;
    double c;
    int i,j;
    for(i=0;i<180;++i) for(j=0;j<360;++j)
        current_state.temperature[0][i][j]=current_state.temperature[1][i][j]=9999;
    while(fscanf(f,"%d%d%lf",&a,&b,&c)==3){
        current_state.temperature[0][a][b]=current_state.temperature[1][a][b]=c;
    }
    fclose(f);
    int p,q;
    double my_sum;
    int my_count;
    for(i=0;i<180;++i) for(j=0;j<360;++j){
        if(current_state.temperature[0][i][j]==9999){
            my_sum=0; my_count=0;
            for(a=-14;a<=14;++a) for(b=-14;b<=14;++b){
                p=i+a; q=j+b;
                if(0<=p && p<180 && 0<=q && q<360){
                    if(current_state.temperature[0][p][q]!=9999) my_sum+=current_state.temperature[0][p][q], ++my_count;
                }
            }
            if(my_count == 0) puts("bomb");
            current_state.temperature[0][i][j]=my_sum/my_count;
            current_state.temperature[1][i][j]=my_sum/my_count;
        }
    }
}

void write_data(char* filename)
{
    FILE 
    int i,j;
    for(i=0;i<180;++i) for(j=0;j<360;++j){
        printf("%d %d %f\n",)
    }
}

int main()
{
    //init();
    int i;
    read_data("startdata.txt");
    state temp_state;
    for(i=0;i<100;++i){
        simulate(current_state,temp_state);
        scanf("%*d");
        current_state=temp_state;
        
    }
    return 0;
}
