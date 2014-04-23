#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

constexpr int oo = 64*64;

enum Color
{
    NONE,
    BLACK,
    WHITE
};

struct Othello
{
    Color board[8][8];
    Color turn;
};

Color inv(Color c)
{
    switch (c)
    {
        case  NONE: return  NONE;
        case BLACK: return WHITE;
        case WHITE: return BLACK;
        default:    return  NONE;
    }
}

Othello const initial_layout = {
    {{NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
     {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
     {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
     {NONE, NONE, NONE,WHITE,BLACK, NONE, NONE, NONE},
     {NONE, NONE, NONE,BLACK,WHITE, NONE, NONE, NONE},
     {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
     {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE},
     {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE}},
    BLACK
};

struct Move
{
    int r;
    int c;
};

Move const dirs[8] = {
    { 0,  1},
    { 0, -1},
    { 1,  0},
    { 1,  1},
    { 1, -1},
    {-1,  0},
    {-1,  1},
    {-1, -1}
};

template <typename O>
auto cellAt(O& o, Move const& m)
    -> decltype(o.board[m.r][m.c])
{
    return o.board[m.r][m.c];
}

bool isOnBoard(Move const& m)
{
    return (m.r>=0 and m.r<8 and m.c>=0 and m.r<8);
}

Move operator+(Move const& a, Move const& b)
{
    return {a.r+b.r, a.c+b.c};
}

void applyMove(Othello& o, Move const& m)
{
    vector<Move> hits;
    
    for (auto& dir : dirs)
    {
        int status = 0;
        for (Move cm = m+dir; isOnBoard(cm); cm=cm+dir)
        {
            auto cell = cellAt(o,cm);
            
            if (status==0)
            {
                if (cell==inv(o.turn))
                    ++status;
                else
                    break;
            }
            else
            {
                if (cell==o.turn)
                {
                    hits.emplace_back(cm);
                    break;
                }
                else if (cell==NONE)
                {
                    break;
                }
            }
        }
    }
    
    for (auto& h : hits)
        cellAt(o,h) = o.turn;
    
    cellAt(o,m) = o.turn;
    o.turn = inv(o.turn);
}

int getMoveScore(Othello const& o, Move const& m)
{
    int rv = 0;

    for (auto& dir : dirs)
    {
        int status = 0;
        for (Move cm = m+dir; isOnBoard(cm); cm=cm+dir)
        {
            auto cell = cellAt(o,cm);
            
            if (status==0)
            {
                if (cell==inv(o.turn))
                    ++status;
                else
                    break;
            }
            else
            {
                if (cell==o.turn)
                {
                    rv += status;
                    break;
                }
                else if (cell==NONE)
                {
                    break;
                }
                else
                {
                    ++status;
                }
            }
        }
    }
    
    return rv;
}

vector<pair<Move,int>> getAllMoves(Othello const& o)
{
    vector<pair<Move,int>> rv;
    
    for (Move m={0,0}; m.r<8; ++m.r)
    {
        for (m.c=0; m.c<8; ++m.c)
        {
            if (cellAt(o,m)!=NONE) continue;
            
            int score = getMoveScore(o,m);
            if (score>0)
                rv.emplace_back(m,score);
        }
    }
    
    return rv;
}

pair<Move,int> getBestMove(Othello const& o)
{
    auto moves = getAllMoves(o);
    
    if (moves.empty())
        return {{0,0},-oo};
    
    return *max_element(begin(moves), end(moves),
        [](pair<Move,int> const& a, pair<Move,int> const& b)
        {
            return a.second<b.second;
        }
    );
}

pair<Move,int> getSmartMove(Othello const& o)
{
    auto allmoves = getAllMoves(o);
    
    pair<Move,int> best {{0,0},-oo};
    
    for (auto& m : allmoves)
    {
        Othello tmp = o;
        applyMove(tmp,m.first);
        auto opbest = getBestMove(tmp);
        int score = m.second-opbest.second;
        if (score>best.second)
            best = {m.first,score};
    }
    
    return best;
}

void pass(Othello& o)
{
    o.turn = inv(o.turn);
}

bool aiMove(Othello& o)
{
    auto m = getSmartMove(o);
    
    if (m.second == -oo)
    {
        pass(o);
        cout << -1 << endl;
        return false;
    }
    else
    {
        applyMove(o,m.first);
        cout << m.first.r*8+m.first.c << endl;
        return true;
    }
}

bool opponentMove(Othello& o)
{
    int a;
    
    cin >> a;
    
    if (a<0)
    {
        pass(o);
        return false;
    }
    else
    {
        applyMove(o,{a/8,a%8});
        return true;
    }
}

int main()
{
    Othello o = initial_layout;
    
    string p;
    cin >> p;
    
    if (p=="w")
    {
        o.turn = WHITE;
        opponentMove(o);
    }
    
    bool running = true;
    while (running)
    {
        bool a = aiMove(o);
        bool b = opponentMove(o);
        running = a or b;
    }
}
