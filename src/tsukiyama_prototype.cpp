#include<iostream>
#include<vector>
#include<bitset>

using namespace std;

#define N 8

vector<vector<int> > adj_mat = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 1, 1},
};

class PointSet {
public:
    bitset<8> set;

    PointSet() {
        set.reset();
    }

    void operator+=(int i) {
        set[i] = 1;
    }

    void operator-=(int i) {
        set[i] = 0;
    }

    bool empty(){
        return set.none();
    }

    PointSet competitors(int i){
        PointSet ret;
        for(int j = 0; j < 8; j++){
            if(adj_mat[i][j] == 1){
                ret += j;
            }
        }
        return ret;
    }

    bool contains(int i) {
        return set[i];
    }

    bool connected(int i){
        for(int j = 0; j < 8; j++){
            if(this->contains(j) && !adj_mat[i][j]){
                return false;
            }
        }
        return true;
    }

    PointSet closure(){
        if(this->empty())
            return PointSet::K0();
        PointSet ret = *this;
        for(int j = 0; j < 8; j++){
            if(!ret.contains(j) && ret.connected(j)) ret += j;
        }
        return ret;
    }

    PointSet neighboursof(int i){
        PointSet ret;
        for(int j = 0; j < 8; j++){
            if(this->contains(j) && adj_mat[i][j]) ret += j;
        }
        return ret;
    }

    // <= i
    PointSet slice(int i){
        PointSet ret = *this;
        for(int j = i + 1; j < 8; j++){
            ret -= j;
        }
        return ret;
    }

    static PointSet cap(PointSet pset1, PointSet pset2){
        PointSet ret;
        for(int j = 0; j < 8; j++){
            if(pset1.contains(j) && pset2.contains(j)) ret += j;
        }
        return ret;
    }

    static PointSet K0(){
        PointSet ret;
        ret += 0;
        return ret.closure();
    }

    bool operator==(PointSet pset){
        return this->set == pset.set;
    }

    friend ostream& operator<<(ostream& os, PointSet pser);
};

ostream& operator<<(ostream& os, PointSet pser){
    for(int i = 0; i < 8; i++){
        if(pser.contains(i)) os << i << " ";
    }
    return os;
}

vector<PointSet> CompSky;

void generateChildren(PointSet pmcs, int core_pmcs){
    cout << "pmcs: " << pmcs << endl;
    CompSky.emplace_back(pmcs);

    //condition (a)
    for(int i = core_pmcs + 1; i < N; i++){
        cout << "i = " << i << endl;
        //condition (b)
        if(pmcs.contains(i)) {
            cout << "Condition (b) is not satisfied" << endl;
            continue;
        }

        /**
         * condition (c)
         * K'_{<= i-1} = K_{<= i} \cap N(i) */
        PointSet K_child = PointSet::cap(pmcs.slice(i), pmcs.neighboursof(i));
        K_child += i;
        K_child = K_child.closure();
        if(!(K_child.slice(i - 1) == PointSet::cap(pmcs.slice(i), pmcs.neighboursof(i)))){
            cout << "(c) not satisfied" << endl;
            cout << K_child.slice(i - 1) << " != " << PointSet::cap(pmcs.slice(i), pmcs.neighboursof(i)) << endl;
            continue;
        }

        /**
         * condition (d)
         * K_{<= i} = C(K_{<= i} \cap N(i))_{<= i}
         */
        if(!(pmcs.slice(i) == PointSet::cap(pmcs.slice(i), pmcs.neighboursof(i)).closure().slice(i))) {
            cout << "(d) not satisfied" << endl;
            cout << pmcs.slice(i) << " != " << pmcs.slice(i).neighboursof(i).closure().slice(i) << endl;
            cout << pmcs.slice(i).neighboursof(i) << endl;
            continue;
        }

        generateChildren(K_child, i);
    }
}

void tsukiyama(){
    cout << "Computing tsukiyama ..." << endl;
    PointSet K0;
    K0 += 0;
    K0 = K0.closure();
    cout << "K0 = " << K0 << endl;
    generateChildren(K0, 0);
}

int main(void){
    tsukiyama();
    cout << CompSky.size() << endl;
    return 0;
}