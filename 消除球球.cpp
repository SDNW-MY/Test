//特性：当鼠标在发射口下方时，子弹无效
//建议：鼠标瞄准点击发射子弹时，敌人才消失，（现在在范围内，瞄准敌人就消失）
//想法：可以稍加修改，参照吸血鬼幸存者

//w=x；h=y；
//发射线宽10，方向误差0.05，合适参数(小范围）
//发射线宽1，方向误差0.01，合适参数（大范围）

#include<graphics.h>
#include<conio.h>
#include<math.h>
#include<iostream>
#include<time.h>

using namespace std;

struct game_window
{
    int wide;
    int height;
};

struct launch
{
    int w, h;
    int r;
};

struct line_sight//瞄准线
{
    int s_w, s_h;
    int e_w, e_h;
    double sin, cos;
    COLORREF color;
};

struct enemy
{
    int w, h;
    double sin, cos;
    int r;
    COLORREF color;
};

struct bullet
{
    int w, h;
    double sin, cos;
    int r;//子弹的半径
    bool active;//子弹是否可见
    int to_b_lau;//子弹和发射口之间的距离
    COLORREF color;
};


const int e_max = 100;//敌人的最大个数
const int e_a_r = 200;//发射的有效攻击范围
const int line_wide = 10; //发射线宽
const double e_range = 0.05;//方向误差
const int b_max = 150;//子弹的最大数量
int b_delay_t = 20;//子弹多少帧发射一次

int b_index = 0;//记录当前发射的子弹是哪一颗
int g_f = 0;//游戏帧数，计算游戏经过了多少帧（每到1e5重置）


game_window g_w;
launch lau;
MOUSEMSG m;
line_sight line_aim;//瞄准线实例
enemy e[e_max] = { 0 };
bullet b[b_max] = { 0 };

void set();
void show();
//计算瞄准线的位置
void c_line_aim();

//计算敌人的位置
void c_e();

//计算瞄准线和敌人之间，敌人是否被消除
void c_line_a_to_e();

//计算单个敌人对发射口的距离
int c_e_to_lau(enemy& e);

//计算敌人和瞄准线是否在同一个方向上（偏差内）
bool between(enemy e, launch lau, double k);

//计算子弹的位置
void c_b();

//计算两点之间的距离
int c_dis_bet_t_p(int w1, int h1, int w2, int h2);

//计算游戏帧数（已经经过的帧数）
void c_g_f();

int main()
{
    set();

    initgraph(g_w.wide, g_w.height);

    while (true)
    {
        show();
        

        c_line_aim();
        c_e();
        c_b();
        c_line_a_to_e();
        c_g_f();

        Sleep(5);
        cleardevice();
    }

    _getch();

    closegraph();
}
void c_g_f()
{
    g_f++;
    if (g_f>1e5)
    {
        g_f = 0;
    }
}

int c_dis_bet_t_p(int w1, int h1, int w2, int h2)
{
    int w = abs(w2 - w1);
    int h = abs(h2 - h1);
    double dis = sqrt(w * w + h * h);
    return dis;
}

void c_b()
{
    for (int i = 0; i < b_max; i++)
    {
        if (b[i].active)
        {
            b[i].to_b_lau++;

            //b[i].w = line_aim.s_w+line_aim.cos * b[i].to_b_lau;//距离炮口缓慢离开(子弹紧靠瞄准线方向发射)
            //b[i].h = line_aim.s_h - fabs(line_aim.sin) * b[i].to_b_lau;

            //b[i].w += line_aim.cos * b[i].to_b_lau;//距离炮口加速离开(子弹紧靠沿着瞄准线方向发射)
            //b[i].h -=  fabs(line_aim.sin) * b[i].to_b_lau;


            b[i].w = line_aim.s_w+b[i].cos * b[i].to_b_lau;//距离炮口缓慢离开(子弹不依靠瞄准线方向发射)
            b[i].h = line_aim.s_h - fabs(b[i].sin) * b[i].to_b_lau;
        }
        else
        {
            b[i].to_b_lau = 0;
            b[i].w = lau.w;
            b[i].h = lau.h;
        }

        //子弹飞出屏幕的情况
        //if (abs(b[i].w - lau.w) >= g_w.wide / 2)
        if (b[i].w < 0 || b[i].w>g_w.wide)
        {
            b[i].active = false;
        }
        if (b[i].h < 0||b[i].h>g_w.height)
        {
            b[i].active = false;
        }
    }
}

bool between(enemy e, launch lau, double k)
{
    bool f = false;
    //if (e.sin>line_aim.sin-k && e.sin < line_aim.sin + k)
    //{
    //    if (e.cos > line_aim.cos - k && e.cos < line_aim.cos + k)
    //    {
    //        f = true;
    //    }
    //}

    if (fabs(line_aim.sin-e.sin)<k)
    {
        if (fabs(line_aim.cos - e.cos) < k)
        {
            f = true;
        }
    }

    return f;
}

int c_e_to_lau(enemy& e)
{
    int a = e.w - lau.w;//w
    int b = lau.h - e.h;//h
    double c = sqrt(a * a + b * b);
    e.sin = (b / c);
    e.cos = (a / c);
    return c;
}

void c_line_a_to_e()
{
    for (int i = 0; i < e_max; i++)
    {
        //c_e_to_lau(e[i]);
        if (c_e_to_lau(e[i]) < e_a_r)
        {
            //if (between(e[i], lau, e_range))//不再需要判断瞄准线是否和敌人在一条线上了，直接通过子弹位置来判断
            {
                for (int j = 0; j < b_max; j++)
                {
                    if (c_dis_bet_t_p(e[i].w, e[i].h, b[j].w, b[j].h) <= e[i].r + b[j].r)
                    {
                        b[j].active = false;
                        e[i].w = rand() % g_w.wide;
                        e[i].h = rand() % lau.h - lau.h;
                    }
                }
            }
        }
    }
}

void c_e()
{
    for (int i = 0; i < e_max; i++)
    {
        if (e[i].h < g_w.height)
        {
            e[i].h++;
        }
        else
        {
            e[i].w = rand() % g_w.wide;
            e[i].h = rand() % lau.h - lau.h;
        }
    }
}

void c_line_aim()
{
    //m = GetMouseMsg();
    PeekMouseMsg(&m);

    if (m.mkLButton)
    {
        line_aim.color = GREEN;
        if (b[b_index].active == false && (g_f % b_delay_t == 0))
        {
            b[b_index].active = true;
            b[b_index].sin = line_aim.sin;
            b[b_index].cos = line_aim.cos;
            b_index++;
        }
    }
    else
    {
        line_aim.color = WHITE;
    }

    //判断子弹索引是否越界
    if (b_index >= b_max)
    {
        b_index = 0;
    }

    int m_l_lenth = (g_w.wide - lau.w)+(lau.h);

    int a = m.x - lau.w;//w
    int b = lau.h - m.y;//h
    double c = sqrt(a* a + b*b);
    line_aim.sin = (b / c);
    line_aim.cos = (a / c);

    line_aim.e_w = line_aim.s_w + line_aim.cos * m_l_lenth;
    line_aim.e_h = line_aim.s_h - fabs(line_aim.sin) * m_l_lenth;

}

void set()
{
    g_w.wide = 400;
    g_w.height = 600;

    lau.w = g_w.wide / 2;
    lau.h = g_w.height / 3 *2;
    lau.r = 10;

    line_aim.s_w = lau.w;
    line_aim.s_h = lau.h;

    srand(time(0));

    for (int i = 0; i < e_max; i++)
    {
        e[i].w = rand()%g_w.wide;
        e[i].h = rand()% lau.h -lau.h;
        e[i].color = WHITE;
        e[i].r = 5;
    }

    for (int i = 0; i < b_max; i++)
    {
        b[i].h = lau.h;
        b[i].w = lau.w;
        b[i].color = RED;
        b[i].active = false;
        b[i].r = line_wide+3;
        b[i].to_b_lau = 0;
    }

    b_index = 0;
    g_f = 0;
}

void show()
{
    BeginBatchDraw();

    //画发射口
    circle(lau.w, lau.h, lau.r);
    circle(lau.w, lau.h, e_a_r);
    
    //画瞄准线
    LINESTYLE* p=new LINESTYLE;
    getlinestyle(p);
    setlinestyle(NULL, line_wide, NULL, NULL);
    setcolor(line_aim.color);
    line(line_aim.s_w, line_aim.s_h, line_aim.e_w, line_aim.e_h);
    setlinestyle(p);
    setcolor(WHITE);

    //画敌人
    for (int i = 0; i < e_max; i++)
    {
        circle(e[i].w, e[i].h, e[i].r);
    }

    //画子弹位置
    p = new LINESTYLE;
    getlinestyle(p);
    setlinestyle(NULL, 3, NULL, NULL);
    for (int i = 0; i < b_max; i++)
    {
        if (b[i].active)
        {
            setcolor(b[i].color);
            
            circle(b[i].w, b[i].h, b[i].r);
            setcolor(WHITE);
        }
    }
    setlinestyle(p);


    //显示鼠标位置
    WCHAR s[50];
    _stprintf_s(s, _T("[%d,%d]"), m.x, m.y);
    outtextxy(0, 0, s);
    //_stprintf_s(s, _T("瞄准线：[%f,%f]"), line_aim.sin, line_aim.cos);
    //outtextxy(0, 15, s);
    //int T = 1;
    //for (int i = 0; i < e_max; i++)
    //{
    //    if (c_e_to_lau(e[i]) < e_a_r)
    //    {
    //        _stprintf_s(s, _T("瞄准范围内敌人：[%f,%f]"), e[i].sin, e[i].cos);
    //        outtextxy(0, 15+T++*15, s);
    //    }
    //}
    // 
    //int T = 1;
    //for (int i = 0; i < b_max; i++)
    //{
    //    if (b[i].active)
    //    {
    //        _stprintf_s(s, _T("子弹坐标：[%d,%d][%d]"), b[i].w, b[i].h,b_index);
    //        outtextxy(0, 15+T++*15, s);
    //    }
    //}


    EndBatchDraw();
}

// 菜ji