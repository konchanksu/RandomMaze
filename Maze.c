#include <stdio.h>
#include <handy.h>
#include <stdlib.h>
#include <time.h>

#define ROAD 0
#define PILLAR 2
#define WALL 1
#define START 4
#define GOAL 3
#define NEWWALL 10
#define ARRAYSIZEX 91
#define ARRAYSIZEY 61
#define MAPSIZEX 1010
#define MAPSIZEY 710
#define MAPOUTBLANK 50
#define BOXSIZE 10

int pillar_max = (ARRAYSIZEX - 3) * (ARRAYSIZEY - 3) / 4;

/*
    wall = 3
    pillar = 1
    road 0;
*/

// 迷路盤面の配列は構造体で管理
typedef struct{
	int arrayx;
	int arrayy;
}coord_t;

//　タプルを定義
coord_t newcoord(x, y){
    coord_t new;
    new.arrayx = x;
    new.arrayy = y;
    return new;
}

//　bfs用の情報管理
typedef struct{
    int arrayx;
    int arrayy;  //座標
    int lastcoursex;
    int lastcoursey;  //最後に進んだ方向(ぐるぐる回らないように),最後に辿っていく
}bfs_st;


bfs_st bfs_inf(x, y, coursex, coursey){
    bfs_st newc;
    newc.arrayx = x;
    newc.arrayy = y;
    newc.lastcoursex = coursex;
    newc.lastcoursey = coursey;
    return newc;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

//  ランダムな迷路の作成

///////////////////////////////////////////////////////////////////////////////////////////////////
//　配列に盤面情報を代入
int insert_mapdata(int meiroarray[][ARRAYSIZEY]){
    int i, j;
    for(i = 0; i < ARRAYSIZEX; i++){
        for(j = 0; j < ARRAYSIZEY; j++){
            if(i == 0 || j == 0 || i == ARRAYSIZEX - 1 || j == ARRAYSIZEY - 1){
                meiroarray[i][j] = WALL;
            }else if(i % 2 == 0 && j % 2 == 0){
                meiroarray[i][j] = PILLAR;
            }else if(i == 1 && j == 1){
                meiroarray[i][j] = START;
            }else if(i == ARRAYSIZEX - 2 && j == ARRAYSIZEY - 2){
                meiroarray[i][j] = GOAL;
            }else{
                meiroarray[i][j] = ROAD;
            }
        }
    }
    return 0;
}

//　柱をカウント、配列を構造体型で作成
int check_pillar(int meiroarray[][ARRAYSIZEY], coord_t coord_pillar[pillar_max]){
    int i, j;
    int count = 0;
    for(i = 0; i < ARRAYSIZEX; i++){
        for(j = 0; j < ARRAYSIZEY; j++){
            if(meiroarray[i][j] == PILLAR){
                coord_pillar[count] = newcoord(i, j);
                //printf("%d %d\n", coord_pillar[count].arrayx, coord_pillar[count].arrayy);
                count++;
            }
        }
    }
    return 0;
}


// 柱の順番をシャッフル
int shaffle_pillar(int meiroarray[][ARRAYSIZEY], coord_t coord_pillar[pillar_max]){
    int hogex, hogey;
    int i = 0;
    int change_a, change_b;
    coord_t tmp;
    while(i < 100){
        change_a = random() % pillar_max;
        change_b = random() % pillar_max;
        if(change_a != change_b){
            tmp = coord_pillar[change_a];
            coord_pillar[change_a] = coord_pillar[change_b];
            coord_pillar[change_b] = tmp;
        }
        i++;
    }
    return 0;
}


//　壁の塗り替え作業
int change_newwall(int meiroarray[][ARRAYSIZEY], int reset){
    int i, j;
    for(i = 0; i < ARRAYSIZEX; i++){
        for(j = 0; j < ARRAYSIZEY; j++){
            if(meiroarray[i][j] == NEWWALL){
                if(reset == -1){
                    meiroarray[i][j] = WALL;
                }else if(reset == 1){
                    if(i % 2 == 0 && j % 2 == 0){
                        meiroarray[i][j] = PILLAR;
                    }else{
                        meiroarray[i][j] = ROAD;
                    }
                }
            }
        }
    }
    return 0;
}


//finish = -1 reset = 1
int extend_newwall(int meiroarray[][ARRAYSIZEY], int newwallx, int newwally, int reset){
    if(reset == 0){
        int root;

        coord_t course[4] = {newcoord(0, 1), newcoord(0, -1), newcoord(1, 0), newcoord(-1, 0)};
        root = random() % 4;

        newwallx += course[root].arrayx;
        newwally += course[root].arrayy;
        meiroarray[newwallx][newwally] = NEWWALL;

        newwallx += course[root].arrayx;
        newwally += course[root].arrayy;
        if(meiroarray[newwallx][newwally] == WALL){
            reset = -1;
        }else if(meiroarray[newwallx][newwally] == NEWWALL){
            reset = 1;
        }else{
            meiroarray[newwallx][newwally] = NEWWALL;
            reset = extend_newwall(meiroarray, newwallx, newwally, reset);
        }
    }
    return reset;
}


int extend_wall(int meiroarray[][ARRAYSIZEY], coord_t coord_pillar[pillar_max]){
    int i;
    int reset = 0;
    int newwallx, newwally;

    for(i = 0; i < pillar_max; i++){
        reset = 0;

        newwallx = coord_pillar[i].arrayx;
        newwally = coord_pillar[i].arrayy;

        if(meiroarray[newwallx][newwally] != WALL){
            meiroarray[newwallx][newwally] = NEWWALL;
            reset = extend_newwall(meiroarray, newwallx, newwally, reset);
        }

        if(reset == 1){
            i--;
        }
        change_newwall(meiroarray, reset);
    }
    return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////

// 迷路を幅優先探索でルートを調べる

///////////////////////////////////////////////////////////////////////////////////////////////////

int bfs(int meiroarray[][ARRAYSIZEY], bfs_st now_positions[10000], bfs_st queue_position[10000]){
    coord_t course[4] = {newcoord(0, 1), newcoord(0, -1), newcoord(1, 0), newcoord(-1, 0)};
    bfs_st queue;
    bfs_st new_queue;
    int end = 0;
    int now_position = 0;
    int top_queue = 0;  //
    int i;
    while(end == 0){
        // dequeue
        queue = queue_position[0];
        if(top_queue > 0){
            for(i = 1; i <= top_queue; i++){
                queue_position[i - 1] = queue_position[i];
            }
            top_queue--;
        }

        for(i = 0; i < 4; i++){
            if(queue.lastcoursex != -course[i].arrayx || queue.lastcoursey != -course[i].arrayy){
                new_queue.arrayx = course[i].arrayx + queue.arrayx;
                new_queue.arrayy = course[i].arrayy + queue.arrayy;
                new_queue.lastcoursex = course[i].arrayx;
                new_queue.lastcoursey = course[i].arrayy;

                if(new_queue.arrayx <= 0 || new_queue.arrayy <= 0 || new_queue.arrayx >= MAPSIZEX - 1 || new_queue.arrayy >= MAPSIZEY - 1){
                    continue;
                }

                if(meiroarray[new_queue.arrayx][new_queue.arrayy] == GOAL){
                    end = 1;
                }else if(meiroarray[new_queue.arrayx][new_queue.arrayy] == ROAD){
                    // enqueue
                    //printf("%d %d   %d %d\n", new_queue.lastcoursex, new_queue.lastcoursey, new_queue.arrayx, new_queue.arrayy);
                    queue_position[top_queue] = new_queue;
                    top_queue++;
                }else{
                    continue;
                }
                now_position++;
                now_positions[now_position] = new_queue;
            }
        }
    }
    return now_position;
}


//　最後に正解のルートをゴールからスタートに向けて進ませていき、meiroarrayの値を変更する作業
int check_root(int meiroarray[][ARRAYSIZEY], bfs_st now_positions[10000], int now_position){
    int coordx, coordy;
    int nextx, nexty;
    for(int i = now_position; i >= 0; i--){
        coordx = now_positions[i].arrayx;
        coordy = now_positions[i].arrayy;
        if(i == now_position){
            meiroarray[coordx][coordy] = GOAL;
            nextx = coordx - now_positions[i].lastcoursex;
            nexty = coordy - now_positions[i].lastcoursey;
        }else if(nextx == coordx && nexty == coordy){
            meiroarray[coordx][coordy] = GOAL;
            nextx = coordx - now_positions[i].lastcoursex;
            nexty = coordy - now_positions[i].lastcoursey;
        }
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

//　描画

//////////////////////////////////////////////////////////////////////////////////////////////////

//　色の設定
int check_color(int color){
    if(color == ROAD) HgSetFillColor(HG_WHITE);
    else if(color == WALL) HgSetFillColor(HG_BLUE);
    else if(color == START || color == GOAL) HgSetFillColor(HG_RED);

    return 0;
}

//　描写、　使い回し推奨
int discription(int meiroarray[][ARRAYSIZEY], int first){
    int color;
    int i, j;
    for(i = 0; i < ARRAYSIZEX; i++){
        for(j = 0; j < ARRAYSIZEY; j++){
            if(meiroarray[i][j] == START || meiroarray[i][j] == GOAL || first != 1){
                color = meiroarray[i][j];
                check_color(color);
                HgBoxFill(MAPOUTBLANK + i * BOXSIZE, MAPOUTBLANK + j * BOXSIZE, BOXSIZE, BOXSIZE, 1);
            }else{
                color = meiroarray[i][j];
                check_color(color);
                HgBoxFill(MAPOUTBLANK + i * BOXSIZE, MAPOUTBLANK + j * BOXSIZE, BOXSIZE, BOXSIZE, 1);
            }
        }
    }
    return 0;
}




int main(){
    srandom(time(NULL));
    int meiroarray[ARRAYSIZEX][ARRAYSIZEY] = {0};
    int now_position;
    int first = 1;
    coord_t coord_pillar[pillar_max];
    bfs_st now_positions[10000];
    bfs_st queue_position[10000];
    now_positions[0] = bfs_inf(1, 1, 0, 0);
    queue_position[0] = bfs_inf(1, 1, 0, 0);

    insert_mapdata(meiroarray);
    check_pillar(meiroarray, coord_pillar);
    shaffle_pillar(meiroarray, coord_pillar);
    extend_wall(meiroarray, coord_pillar);

    HgOpen(MAPSIZEX, MAPSIZEY);
    discription(meiroarray, first);
    HgGetChar();

    now_position = bfs(meiroarray, now_positions, queue_position);
    first = 0;
    check_root(meiroarray, now_positions, now_position);
    discription(meiroarray, first);

    HgGetChar();
    HgClose();
    return 0;
}
